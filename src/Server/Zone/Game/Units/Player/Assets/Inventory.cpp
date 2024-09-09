/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 *
 * Copyright (c) 2019 Sagun K. (sagunxp@gmail.com).
 * Copyright (c) 2019 Horizon Dev Team.
 *
 * Base Author - Sagun K. (sagunxp@gmail.com)
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************/

#include "Inventory.hpp"

#include "Server/Zone/Game/Map/Grid/Notifiers/GridNotifiers.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainer.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainerVisitor.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Item/Item.hpp"
#include "Server/Zone/Game/StaticDB/ItemDB.hpp"
#include "Server/Zone/Game/StaticDB/JobDB.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Game/Units/Traits/AttributesImpl.hpp"
#include "Server/Zone/Interface/ZoneClientInterface.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Zone.hpp"

#include <boost/format.hpp>

using namespace Horizon::Zone::Units::Traits;
using namespace Horizon::Zone::Assets;

Inventory::Inventory(std::shared_ptr<Horizon::Zone::Units::Player> player, uint32_t max_storage)
: _player(player), _max_storage(max_storage)
{
	_equipments[IT_EQPI_ACC_L].first = IT_EQPM_ACC_L;
	_equipments[IT_EQPI_ACC_R].first = IT_EQPM_ACC_R;
	_equipments[IT_EQPI_SHOES].first = IT_EQPM_SHOES;
	_equipments[IT_EQPI_GARMENT].first = IT_EQPM_GARMENT;
	_equipments[IT_EQPI_HEAD_LOW].first = IT_EQPM_HEAD_LOW;
	_equipments[IT_EQPI_HEAD_MID].first = IT_EQPM_HEAD_MID;
	_equipments[IT_EQPI_HEAD_TOP].first = IT_EQPM_HEAD_TOP;
	_equipments[IT_EQPI_ARMOR].first = IT_EQPM_ARMOR;
	_equipments[IT_EQPI_HAND_L].first = IT_EQPM_HAND_L;
	_equipments[IT_EQPI_HAND_R].first = IT_EQPM_HAND_R;
	_equipments[IT_EQPI_COSTUME_TOP].first = IT_EQPM_COSTUME_HEAD_TOP;
	_equipments[IT_EQPI_COSTUME_MID].first = IT_EQPM_COSTUME_HEAD_MID;
	_equipments[IT_EQPI_COSTUME_LOW].first = IT_EQPM_COSTUME_HEAD_LOW;
	_equipments[IT_EQPI_COSTUME_GARMENT].first = IT_EQPM_COSTUME_GARMENT;
	_equipments[IT_EQPI_AMMO].first = IT_EQPM_AMMO;
	_equipments[IT_EQPI_SHADOW_ARMOR].first = IT_EQPM_SHADOW_ARMOR;
	_equipments[IT_EQPI_SHADOW_WEAPON].first = IT_EQPM_SHADOW_WEAPON;
	_equipments[IT_EQPI_SHADOW_SHIELD].first = IT_EQPM_SHADOW_SHIELD;
	_equipments[IT_EQPI_SHADOW_SHOES].first = IT_EQPM_SHADOW_SHOES;
	_equipments[IT_EQPI_SHADOW_ACC_R].first = IT_EQPM_SHADOW_ACC_R;
	_equipments[IT_EQPI_SHADOW_ACC_L].first = IT_EQPM_SHADOW_ACC_L;
}

Inventory::~Inventory()
{
	//
}

/**
 * Inventory initialization for player, called after Status is computed and notified.
 */
void Inventory::initialize()
{
	for (auto item : _inventory_items) {
		player()->status()->current_weight()->add_base(item->config->weight * item->amount);
	}
}

bool Inventory::use_item(uint32_t inventory_index, uint32_t guid)
{
	std::shared_ptr<item_entry_data> inv_item = _inventory_items.at(inventory_index - 2);

	if (inv_item == nullptr)
		return false;

	if (inv_item->config == nullptr) {
		player()->get_session()->clif()->notify_use_item(inv_item, false);
		return false;
	}

	player()->get_session()->clif()->notify_use_item(inv_item, true);
	return true;
}

