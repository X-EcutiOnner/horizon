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

#include "Player.hpp"

#include "Server/Zone/Definitions/UnitDefinitions.hpp"

#include "Server/Zone/Game/Units/Player/Assets/Inventory.hpp"
#include "Server/Zone/Game/Units/Player/Assets/Storage.hpp"
#include "Server/Zone/Game/Map/Grid/Notifiers/GridNotifiers.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainer.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainerVisitor.hpp"
#include "Server/Zone/Game/StaticDB/SkillDB.hpp"
#include "Server/Zone/Game/StaticDB/StorageDB.hpp"
#include "Server/Zone/Game/Units/Traits/AttributesImpl.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Game/Units/Item/Item.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Socket/ZoneSocket.hpp"

#include "Server/Zone/Zone.hpp"


#include "version.hpp"

using namespace Horizon::Zone::Units;

Player::Player(std::shared_ptr<ZoneSession> session, uint64_t uuid)
: Unit(uuid, UNIT_PLAYER, UNIT_MASK_PLAYER), _session(session), _lua_state(std::make_shared<sol::state>())
{
}

Player::~Player()
{
	if (has_valid_grid_reference())
		remove_grid_reference();
}

uint64_t Player::new_unique_id()
{
	int64_t char_id = (int64_t) character()._character_id;
	
	if (character()._last_unique_id > 0)
		return ++character()._last_unique_id;

	return (character()._last_unique_id = (char_id << 32));
}

void Player::create(int char_id, std::string account_gender, int group_id)
{
	character()._character_id = char_id;
	account()._account_gender = account_gender;
	account()._group_id = group_id;

	load();
}
bool Player::initialize()
{
	if (Unit::initialize() == false)
		return false;

	// Inventory.
	_inventory = std::make_shared<Assets::Inventory>(downcast<Player>(), get_max_inventory_size());
	_inventory->load();

	for (auto s : StorageDB->get_storage_db()) {
		std::shared_ptr<const storage_config_data> storage_config = s.second;
		std::shared_ptr<Assets::Storage> storage = std::make_shared<Assets::Storage>(downcast<Player>(), storage_config->storage_id, storage_config->name, storage_config->capacity);
		storage->load();
		_storages.push_back(storage);
	}

	// Initialize Status, after inventory is loaded to compute EquipAtk etc.
	status()->initialize(shared_from_this()->downcast<Player>());
	status()->size()->set_base(ESZ_MEDIUM);

	// Inventory is initialized for the player, notifying weight etc.
	_inventory->initialize();
	
	// Populate skill tree.
	std::vector<std::shared_ptr<const skill_tree_config>> sktree = SkillDB->get_skill_tree_by_job_id((job_class_type) job_id());

	for (auto s : sktree) {
		skill_learnt_info info;
		info.skill_id = s->skill_id;
		info.level = 0;
		info.learn_type = SKILL_LEARN_PERMANENT;
		add_learnt_skill(std::make_shared<skill_learnt_info>(info));
	}

	// On map entry processing.
	on_map_enter();

	// @TODO (multithreaded support, multi-segment support) Initialize Player, npc and monster components for player's lua state.
	sZone->get_component_of_type<Horizon::Zone::ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM)->initialize_player_state(lua_state());

	// required for npc component definitions upon triggering
	sZone->get_component_of_type<Horizon::Zone::ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM)->initialize_npc_state(lua_state());

 	// required for triggering monster components upon monster kill events etc.
	sZone->get_component_of_type<Horizon::Zone::ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM)->initialize_monster_state(lua_state());

	try {
		std::string script_root_path = sZone->config().get_script_root_path().string();
		sol::load_result fx = lua_state()->load_file(script_root_path + "internal/on_login_event.lua");
		sol::protected_function_result result = fx(shared_from_this()->downcast<Player>(), VER_PRODUCTVERSION_STR);
		if (!result.valid()) {
			sol::error err = result;
			HLog(error) << "Player::initialize: " << err.what();
			return false;
		}
	} catch (sol::error &e) {
		HLog(error) << "Player::initialize: " << e.what();
		return false;
	}


	// Ensure grid for unit.
	map()->ensure_grid_for_unit(this, map_coords());
	map()->add_user_count();
	
	set_initialized(true);
	
	return true;
}

void Player::stop_movement()
{
	MapCoords const &coords = map_coords();
	get_session()->clif()->notify_movement_stop(guid(), coords.x(), coords.y());
}

