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

#ifndef HORIZON_ZONE_GAME_ASSETS_STORAGE_HPP
#define HORIZON_ZONE_GAME_ASSETS_STORAGE_HPP

#include "Server/Zone/Definitions/ItemDefinitions.hpp"

namespace Horizon
{
namespace Zone
{
enum storage_to_inventory_result_type
{
	STORAGE_TO_INVENTORY_SUCCESS          = 0, /// Success
	STORAGE_TO_INVENTORY_INVALID          = 1, /// Invalid itemid not found or negative amount
	STORAGE_TO_INVENTORY_OVER_WEIGHT      = 2, /// Overweight
	STORAGE_TO_INVENTORY_NO_INV_SPACE     = 3, /// No free place found
	STORAGE_TO_INVENTORY_OVER_QUANTITY    = 4, /// Max amount reached
	STORAGE_TO_INVENTORY_OVER_STACK_LIMIT = 5, /// Stack limitation
};
enum storage_add_item_result_type
{
	STORAGE_ADD_SUCCESS          = 0, /// Success
	STORAGE_ADD_INVALID          = 1, /// Invalid itemid not found or negative amount
	STORAGE_ADD_OVER_WEIGHT      = 2, /// Overweight
	STORAGE_ADD_NO_INV_SPACE     = 3, /// No free place found
	STORAGE_ADD_OVER_QUANTITY    = 4, /// Max amount reached
	STORAGE_ADD_OVER_STACK_LIMIT = 5, /// Stack limitation
};
enum storage_remove_item_result_type
{
	STORAGE_REMOVE_SUCCESS          = 0, /// Success
	STORAGE_REMOVE_INVALID          = 1, /// Invalid itemid not found or negative amount
};
namespace Units
{
	class Player;
	class Item;
}
namespace Assets
{
class Storage
{
	typedef std::vector<std::shared_ptr<item_entry_data>> storage_type;
public:
	Storage(std::shared_ptr<Horizon::Zone::Units::Player> player, int32_t storage_id, std::string name, uint32_t max_storage);
	virtual ~Storage();

	int32_t load();
	int32_t save();

	int16_t get_free_index();
	int32_t get_storage_id() { return _storage_id; }
	std::string name() { return _name; }
	int32_t max_storage() { return _max_storage; }
	std::shared_ptr<Horizon::Zone::Units::Player> player() { return _player.lock(); }


	storage_add_item_result_type add_item(std::shared_ptr<item_entry_data> item, int amount);
	storage_remove_item_result_type remove_item(int32_t index, int amount);

	std::shared_ptr<item_entry_data> get_item(int32_t storage_index);
	storage_add_item_result_type add_item_from_inventory(int16_t inventory_index, int amount);
	storage_to_inventory_result_type add_item_to_inventory(int storage_index, int amount);
    
	void notify_all();
    void notify_normal();
    void notify_equips();

private:
	int32_t _storage_id{ 0 };
	std::string _name{""};
	int32_t _max_storage{ 0 };
	std::weak_ptr<Horizon::Zone::Units::Player> _player;
	storage_type _storage_items, _saved_storage_items;
};
}
}
}

#endif // HORIZON_ZONE_GAME_ASSETS_STORAGE_HPP