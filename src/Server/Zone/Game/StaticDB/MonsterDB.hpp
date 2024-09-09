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

#ifndef HORIZON_ZONE_STATICDB_MONSTERDB_HPP
#define HORIZON_ZONE_STATICDB_MONSTERDB_HPP
#include "Core/Multithreading/LockedLookupTable.hpp"
 // Linux
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Definitions/MonsterDefinitions.hpp"
#include <sol/sol.hpp>

namespace Horizon
{
namespace Zone
{
class MonsterDatabase
{
public:
	MonsterDatabase();
	~MonsterDatabase();
	
	static MonsterDatabase *get_instance()
	{
		static MonsterDatabase instance;
		return &instance;
	}
	
	bool load();

protected:
	bool load_internal(sol::object const &key, sol::object const &value);

	bool parse_level(sol::table const &table, monster_config_data &data);
	bool parse_hp(sol::table const &table, monster_config_data &data);
	bool parse_sp(sol::table const &table, monster_config_data &data);
	bool parse_reward_base_exp(sol::table const &table, monster_config_data &data);
	bool parse_reward_job_exp(sol::table const &table, monster_config_data &data);
	bool parse_attack_range(sol::table const &table, monster_config_data &data);
	bool parse_attack(sol::table const &table, monster_config_data &data);
	bool parse_defense(sol::table const &table, monster_config_data &data);
	bool parse_magic_defense(sol::table const &table, monster_config_data &data);
	bool parse_view_range(sol::table const &table, monster_config_data &data);
	bool parse_chase_range(sol::table const &table, monster_config_data &data);
	bool parse_size(sol::table const &table, monster_config_data &data);
	bool parse_race(sol::table const &table, monster_config_data &data);
	bool parse_element(sol::table const &table, monster_config_data &data);
	bool parse_mode(sol::table const &table, monster_config_data &data);
	bool parse_move_speed(sol::table const &table, monster_config_data &data);
	bool parse_attack_delay(sol::table const &table, monster_config_data &data);
	bool parse_attack_motion(sol::table const &table, monster_config_data &data);
	bool parse_damage_motion(sol::table const &table, monster_config_data &data);
	bool parse_mvp_exp(sol::table const &table, monster_config_data &data);
	bool parse_damage_taken_rate(sol::table const &table, monster_config_data &data);

	bool parse_stats(sol::table const &table, monster_config_data &data);
	bool parse_drops(sol::table const &table, monster_config_data &data, bool mvp);
	bool parse_view(sol::table const &table, monster_config_data &data);


	bool load_skill_internal(sol::object const &key, sol::object const &value);

public:
	std::shared_ptr<const monster_config_data> get_monster_by_id(uint32_t id) { return _monster_db.at(id, nullptr); }
	std::shared_ptr<const monster_config_data> get_monster_by_name(std::string name) { return _monster_str_db.at(name); }

	std::shared_ptr<std::vector<std::shared_ptr<const monster_skill_config_data>>> get_monster_skill_by_id(uint32_t monster_id) { return _monster_skill_db.at(monster_id); }
private:
	LockedLookupTable<uint32_t, std::shared_ptr<const monster_config_data>> _monster_db;
	LockedLookupTable<std::string, std::shared_ptr<const monster_config_data>> _monster_str_db;
	LockedLookupTable<uint32_t, std::shared_ptr<std::vector<std::shared_ptr<const monster_skill_config_data>>>> _monster_skill_db;
};
}
}

#define MonsterDB Horizon::Zone::MonsterDatabase::get_instance()

#endif /* HORIZON_ZONE_STATICDB_MONSTERDB_HPP */