item_equip_result_type Inventory::equip_item(uint32_t inventory_index, uint16_t equip_location_mask)
{
	uint32_t job_id = player()->job_id();
	std::shared_ptr<item_entry_data> inv_item = _inventory_items.at(inventory_index - 2);

	if (inv_item == nullptr) {
		HLog(debug) << "Inventory::equip_item: Could not wear item at inventory index " << inventory_index << " - Inventory data not found.";
		return IT_EQUIP_FAIL;
	}

	if (inv_item->config == nullptr) {
		player()->get_session()->clif()->notify_equip_item(inv_item, IT_EQUIP_FAIL);
		return IT_EQUIP_FAIL;
	}

	if (inv_item->current_equip_location_mask != 0 || inv_item->config->equip_location_mask != equip_location_mask) {
		player()->get_session()->clif()->notify_equip_item(inv_item, IT_EQUIP_FAIL);
		return IT_EQUIP_FAIL;
	}

	auto req_job_it = std::find_if(inv_item->config->requirements.job_ids.begin(), inv_item->config->requirements.job_ids.end(),
		[job_id](uint32_t id) {
			return job_id == id;
		});
	if (req_job_it == inv_item->config->requirements.job_ids.end()) {
		player()->get_session()->clif()->notify_equip_item(inv_item, IT_EQUIP_FAIL);
		return IT_EQUIP_FAIL;
	}

	if (inv_item->config->config.bind_on_equip != 0 && inv_item->bind_type == IT_BIND_NONE) {
		inv_item->bind_type = IT_BIND_CHARACTER;
		player()->get_session()->clif()->notify_bind_on_equip(inv_item->index.inventory);
	}

	inv_item->current_equip_location_mask = calculate_current_equip_location_mask(inv_item->config);

	add_to_equipment_list(inv_item);

	if (inv_item->type == IT_TYPE_AMMO) {
		player()->get_session()->clif()->notify_equip_arrow(inv_item);
		player()->get_session()->clif()->notify_action_failure(3);
	} else {
		HLog(debug) << "Inventory Item " << inv_item->index.inventory << " - " << inv_item->item_id << "worn.";
		
		player()->get_session()->clif()->notify_equip_item(inv_item, IT_EQUIP_SUCCESS);
		
		player()->on_item_equip(inv_item);
		
		print_inventory();
	}

	return IT_EQUIP_SUCCESS;
}

item_unequip_result_type Inventory::unequip_item(uint32_t inventory_index)
{
	std::shared_ptr<item_entry_data> inv_item = _inventory_items.at(inventory_index - 2);

	if (inv_item == nullptr) {
		player()->get_session()->clif()->notify_unequip_item(inv_item, IT_UNEQUIP_FAIL);
		return IT_UNEQUIP_FAIL;
	}

	if (inv_item->config == nullptr) {
		player()->get_session()->clif()->notify_unequip_item(inv_item, IT_UNEQUIP_FAIL);
		return IT_UNEQUIP_FAIL;
	}

	player()->get_session()->clif()->notify_unequip_item(inv_item, IT_UNEQUIP_SUCCESS);

	remove_from_equipment_list(inv_item);

	player()->on_item_unequip(inv_item);

	print_inventory();

	return IT_UNEQUIP_SUCCESS;
}

void Inventory::add_to_equipment_list(std::shared_ptr<item_entry_data> item)
{
	for (int i = 0; i < (item_equip_location_index) IT_EQPI_MAX; i++) {
		auto &equip = equipments()[i];
		std::shared_ptr<item_entry_data> id = equip.second.lock();

		// Unequip the existing item and wear the new item.
		if ((item->current_equip_location_mask & equip.first)) {
			if (!equip.second.expired() && unequip_item(id->index.inventory) == IT_UNEQUIP_FAIL)
				return;
			equip.second = item;
		}
	}
}

