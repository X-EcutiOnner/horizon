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

#ifndef HORIZON_ZONE_STATICDB_JOBDB_HPP
#define HORIZON_ZONE_STATICDB_JOBDB_HPP

#include "Core/Multithreading/LockedLookupTable.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include <array>
#include <sol/sol.hpp>
#include <map>

namespace Horizon
{
namespace Zone
{
struct job_config_data
{
	int id{0};
	std::string name{""};
	int max_weight{20000};
	std::string base_exp_group{""}, job_exp_group{""};
	std::array<int, IT_WT_SINGLE_MAX> weapon_base_aspd { 0 };
	std::vector<int> hp_table { 0 }, sp_table { 0 };
};
class JobDatabase
{
public:
	JobDatabase();
	~JobDatabase() { }

	static JobDatabase *get_instance()
	{
		static JobDatabase instance;
		return &instance;
	}

	job_class_type get_job_class_by_name(std::string name) const;
	job_class_mask job_id_to_mask(job_class_type job_id) const;

	bool load();
	int load_job(sol::table &job_tbl, std::string name = "");
	bool load_job_internal(sol::table &job_tbl, job_config_data &data, std::string job_name = "");
	bool load_hp_sp_table(sol::table &job_tbl, job_config_data &data, std::string &job_name, std::string table_name);

	std::shared_ptr<const job_config_data> get_job_by_id(uint16_t job_id) { return _job_db.at((job_class_type) job_id); }
	std::string get_job_name_by_id(int32_t id) { 
		for (auto j = _name2id_list.begin(); j != _name2id_list.end(); j++)
			if (j->second == id)
				return j->first;
		return "";
	}
private:
	LockedLookupTable<uint32_t, std::shared_ptr<const job_config_data>> _job_db;
	std::map<std::string, int> _name2id_list;
};
}
}

#define JobDB Horizon::Zone::JobDatabase::get_instance()

#endif /* HORIZON_ZONE_STATICDB_JOBDB_HPP */