bool Player::save()
{
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("UPDATE `characters` SET `account_id` = ?, `slot` = ?, `name` = ?, `online` = ?, `gender` = ?, `unban_time` = ?, `rename_count` = ?,"
			"`last_unique_id` = ?, `hotkey_row_index` = ?, `change_slot_count` = ?, `font` = ?, `show_equip` = ?, `allow_party` = ?, `partner_aid` = ?, `father_aid` = ?, `mother_aid` = ?,"
			"`child_aid` = ?, `party_id` = ?, `guild_id` = ?, `pet_id` = ?, `homun_id` = ?, `elemental_id` = ?, `current_map` = ?, `current_x` = ?, `current_y` = ?,"
			"`saved_map` = ?, `saved_x` = ?, `saved_y` = ? "
			"WHERE `id` = ?");
		auto b1 = stmt.bind(account()._account_id, character()._slot, name(), character()._online, character()._gender == UNIT_GENDER_MALE ? "M" : "F", character()._unban_time, character()._rename_count,
				character()._last_unique_id, character()._hotkey_row_index, character()._change_slot_count, character()._font, character()._show_equip, character()._allow_party,
				character()._partner_aid, character()._father_aid, character()._mother_aid, character()._child_aid, character()._party_id, character()._guild_id, character()._pet_id,
				character()._homun_id, character()._elemental_id, map()->get_name(), map_coords().x(), map_coords().y(), character()._saved_map, character()._saved_x, character()._saved_y,
				character()._character_id
			);
		boost::mysql::results results;
		conn->execute(b1, results);

		// Status
		if (status() != nullptr)
			status()->save(shared_from_this()->downcast<Player>());

		// Inventory
		if (inventory() != nullptr)
			inventory()->save();

		// Storages
		if (_storages.size() > 0) {
			for (auto s : _storages)
				s->save();
		}
	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << "Player::save:" << error.what();
		return false;
	}
	catch (std::exception& error) {
		HLog(error) << "Player::save:" << error.what();
		return false;
	}
	
	return true;
}

bool Player::load()
{
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `id`, `account_id`, `slot`, `name`, `font`, `gender`, `last_unique_id`, `saved_map`, `saved_x`, `saved_y`, "
			"`current_map`, `current_x`, `current_y` FROM `characters` WHERE id = ?");
		auto b1 = stmt.bind(character()._character_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (results.rows().empty()) {
			HLog(error) << "Error loading player, character with ID " << character()._character_id << " does not exist.";
			return false;
		}

		auto r = results.rows()[0];
		/* Initialize Player Model */
		character()._character_id = r[0].as_uint64();
		account()._account_id = r[1].as_uint64();
		character()._slot = r[2].as_int64();
		set_name(r[3].as_string());
		set_posture(POSTURE_STANDING);
		character()._font = r[4].as_uint64();

		std::string char_gender = r[5].as_string();

		character()._gender = strcmp(char_gender.c_str(), "U") == 0
			? (strcmp(account()._account_gender.c_str(), "M") == 0 ? UNIT_GENDER_MALE : UNIT_GENDER_FEMALE)
			: strcmp(char_gender.c_str(), "M") == 0 ? UNIT_GENDER_MALE : UNIT_GENDER_FEMALE;

		character()._online = 1;
		set_last_unique_id((uint64_t)r[6].as_uint64());

		character()._saved_map = r[7].as_string();
		character()._saved_x = r[8].as_uint64();
		character()._saved_y = r[9].as_uint64();

		/**
		 * Set map and coordinates for unit.
		 */
		// @TODO
		MapCoords mcoords(r[11].as_uint64(), r[12].as_uint64());
		int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, r[10].as_string(), nullptr);

		if (segment_number == 0)
			return false;
		
		auto container = sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number);
		
		std::shared_ptr<Map> map = container->get_resource_manager().template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(r[10].as_string(), nullptr);

		if (map == nullptr) { 
			HLog(warning) << "Player::load: Map " << r[10].as_string() << " does not exist, setting to default map.";
			segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, "prontera", nullptr);

			if (segment_number == 0)
				return false;

			map = container->get_resource_manager().template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>("prontera", nullptr);
			
			if (map == nullptr) {
				HLog(error) << "Player::load: Default map prontera does not exist.";
				return false;
			}
		}
		
		get_session()->set_map_name(map->get_name());
		
		set_map(map);
		set_map_coords(mcoords);
	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << "Player::load:" << error.what();
		return false;
	}
	catch (std::exception& error) {
		HLog(error) << "Player::load:" << error.what();
		return false;
	}

	return true;
}