void Inventory::remove_from_equipment_list(std::shared_ptr<item_entry_data> item)
{
	for (int i = 0; i < (item_equip_location_index)  IT_EQPI_MAX; i++) {
		auto &equip = equipments()[i];
		std::shared_ptr<const item_entry_data> id = equip.second.lock();

		if ((item->current_equip_location_mask & equip.first)) {
			item->current_equip_location_mask &= ~equip.first;
			equip.second.reset();
		}
	}
}

uint32_t Inventory::calculate_current_equip_location_mask(std::shared_ptr<const item_config_data> item)
{
	job_class_mask job_mask = JobDB->job_id_to_mask((job_class_type) player()->job_id());
	uint32_t current_equip_location_mask = item->equip_location_mask;

	if (item->type == IT_TYPE_WEAPON) {
		if (item->sub_type.weapon_t == IT_WT_DAGGER ||
			item->sub_type.weapon_t == IT_WT_1HSWORD ||
			item->sub_type.weapon_t == IT_WT_1HAXE) {
			if ((job_mask & JMASK_ASSASSIN) == JMASK_ASSASSIN
				 || (job_mask & JMASK_KAGEROUOBORO) == JMASK_KAGEROUOBORO) {
				if (item->equip_location_mask == IT_EQPM_HAND_R)
					current_equip_location_mask = IT_EQPM_ARMS;
				if (item->equip_location_mask == IT_EQPM_SHADOW_WEAPON)
					current_equip_location_mask = IT_EQPM_SHADOW_ARMS;
			}
		}
	}

	if (current_equip_location_mask == IT_EQPM_ACC)
		current_equip_location_mask = equipments()[IT_EQPI_ACC_L].second.expired() ? IT_EQPM_ACC_L : IT_EQPM_ACC_R;
	else if (current_equip_location_mask == IT_EQPM_ARMS && item->equip_location_mask == IT_EQPM_WEAPON)
		current_equip_location_mask = equipments()[IT_EQPI_HAND_L].second.expired() ? IT_EQPM_HAND_L : IT_EQPM_HAND_R;
	else if (current_equip_location_mask == IT_EQPM_SHADOW_ACC)
		current_equip_location_mask = equipments()[IT_EQPI_SHADOW_ACC_L].second.expired() ? IT_EQPM_SHADOW_ACC_L : IT_EQPM_SHADOW_ACC_R;
	else if (current_equip_location_mask == IT_EQPM_SHADOW_ARMS && item->equip_location_mask == IT_EQPM_SHADOW_WEAPON)
		current_equip_location_mask = equipments()[IT_EQPI_SHADOW_WEAPON].second.expired() ? IT_EQPM_SHADOW_WEAPON : IT_EQPM_SHADOW_SHIELD;

	return current_equip_location_mask;
}

void Inventory::print_inventory()
{
	HLog(debug) << " -- Inventory List --";
	for (auto i : _inventory_items)
		HLog(debug) << "Idx: " << i->index.inventory << " ItemID: " << i->item_id << " Amount: " << i->amount;


	HLog(debug) << " -- Equipments List --";
	for (int i = 0; i < IT_EQPI_MAX; i++) {
		auto &equip = equipments()[i];
		std::shared_ptr<const item_entry_data> id = equip.second.lock();
		if (id != nullptr)
			HLog(debug) << "Loc:" << std::hex << id->current_equip_location_mask << " Loc2: " << id->actual_equip_location_mask << " Idx: " << std::dec << id->index.inventory << " ItemID: " << id->item_id << " Amount: " << id->amount;
	}
}


void Inventory::notify_all()
{
	player()->get_session()->clif()->notify_inventory_start(INVTYPE_INVENTORY, "Inventory");
	notify_only_equipments();
	notify_without_equipments();
	player()->get_session()->clif()->notify_inventory_end(INVTYPE_INVENTORY);
}

void Inventory::notify_without_equipments()
{
	std::vector<std::shared_ptr<const item_entry_data>> normal_items;

	for (auto nit = _inventory_items.begin(); nit != _inventory_items.end(); nit++) {
		if ((*nit)->is_equipment() == false)
			normal_items.push_back(*nit);
	}

	player()->get_session()->clif()->notify_normal_item_list(normal_items);

}

