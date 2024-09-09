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

#include "StatusEffectDB.hpp"

#include "Server/Zone/Script/Components/StatusEffectComponent.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

StatusEffectDatabase::StatusEffectDatabase()
{
	//
}

StatusEffectDatabase::~StatusEffectDatabase()
{
	//
}

bool StatusEffectDatabase::load()
{
	std::shared_ptr<sol::state> lua = std::make_shared<sol::state>();
	
	lua->open_libraries(sol::lib::base);
	lua->open_libraries(sol::lib::package);

	std::shared_ptr<StatusEffectComponent> sec = std::make_shared<StatusEffectComponent>();

	sec->sync_definitions(lua);
	sec->sync_data_types(lua);
	sec->sync_functions(lua);

	/**
	 * Skill DB
	 */
    try {
		int total_entries = 0;
		std::string file_path = sZone->config().get_static_db_path().string() + "status_effect_db.lua";
        sol::load_result fx = lua->load_file(file_path);
		sol::table status_tbl = fx();
		status_tbl.for_each([this, &total_entries] (sol::object const &key, sol::object const &value) {
			total_entries += load_internal(key, value) ? 1 : 0;
		});
		HLog(info) << "Loaded " << total_entries << " entries from '" << file_path << "'.";
    } catch (sol::error &e) {
        HLog(error) << "StatusEffectDatabase::load: " << e.what();
        return false;
    }

	return true;
}

bool StatusEffectDatabase::load_internal(sol::object const &key, sol::object const &value)
{
	try {
		status_effect_config_data se_conf;
		if (!key.is<std::string>()) {
			HLog(error) << "String expected for key name in 'db/status_effect_db.lua', unknown type provided... skipping.";
			return false;
		}

		se_conf.name = key.as<std::string>();

		if (value.get_type() != sol::type::table) {
			HLog(error) << "Table expected for value of entry '" << se_conf.name << "' unknown type provided... skipping.";
			return false;
		}

		sol::table v_tbl = value.as<sol::table>();

		int status_id = v_tbl.get_or("Id", -1);
		
		if (status_id == -1) {
			HLog(error) << "Id is a mandatory field, Id was not provided for status effect '" << se_conf.name << "'... skipping.";
			return false;
		}

		se_conf.status_id = status_id;

		se_conf.visible = v_tbl.get_or("Visible", false);

		sol::optional<sol::table> maybe_tbl = v_tbl.get<sol::optional<sol::table>>("Behavior");
		if (maybe_tbl) {
			sol::table a_tbl = maybe_tbl.value();

			for (auto const &a : a_tbl) {
				sol::object const &v = a.second;

				if (v.get_type() != sol::type::number) {
					HLog(error) << "Number expected for value of Behavior in status '" << se_conf.name << "', unknown type provided... skipping.";
					continue;
				}

				se_conf.behavior |= v.as<int>();
			}
		}

		se_conf.icon = v_tbl.get_or("Icon", std::string(""));

		se_conf.on_start_script = v_tbl.get_or("OnStart", std::string(""));

		se_conf.on_end_script = v_tbl.get_or("OnEnd", std::string(""));

		se_conf.per_second_script = v_tbl.get_or("PerSecond", std::string(""));

		_status_effect_db.insert(se_conf.status_id, std::make_shared<status_effect_config_data>(se_conf));
	} catch (sol::error &e) {
		HLog(error) << "StatusEffectDatabase::load_internal: " << e.what();
	}
	return true;
}