std::shared_ptr<Horizon::Zone::Assets::Storage> Player::get_storage(int32_t storage_id) 
{ 
	for (auto s : _storages) {
		if (s->get_storage_id() == storage_id)
			return s;
	}

	return nullptr;
}

void Player::on_pathfinding_failure()
{
	//HLog(debug) << "Player " << name() << " has failed to find path from (" << map_coords().x() << "," << map_coords().y() << ") to (" << dest_coords().x() << ", " << dest_coords().y() << ").";
}

void Player::on_movement_begin()
{
	get_session()->clif()->notify_player_movement(map_coords(), dest_coords());
}

void Player::on_movement_end()
{
}

void Player::on_movement_step()
{
	GridNPCTrigger npc_trigger(shared_from_this());
	GridReferenceContainerVisitor<GridNPCTrigger, GridReferenceContainer<AllUnitTypes>> npc_trigger_performer(npc_trigger);

	map()->ensure_grid_for_unit(this, map_coords());

	update_viewport();

	map()->visit_in_range(map_coords(), npc_trigger_performer, MAX_NPC_TRIGGER_RANGE);
}

bool Player::is_overweight_50() { return status()->current_weight()->total() * 100 >= status()->max_weight()->total() * 50; }
bool Player::is_overweight_90() { return status()->current_weight()->total() * 10 >= status()->max_weight()->total() * 9; }

void Player::update_viewport()
{
	GridViewPortUpdater updater(shared_from_this());
	GridReferenceContainerVisitor<GridViewPortUpdater, GridReferenceContainer<AllUnitTypes>> update_caller(updater);

	map()->visit_in_range(map_coords(), update_caller);
}

void Player::add_unit_to_viewport(std::shared_ptr<Unit> unit)
{
	if (unit == nullptr)
		return;

	if (unit_is_in_viewport(unit))
		return;

	if (unit->type() == UNIT_ITEM) {
		item_viewport_entry entry = get_session()->clif()->create_viewport_item_entry(unit->downcast<Item>());
		get_session()->clif()->notify_viewport_item_entry(entry);
	} else {
		unit_viewport_entry entry = get_session()->clif()->create_viewport_entry(unit);
		get_session()->clif()->notify_viewport_add_unit(entry);
	}
	
	_viewport_entities.push_back(unit);

	HLog(debug) << "------- VIEWPORT ENTITIES ----------";
	for (auto it = _viewport_entities.begin(); it != _viewport_entities.end(); it++) {
		if ((*it).expired())
			continue;
		HLog(debug) << "Unit:" << it->lock()->name() << " " << it->lock()->guid();
	}
	HLog(debug) << "--------------------";
}

void Player::remove_unit_from_viewport(std::shared_ptr<Unit> unit, unit_viewport_notification_type type)
{
	if (!unit_is_in_viewport(unit))
		return;

	_viewport_entities.erase(std::remove_if(_viewport_entities.begin(), _viewport_entities.end(),
		[unit] (std::weak_ptr<Unit> wp_e) {
			if (wp_e.expired())
				return false;
			return wp_e.lock()->guid() == unit->guid();
		}
	), _viewport_entities.end());

	if (unit->type() == UNIT_ITEM)
		get_session()->clif()->notify_item_removal_from_floor(unit->guid());
	else
		get_session()->clif()->notify_viewport_remove_unit(unit, type);

	HLog(debug) << "------- VIEWPORT ENTITIES ----------";
	for (auto it = _viewport_entities.begin(); it != _viewport_entities.end(); it++) {
		if ((*it).expired())
			continue;
		HLog(debug) << "Unit:" << it->lock()->name() << " " << it->lock()->guid();
	}
	HLog(debug) << "--------------------";
}

bool Player::unit_is_in_viewport(std::shared_ptr<Unit> unit)
{
	if (unit == nullptr)
		return false;

	std::vector<std::weak_ptr<Unit>>::iterator it = std::find_if (_viewport_entities.begin(), _viewport_entities.end(), 
		[unit] (const std::weak_ptr<Unit> &vp_ew) { 
			std::shared_ptr<Unit> vp_es = vp_ew.lock();

			if (vp_es == nullptr || unit == nullptr)
				return false;

			return (vp_es->guid() == unit->guid());
		}
	);
	
	return (it != _viewport_entities.end());
}

void Player::realize_unit_movement(std::shared_ptr<Unit> unit)
{
	if (unit == nullptr)
		return;

	get_session()->clif()->notify_unit_move(unit->guid(), unit->map_coords(), unit->dest_coords());
}