void Inventory::notify_only_equipments()
{
	std::vector<std::shared_ptr<const item_entry_data>> equipments;

	for (auto eit = _inventory_items.begin(); eit != _inventory_items.end(); eit++) {
		if ((*eit)->is_equipment())
			equipments.push_back(*eit);
	}

	player()->get_session()->clif()->notify_equipment_item_list(equipments);
}

int16_t Inventory::get_free_index()
{
	for (auto i = _inventory_items.begin(); i != _inventory_items.end(); i++) {
		if (*i == nullptr)
			return std::distance(_inventory_items.begin(), i);
	}

	_inventory_items.resize(_inventory_items.size() + 1);
	return _inventory_items.size() - 1;
}

inventory_addition_result_type Inventory::add_item(std::shared_ptr<Horizon::Zone::Units::Item> floor_item)
{
	if (floor_item->entry() == nullptr)
		return add_item(floor_item->config()->item_id, floor_item->amount(), floor_item->is_identified());

	return add_item(floor_item->entry(), floor_item->amount());
}

inventory_addition_result_type Inventory::add_item(std::shared_ptr<item_entry_data> item, int amount)
{
	if (item == nullptr)
		return INVENTORY_ADD_INVALID;

	if (item->config == nullptr)
		return INVENTORY_ADD_INVALID;

	std::shared_ptr<Horizon::Zone::Traits::CurrentWeight> current_weight = player()->status()->current_weight();
	std::shared_ptr<Horizon::Zone::Traits::MaxWeight> max_weight = player()->status()->max_weight();

	if (amount <= 0)
		return INVENTORY_ADD_INVALID;

	if (*current_weight + (amount * item->config->weight) > max_weight->total()) {
		notify_add(item, item->amount, INVENTORY_ADD_OVER_WEIGHT);
		return INVENTORY_ADD_OVER_WEIGHT;
	}

	if (item->config->stack.inventory > 0 && (amount > item->config->stack.inventory)) {
		notify_add(item, amount, INVENTORY_ADD_OVER_STACK_LIMIT);
		return INVENTORY_ADD_OVER_STACK_LIMIT;
	}

	if (amount > MAX_INVENTORY_STACK_LIMIT) {
		notify_add(item, amount, INVENTORY_ADD_OVER_QUANTITY);
		return INVENTORY_ADD_OVER_QUANTITY;
	}

	if (_inventory_items.size() >= max_storage()) {
		notify_add(item, amount, INVENTORY_ADD_NO_INV_SPACE);
		return INVENTORY_ADD_NO_INV_SPACE;
	}

	// Check if item is stackable
	if (item->is_stackable()) {
		// Check if item exists in inventory.
		auto invitem = std::find_if(_inventory_items.begin(), _inventory_items.end(), 
			[&item] (std::shared_ptr<item_entry_data> invit) { 
				if (invit == nullptr)
					return false;
				return (invit->amount < MAX_INVENTORY_STACK_LIMIT && *invit == *item); 
			}
		);
		// If item was found in inventory...
		if (invitem != _inventory_items.end()) {
			std::shared_ptr<item_entry_data> it_entry = *invitem;
			// Check if amount exeeds stack size.
			if (it_entry->amount + amount > MAX_INVENTORY_STACK_LIMIT) {
				// Add appropriately
				int left_amt = it_entry->amount - MAX_INVENTORY_STACK_LIMIT;
				it_entry->amount += left_amt;
				item->amount -= left_amt;
				add_item(item->item_id, amount, item->info.is_identified);
			} else {
				it_entry->amount += amount;
				notify_add(it_entry, amount, INVENTORY_ADD_SUCCESS);

				current_weight->add_base(item->config->weight * amount);
			}
		} else {
			int index = get_free_index();
			std::shared_ptr<item_entry_data> item_new_stack = std::make_shared<item_entry_data>(*item);
			item_new_stack->amount = amount;
			item_new_stack->storage_type = ITEM_STORE_INVENTORY;
			item_new_stack->index.inventory = index + 2;
			_inventory_items[index] = item_new_stack;
			notify_add(item_new_stack, amount, INVENTORY_ADD_SUCCESS);

			current_weight->add_base(item_new_stack->config->weight * amount);
		}
	} else {
		int index = get_free_index();
		std::shared_ptr<item_entry_data> it_entry = std::make_shared<item_entry_data>(*item);
		it_entry->unique_id = player()->new_unique_id();
		it_entry->amount = 1;
		it_entry->storage_type = ITEM_STORE_INVENTORY;
		it_entry->index.inventory = index + 2;
		_inventory_items[index] = it_entry;
		notify_add(it_entry, it_entry->amount, INVENTORY_ADD_SUCCESS);
		current_weight->add_base(item->config->weight);
		if (--amount)
			add_item(item, amount);
	}

	return INVENTORY_ADD_SUCCESS;
}

