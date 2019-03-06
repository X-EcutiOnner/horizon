/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2019 Horizon Dev Team.
 *
 * Base Author - Sxyz (sagunxp@gmail.com)
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

#ifndef HORIZON_ZONE_GAME_ASSETS_INVENTORY_HPP
#define HORIZON_ZONE_GAME_ASSETS_INVENTORY_HPP

#include "ItemStore.hpp"
#include "Common/Definitions/ItemDefinitions.hpp"

#include <unordered_map>
#include <array>
namespace Horizon
{
namespace Zone
{
class PacketHandler;
namespace Game
{
	namespace Entities
	{
		class Player;
	}
namespace Assets
{
class Inventory : public ItemStore
{
	typedef std::array<std::pair<item_equip_location_mask, std::weak_ptr<const item_entry_data>>, IT_EQPI_MAX> EquippedItemsArray;
public:
	Inventory(std::weak_ptr<Entities::Player> player, uint32_t max_storage);
	virtual ~Inventory();

	std::shared_ptr<PacketHandler> get_packet_handler() { return _packet_handler.lock(); }

	itemstore_addition_result_type add_item(uint32_t item_id, uint16_t amount, bool is_identified = false);

	bool use_item(uint32_t inventory_index, uint32_t guid);
	item_equip_result_type equip_item(uint32_t inventory_index, uint16_t equip_location_mask);
	item_unequip_result_type unequip_item(uint32_t inventory_index);

	EquippedItemsArray &get_equipped_items() { return _equipped_items; }

	void notify_all();
	void notify_without_equipments();
	void notify_only_equipments();
	void notify_add(item_entry_data const &data, uint16_t amount, itemstore_addition_result_type fail);
	void notify_deletion(uint16_t idx, uint16_t amount, itemstore_deletion_reason_type reason);
	void notify_drop(uint16_t idx, uint16_t amount);
	void notify_move_fail(uint16_t idx, bool silent);

protected:
	uint32_t calculate_current_equip_location_mask(std::shared_ptr<const item_config_data> item);
private:
	std::weak_ptr<PacketHandler> _packet_handler;
	EquippedItemsArray _equipped_items;
};
}
}
}
}
#endif /* HORIZON_ZONE_GAME_ASSETS_INVENTORY_HPP */