void Player::realize_unit_movement_entry(std::shared_ptr<Unit> unit)
{
	if (unit == nullptr)
		return;

	unit_viewport_entry entry = get_session()->clif()->create_viewport_entry(unit);
	get_session()->clif()->notify_viewport_moving_unit(entry);
}

void Player::spawn_unit_in_viewport(std::shared_ptr<Unit> unit)
{
	if (unit == nullptr)
		return;

	unit_viewport_entry entry = get_session()->clif()->create_viewport_entry(unit);
	get_session()->clif()->notify_viewport_spawn_unit(entry);
}

bool Player::move_to_map(std::shared_ptr<Map> dest_map, MapCoords coords)
{
	if (dest_map == nullptr)
		return false;

	force_movement_stop_internal(true);

	std::shared_ptr<Player> myself = downcast<Player>();

	notify_nearby_players_of_existence(EVP_NOTIFY_TELEPORT);

	{
		// If the map is not managed by the destination container, 
		// remove the session from the current container and add it to the destination container
		if (dest_map->container()->get_resource_manager().template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(map()->get_name(), nullptr) == nullptr) {
			map()->container()->get_resource_manager().remove<RESOURCE_PRIORITY_TERTIARY>(myself->uuid());
			dest_map->container()->get_resource_manager().add<RESOURCE_PRIORITY_TERTIARY>(myself->uuid(), myself);

			map()->container()->get_resource_manager().remove<RESOURCE_PRIORITY_SECONDARY>(myself->uuid());
			dest_map->container()->get_resource_manager().add<RESOURCE_PRIORITY_SECONDARY>(myself->uuid(), myself);
		}

		map()->sub_user_count();
		dest_map->add_user_count();
		
		get_session()->set_map_name(dest_map->get_name());

		if (coords == MapCoords(0, 0))
			coords = dest_map->get_random_accessible_coordinates();

		dest_map->ensure_grid_for_unit(this, coords);

		set_map(dest_map);
		set_map_coords(coords);
	}

	get_session()->clif()->notify_move_to_map(dest_map->get_name(), coords.x(), coords.y());

	on_map_enter();

	return true;
}

void Player::send_npc_dialog(uint32_t npc_guid, std::string dialog)
{
	get_session()->clif()->notify_npc_dialog(npc_guid, dialog);
}

void Player::send_npc_next_dialog(uint32_t npc_guid)
{
	get_session()->clif()->notify_npc_next_dialog(npc_guid);
}

void Player::send_npc_close_dialog(uint32_t npc_guid)
{
	get_session()->clif()->notify_npc_close_dialog(npc_guid);
}

void Player::send_npc_menu_list(uint32_t npc_guid, std::string const &menu)
{
	get_session()->clif()->notify_npc_menu_list(npc_guid, menu);
}

void Player::on_item_equip(std::shared_ptr<const item_entry_data> item)
{
	status()->on_equipment_changed(true, item);
}

void Player::on_item_unequip(std::shared_ptr<const item_entry_data> item)
{
	status()->on_equipment_changed(false, item);
}

void Player::pickup_item(int32_t guid)
{
	std::shared_ptr<Horizon::Zone::Unit> unit = get_nearby_unit(guid);

	if (unit == nullptr)
		return;

	if (unit->type() != UNIT_ITEM)
		return;

	if (unit->is_in_range_of(shared_from_this(), 1) == false)
		return;

	std::shared_ptr<Item> item = unit->downcast<Item>();

	if (inventory()->add_item(item) == Horizon::Zone::Assets::inventory_addition_result_type::INVENTORY_ADD_SUCCESS) {	
		item->finalize();
		std::shared_ptr<GameLogicProcess> container = item->map()->container();
		container->get_resource_manager().remove<RESOURCE_PRIORITY_TERTIARY>(item->uuid());
		get_session()->clif()->notify_item_removal_from_floor(item->guid());
		get_session()->clif()->notify_action(item->guid(), PLAYER_ACT_ITEM_PICKUP);
	}
}
void Player::throw_item(std::shared_ptr<item_entry_data> item, int32_t amount)
{	
	inventory()->drop_item(item->index.inventory, amount);
	map()->add_item_drop(item, amount, map_coords());
}
void Player::notify_map_properties()
{
	zc_map_properties mp;

	// TODO: Should not be responsibility of player to do that. Working on it.
	mp.pvp = 0;
	mp.gvg = 0;
	mp.siege = 0;
	mp.no_effects = 0;
	mp.party_pvp = 0;
	mp.pvp_kill_counter = 0;
	mp.disallow_party = 0;
	mp.battleground = 0;
	mp.no_costume = 0;
	mp.allow_carts = 1;
	mp.stargladiator_miracles = 0;
	mp.spare_bits = 0;

	get_session()->clif()->notify_map_properties(mp);
}

