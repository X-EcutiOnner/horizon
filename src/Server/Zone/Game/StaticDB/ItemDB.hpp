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

#ifndef HORIZON_ZONE_STATICDB_ITEMDB_HPP
#define HORIZON_ZONE_STATICDB_ITEMDB_HPP
#include "Core/Multithreading/LockedLookupTable.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include <sol/sol.hpp>

namespace Horizon
{
namespace Zone
{
class ItemDatabase
{
// bonus values and upgrade chances for refining equipment
struct refine_config {
	int chance[REFINE_CHANCE_TYPE_MAX][MAX_REFINE_LEVEL]{{0}}; // success chance
	int bonus[MAX_REFINE_LEVEL]{0}; // cumulative fixed bonus damage
	int randombonus_max[MAX_REFINE_LEVEL]{0}; // cumulative maximum random bonus damage
};
public:
	ItemDatabase();
	~ItemDatabase() { }

	static ItemDatabase *get_instance()
	{
		static ItemDatabase instance;
		return &instance;
	}

	bool load();
	bool load_refine_db();
	bool load_weapon_target_size_modifiers_db();
	bool load_weapon_attribute_modifiers_db();

	bool add_job_group_to_item(std::string const &group, item_config_data &id, bool enable, std::string const &file_path);

	std::shared_ptr<const item_config_data> get_item_by_id(uint32_t item_id) const { return _item_db.at(item_id); }
	std::shared_ptr<const item_config_data> get_item_by_key_name(std::string key_name) const { return _item_db_str.at(key_name); }

	std::shared_ptr<const refine_config> get_refine_config(refine_type type)
	{
		return _refine_db.at(type, std::shared_ptr<refine_config>());
	}

	uint8_t get_weapon_target_size_modifier(item_weapon_type wtype, unit_size_type stype)
	{
		std::shared_ptr<std::array<uint8_t, ESZ_MAX>> arr = _weapon_target_size_modifiers_db.at(wtype);
		return arr != nullptr ? (*arr)[stype] : 100;
	}

	int32_t get_weapon_attribute_modifier(int32_t weapon_lv, element_type element, element_type def_ele)
	{
		if (weapon_lv < 1 || weapon_lv > 5) 
			return 100;

		if (element < ELE_NEUTRAL || element >= ELE_MAX)
			return 100;

		if (def_ele < ELE_NEUTRAL || def_ele >= ELE_MAX)
			return 100;

		std::shared_ptr<std::array<std::array<uint8_t, ELE_MAX>, ELE_MAX>> lvl_arr = _weapon_attribute_modifiers_db.at(weapon_lv);
		std::array<uint8_t, ELE_MAX> ele_arr = lvl_arr->at(element);

		return ele_arr[def_ele];
	}

	std::string get_weapon_type_name(item_weapon_type type)
	{
		try {
			return _weapontype2name_db[type];
		} catch (std::exception & /*e*/) {
			return "Unknown";
		}
	}

private:
	int load_items(sol::table const &item_tbl, std::string file_path);
	bool load_refine_table(refine_type tbl_type, sol::table const &refine_table, std::string table_name, std::string file_path);
	std::array<std::string, IT_WT_SINGLE_MAX> _weapontype2name_db;
	LockedLookupTable<int32_t, std::shared_ptr<const item_config_data>> _item_db;
	LockedLookupTable<std::string, std::shared_ptr<const item_config_data>> _item_db_str;
	LockedLookupTable<int32_t, std::shared_ptr<const refine_config>> _refine_db;
	LockedLookupTable<int32_t, std::shared_ptr<std::array<uint8_t, ESZ_MAX>>> _weapon_target_size_modifiers_db;
	LockedLookupTable<int32_t, std::shared_ptr<std::array<std::array<uint8_t, ELE_MAX>, ELE_MAX>>> _weapon_attribute_modifiers_db;
};
}
}

#define ItemDB Horizon::Zone::ItemDatabase::get_instance()

#endif /* HORIZON_ZONE_STATICDB_ITEMDB_HPP */
