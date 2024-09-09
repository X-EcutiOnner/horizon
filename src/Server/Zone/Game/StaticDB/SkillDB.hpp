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

#ifndef HORIZON_ZONE_STATICDB_SKILLDB_HPP
#define HORIZON_ZONE_STATICDB_SKILLDB_HPP
#include "Core/Multithreading/LockedLookupTable.hpp"
 // Linux
#include "Server/Zone/Definitions/BattleDefinitions.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Definitions/SkillDefinitions.hpp"

#include <sol/sol.hpp>

namespace Horizon
{
namespace Zone
{
class SkillDatabase
{
public:
	SkillDatabase();
	~SkillDatabase();
	
	static SkillDatabase *get_instance()
	{
		static SkillDatabase instance;
		return &instance;
	}

	bool load();

protected:
	bool load_internal_skill_db(sol::object const &key, sol::object const &value);
	bool load_internal_skill_tree(sol::object const &key, sol::object const &value);

	bool parse_range(sol::table const &table, skill_config_data &data);
	bool parse_hit(sol::table const &table, skill_config_data &data);
	bool parse_skill_type(sol::table const &table, skill_config_data &data);
	bool parse_skill_info(sol::table const &table, skill_config_data &data);
	bool parse_attack_type(sol::table const &table, skill_config_data &data);
	bool parse_element(sol::table const &table, skill_config_data &data);
	bool parse_damage_type(sol::table const &table, skill_config_data &data);
	bool parse_splash_range(sol::table const &table, skill_config_data &data);
	bool parse_number_of_hits(sol::table const &table, skill_config_data &data);
	bool parse_interrupt_cast(sol::table const &table, skill_config_data &data);
	bool parse_cast_defense_rate(sol::table const &table, skill_config_data &data);
	bool parse_max_skill_instances(sol::table const &table, skill_config_data &data);
	bool parse_knock_back_tiles(sol::table const &table, skill_config_data &data);
	bool parse_cast_time(sol::table const &table, skill_config_data &data);
	bool parse_after_cast_act_delay(sol::table const &table, skill_config_data &data);
	bool parse_after_cast_walk_delay(sol::table const &table, skill_config_data &data);
	bool parse_skill_data1(sol::table const &table, skill_config_data &data);
	bool parse_skill_data2(sol::table const &table, skill_config_data &data);
	bool parse_cooldown(sol::table const &table, skill_config_data &data);
	bool parse_fixed_cast_time(sol::table const &table, skill_config_data &data);
	bool parse_cast_time_options(sol::table const &table, skill_config_data &data);
	bool parse_skill_delay_options(sol::table const &table, skill_config_data &data);
	bool parse_requirements(sol::table const &table, skill_config_data &data);

	bool parse_req_hp_cost(sol::table const &table, skill_config_data &data);
	bool parse_req_sp_cost(sol::table const &table, skill_config_data &data);
	bool parse_req_hp_rate_cost(sol::table const &table, skill_config_data &data);
	bool parse_req_sp_rate_cost(sol::table const &table, skill_config_data &data);
	bool parse_req_max_hp_trigger(sol::table const &table, skill_config_data &data);
	bool parse_req_max_sp_trigger(sol::table const &table, skill_config_data &data);
	bool parse_req_zeny_cost(sol::table const &table, skill_config_data &data);
	bool parse_req_weapon_types(sol::table const &table, skill_config_data &data);
	bool parse_req_ammunition_type(sol::table const &table, skill_config_data &data);
	bool parse_req_ammunition_cost(sol::table const &table, skill_config_data &data);
	bool parse_req_state(sol::table const &table, skill_config_data &data);
	bool parse_req_spirit_sphere_cost(sol::table const &table, skill_config_data &data);
	bool parse_req_items(sol::table const &table, skill_config_data &data, bool equips);

	bool parse_placement(sol::table const &table, skill_config_data &data);
	bool parse_placement_id(sol::table const &table, skill_config_data &data);
	bool parse_placement_layout(sol::table const &table, skill_config_data &data);
	bool parse_placement_range(sol::table const &table, skill_config_data &data);
	bool parse_placement_interval(sol::table const &table, skill_config_data &data);
	bool parse_placement_target(sol::table const &table, skill_config_data &data);
	bool parse_placement_flag(sol::table const &table, skill_config_data &data);

	template <typename T>
	void fill_lvl_range(T *setting, T value);

public:
	std::shared_ptr<const skill_config_data> get_skill_by_id(int32_t id) { return _skill_db.at(id); }
	std::shared_ptr<const skill_config_data> get_skill_by_name(std::string name) { return _skill_str_db.at(name); }

	std::vector<std::shared_ptr<const skill_tree_config>> get_skill_tree_by_job_id(job_class_type job_id) 
	{
		return _skill_tree_db.at(job_id, std::vector<std::shared_ptr<const skill_tree_config>>()); 
	}

	std::shared_ptr<const skill_tree_config> get_skill_tree_skill_id_by_job_id(job_class_type job_id, int16_t skill_id)
	{
		for (auto stc : get_skill_tree_by_job_id(job_id)) {
			if (stc->skill_id == skill_id)
				return stc;
		}

		return nullptr;
	}

private:
	LockedLookupTable<uint32_t, std::shared_ptr<const skill_config_data>> _skill_db;
	LockedLookupTable<std::string, std::shared_ptr<const skill_config_data>> _skill_str_db;
	LockedLookupTable<job_class_type, std::vector<std::shared_ptr<const skill_tree_config>>> _skill_tree_db;
};
}
}

template <typename T>
void Horizon::Zone::SkillDatabase::fill_lvl_range(T *setting, T value)
{
	for (int i = 0; i < MAX_SKILL_LEVEL; i++)
		setting[i] = value;
}

#define SkillDB Horizon::Zone::SkillDatabase::get_instance()

#endif /* HORIZON_ZONE_STATICDB_SKILLDB_HPP */