inventory_addition_result_type Inventory::add_item(uint32_t item_id, uint16_t amount, bool is_identified)
{
	item_entry_data data;

	std::shared_ptr<const item_config_data> item_d = ItemDB->get_item_by_id(item_id);
	std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(player()->job_id());

	if (item_d == nullptr)
		return INVENTORY_ADD_INVALID;

	// Copy item properties from static db entry.
	data.item_id = item_id;
	data.type = item_d->type;
	data.actual_equip_location_mask = item_d->equip_location_mask;
	data.bind_type = IT_BIND_NONE;
	data.info.is_identified = is_identified;
	data.info.is_favorite = 0;
	data.config = item_d;

	std::shared_ptr<item_entry_data> inv_item = std::make_shared<item_entry_data>(data);

	return add_item(inv_item, amount);
}

inventory_removal_result_type Inventory::remove_item(int16_t inventory_index, int amount, item_deletion_reason_type reason)
{
	std::shared_ptr<item_entry_data> inv_item = get_item(inventory_index);
	
	std::shared_ptr<Horizon::Zone::Traits::CurrentWeight> current_weight = player()->status()->current_weight();

	if (inv_item->amount - amount < 0)
		return INVENTORY_REMOVE_INVALID;

	if (amount > inv_item->amount || amount > MAX_INVENTORY_STACK_LIMIT)
		return INVENTORY_REMOVE_INVALID;
	
	if (inv_item->amount - amount == 0)
		_inventory_items[inventory_index - 2] = nullptr;
	else
		inv_item->amount -= amount;
	
	player()->get_session()->clif()->notify_delete_item(inventory_index, amount, reason);
	current_weight->sub_base(inv_item->config->weight * amount);

	return INVENTORY_REMOVE_SUCCESS;
}
std::shared_ptr<item_entry_data> Inventory::get_item(uint32_t inventory_index)
{
	return _inventory_items.at(inventory_index - 2);
}

void Inventory::drop_item(uint32_t inventory_index, uint16_t amount)
{
	std::shared_ptr<item_entry_data> inv_item = get_item(inventory_index);

	if (inv_item == nullptr)
		return;

	if (inv_item->config == nullptr)
		return;

	if (inv_item->amount < amount)
		return;

	player()->status()->current_weight()->sub_base(inv_item->config->weight * amount);

	if (inv_item->amount == amount) {
		inv_item = nullptr;
		notify_drop(inventory_index, amount);
	} else {
		inv_item->amount -= amount;
		notify_drop(inventory_index, amount);
	}
}

void Inventory::notify_add(std::shared_ptr<item_entry_data> const &item, uint16_t amount, inventory_addition_result_type result)
{
	item_inventory_addition_notif_type notif_type;

	switch (result)
	{
	case INVENTORY_ADD_SUCCESS: notif_type = ITEM_INV_ADD_SUCCESS; break;
	case INVENTORY_ADD_INVALID: notif_type = ITEM_INV_ADD_INVALID; break;
	case INVENTORY_ADD_OVER_WEIGHT: notif_type = ITEM_INV_ADD_OVER_WEIGHT; break;
	case INVENTORY_ADD_NO_INV_SPACE: notif_type = ITEM_INV_ADD_NO_INV_SPACE; break;
	case INVENTORY_ADD_OVER_QUANTITY: notif_type = ITEM_INV_ADD_OVER_QUANTITY; break;
	case INVENTORY_ADD_OVER_STACK_LIMIT: notif_type = ITEM_INV_ADD_OVER_STACK_LIMIT; break;
	}

	player()->get_session()->clif()->notify_pickup_item(item, amount, notif_type);
}