void Player::on_map_enter()
{
    //get_packet_handler()->Send_ZC_MAPPROPERTY_R2(get_map());

	if (!is_initialized()) {
		return;
	}
		
	inventory()->notify_all();

	// Notify Weight.
	status()->current_weight()->notify();
	status()->max_weight()->notify();

	// clear viewport
	get_viewport_entities().clear();

	update_viewport();
	notify_map_properties();

	notify_nearby_players_of_spawn();

	// Notify learnt skill list.
	get_session()->clif()->notify_learnt_skill_list();
}

void Player::on_status_effect_start(std::shared_ptr<status_change_entry> sce)
{
	get_session()->clif()->notify_status_change(sce->type, guid(), 1, sce->current_time, sce->val1, sce->val2, sce->val3);
}

void Player::on_status_effect_end(std::shared_ptr<status_change_entry> sce)
{
	get_session()->clif()->notify_status_change_end(sce->type, guid(), 0);
}

void Player::on_status_effect_change(std::shared_ptr<status_change_entry> sce)
{

}

void Player::notify_in_area(ByteBuffer &buf, grid_notifier_type type, uint16_t range)
{
	GridPlayerNotifier notifier(buf, static_cast<Unit *>(this)->shared_from_this(), type);
	GridReferenceContainerVisitor<GridPlayerNotifier, GridReferenceContainer<AllUnitTypes>> container(notifier);

	map()->visit_in_range(map_coords(), container, range);
}

bool Player::job_change(int32_t job_id)
{
	std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(job_id);

	if (job == nullptr) {
		HLog(error) << "Player::job_change: Invalid job_id " << job_id << " provided, job was not found or not supported.";
		return false;
	}

	set_job_id(job_id);
	status()->base_appearance()->set(job_id);

	return true;
}

bool Player::perform_skill(int16_t skill_id, int16_t skill_lv)
{
	std::shared_ptr<const skill_config_data> sk_d = SkillDB->get_skill_by_id(skill_id);

    if (sk_d == nullptr) {
        HLog(warning) << "Tried to perform skill for non-existent id " << skill_id << ", ignoring...";
        return false;
    }

    try {
		std::string script_root_path = sZone->config().get_script_root_path().string();
		sol::load_result fx = lua_state()->load_file(script_root_path + "skills/" + sk_d->name + ".lua");
        sol::protected_function_result result = fx(shared_from_this(), skill_id, skill_lv);
        if (!result.valid()) {
            sol::error err = result;
            HLog(error) << "Unit::perform_skill: " << err.what();
        }
    } catch (sol::error &e) {
        HLog(error) << "Unit::perform_skill: " << e.what();
        return false;
    }

    return true;
}

bool Player::on_skill_failure(int16_t skill_id, int message_type, int item_id, skill_use_fail_cause_type cause)
{
	get_session()->clif()->notify_skill_fail(skill_id, message_type, item_id, cause);
	return true;
}

bool Player::on_action_request(player_action_type action)
{
	try {
		sol::load_result fx = lua_state()->load_file(sZone->config().get_script_root_path().string() + "internal/on_player_action.lua");
		sol::protected_function_result result = fx(shared_from_this()->downcast<Player>(), action);
		if (!result.valid()) {
			sol::error err = result;
			HLog(error) << "Player::on_action_request: " << err.what();
		}
	} catch (sol::error &e) {
		HLog(error) << "Player::on_action_request: " << e.what();
	}
	return true;
}

bool Player::perform_action(player_action_type action)
{
	switch(action)
	{
		case PLAYER_ACT_SIT:
			get_session()->clif()->notify_action(PLAYER_ACT_SIT);
			break;
		case PLAYER_ACT_STAND:
			get_session()->clif()->notify_action(PLAYER_ACT_STAND);
			break;
		case PLAYER_ACT_ATTACK:
		case PLAYER_ACT_ATTACK_REPEAT:
			break;
		default:
			break;
	};

	return true;
}

bool Player::attack(std::shared_ptr<Unit> target, bool continuous)
{
	if (Unit::attack(target, continuous) == false)
		return false;

	return true;
}

bool Player::stop_attack()
{
	if (map()->container()->getScheduler().Count(get_scheduler_task_id(UNIT_SCHEDULE_ATTACK)) > 0)
		map()->container()->getScheduler().CancelGroup(get_scheduler_task_id(UNIT_SCHEDULE_ATTACK));

	return true;
}