void Inventory::notify_deletion(uint16_t idx, uint16_t amount, inventory_deletion_reason_type reason)
{

}

void Inventory::notify_drop(uint16_t idx, uint16_t amount)
{
	player()->get_session()->clif()->notify_throw_item(idx, amount);
}

void Inventory::notify_move_fail(uint16_t idx, bool silent)
{

}

int32_t Inventory::save()
{
	int32_t changes = 0;

	// Erase saved vector and copy the current inventory.
	_saved_inventory_items.clear();
	std::copy(_inventory_items.begin(), _inventory_items.end(), std::inserter(_saved_inventory_items, _saved_inventory_items.end()));
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();

	try {
		boost::mysql::statement stmt = conn->prepare_statement("DELETE FROM `character_inventory` WHERE `char_id` = ?");
		auto b1 = stmt.bind(player()->character()._character_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		boost::format fmt_ctx = boost::format("INSERT INTO `character_inventory` (`char_id`, `item_id`, `amount`, `equip_location_mask`,"
			"`is_identified`, `refine_level`, `element_type`, `slot_item_id_0`, `slot_item_id_1`, `slot_item_id_2`, `slot_item_id_3`, `opt_idx0`, `opt_val0`,"
			"`opt_idx1`, `opt_val1`, `opt_idx2`, `opt_val2`, `opt_idx3`, `opt_val3`, `opt_idx4`, `opt_val4`, `hire_expire_date`, `is_favorite`, `is_broken`, `bind_type`, `unique_id`"
			"VALUES ");
			
		int count = 0;
		for (auto mit_i = _saved_inventory_items.begin(); mit_i != _saved_inventory_items.end(); mit_i++) {
			std::shared_ptr<const item_entry_data> mit = *mit_i;

			if (mit == nullptr)
				continue;

			if (count != 0) {
				std::string str = fmt_ctx.str();
				fmt_ctx = boost::format(str.append(", "));
			}

			boost::format new_fmt = boost::format("(%1%, %2%, %3%, %4%, %5%, %6%, %7%, %8%, %9%, %10%, %11%, %12%, %13%, %14%, %15%, %16%, %17%, %18%, %19%, %20%, %21%, %22%, %23%, %24%, %25%, %26%)")
				% player()->character()._character_id
				% (int)mit->item_id
				% (int)mit->amount
				% (int)mit->current_equip_location_mask
				% (int)mit->info.is_identified
				% (int)mit->refine_level
				% (int)mit->ele_type
				% (int)mit->slot_item_id[0]
				% (int)mit->slot_item_id[1]
				% (int)mit->slot_item_id[2]
				% (int)mit->slot_item_id[3]
				% (int)mit->option_data[0].get_index()
				% (int)mit->option_data[0].get_value()
				% (int)mit->option_data[1].get_index()
				% (int)mit->option_data[1].get_value()
				% (int)mit->option_data[2].get_index()
				% (int)mit->option_data[2].get_value()
				% (int)mit->option_data[3].get_index()
				% (int)mit->option_data[3].get_value()
				% (int)mit->option_data[4].get_index()
				% (int)mit->option_data[4].get_value()
				% (int)mit->hire_expire_date
				% (int)mit->info.is_favorite
				% (int)mit->info.is_broken
				% (int)mit->bind_type
				% (int64_t)mit->unique_id;

			std::string str_str = fmt_ctx.str() + new_fmt.str();
			fmt_ctx = boost::format(str_str);
			count++;
		}

		if (count) {
			boost::mysql::results results;
			conn->execute(fmt_ctx.str(), results);
		}

		changes = count;
	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << "Inventory::save:" << error.what();
		return false;
	}
	catch (std::exception& error) {
		HLog(error) << "Inventory::save:" << error.what();
		return false;
	}

	HLog(info) << "Saved inventory for (Character ID: " << player()->account()._account_id << ") with " << changes << " changes.";
	return changes;
}

int32_t Inventory::load()
{	

	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `item_id`, `amount`, `equip_location_mask`, `refine_level`, `slot_item_id_0`,"
			"`slot_item_id_1`, `slot_item_id_2`, `slot_item_id_3`, `hire_expire_date`, `element_type`, `opt_idx0`, `opt_val0`, `opt_idx1`, `opt_val1`,"
			"`opt_idx2`, `opt_val2`, `opt_idx3`, `opt_val3`, `opt_idx4`, `opt_val4`, `is_identified`, `is_broken`, `is_favorite`, `bind_type`, `unique_id`"
			" FROM `character_inventory` WHERE `char_id` = ?");
		auto b1 = stmt.bind(player()->character()._character_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (_inventory_items.size() != 0) {
			HLog(warning) << "Attempt to synchronize the saved inventory, which should be empty at the time of load or re-load, size: " << _inventory_items.size();
			return 0;
		}

		int inventory_index = 2;
		for (auto r : results.rows()) {
			item_entry_data i;

			std::shared_ptr<const item_config_data> d = ItemDB->get_item_by_id(r[0].as_uint64());

			i.storage_type = ITEM_STORE_INVENTORY;
			i.index.inventory = inventory_index;
			i.item_id = r[0].as_uint64();
			i.type = d->type;
			i.amount = r[1].as_uint64();
			i.current_equip_location_mask = r[2].as_uint64();
			i.actual_equip_location_mask = d->equip_location_mask;
			i.refine_level = r[3].as_uint64();
			i.config = d;

			i.slot_item_id[0] = r[4].as_int64();
			i.slot_item_id[1] = r[5].as_int64();
			i.slot_item_id[2] = r[6].as_int64();
			i.slot_item_id[3] = r[7].as_int64();

			i.hire_expire_date = r[8].as_uint64();
			i.sprite_id = d->sprite_id;

			i.ele_type = (element_type)(int)r[9].as_uint64();

			if (r[10].as_uint64()) {
				i.option_data[0].set_index(r[10].as_uint64());
				i.option_data[0].set_value(r[11].as_int64());
				i.option_count = 1;
			}

			if (r[12].as_uint64()) {
				i.option_data[1].set_index(r[12].as_uint64());
				i.option_data[1].set_value(r[13].as_int64());
				i.option_count = 2;
			}

			if (r[14].as_uint64()) {
				i.option_data[2].set_index(r[14].as_uint64());
				i.option_data[2].set_value(r[15].as_int64());
				i.option_count = 3;
			}

			if (r[16].as_uint64()) {
				i.option_data[3].set_index(r[16].as_uint64());
				i.option_data[3].set_value(r[17].as_int64());
				i.option_count = 4;
			}

			if (r[18].as_uint64()) {
				i.option_data[4].set_index(r[18].as_uint64());
				i.option_data[4].set_value(r[19].as_int64());
				i.option_count = 5;
			}

			i.info.is_identified = r[20].as_int64();
			i.info.is_broken = r[21].as_uint64();
			i.info.is_favorite = r[22].as_uint64();

			i.bind_type = (item_bind_type)(int)r[23].as_uint64(); // int16_t
			i.unique_id = r[24].as_uint64();

			std::shared_ptr<item_entry_data> item = std::make_shared<item_entry_data>(i);

			_saved_inventory_items.resize(inventory_index - 2 + 1);
			_inventory_items.resize(inventory_index - 2 + 1);
			_saved_inventory_items[inventory_index - 2] = item;
			_inventory_items[inventory_index - 2] = item;
			inventory_index++;
		}
	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << "Inventory::load:" << error.what();
		return false;
	}
	catch (std::exception& error) {
		HLog(error) << "Inventory::load:" << error.what();
		return false;
	}
	
	HLog(info) << "Loaded inventory for (Character ID: " << player()->account()._account_id << ") with " << _inventory_items.size() << " items.";

	return _inventory_items.size();
}
