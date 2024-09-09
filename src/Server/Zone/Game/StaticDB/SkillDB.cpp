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

#include "SkillDB.hpp"

#include "Server/Zone/Script/Components/CombatComponent.hpp"
#include "Server/Zone/Script/Components/ItemComponent.hpp"
#include "Server/Zone/Script/Components/SkillComponent.hpp"
#include "Server/Zone/Script/Components/UnitComponent.hpp"

#include "Server/Zone/Game/StaticDB/ItemDB.hpp"
#include "Server/Zone/Game/StaticDB/JobDB.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

SkillDatabase::SkillDatabase()
{
	//
}

SkillDatabase::~SkillDatabase()
{
	//
}

bool SkillDatabase::load()
{
	std::shared_ptr<sol::state> lua = std::make_shared<sol::state>();
	
	lua->open_libraries(sol::lib::base);
	lua->open_libraries(sol::lib::package);

	std::shared_ptr<CombatComponent> combat_component = std::make_shared<CombatComponent>();
	std::shared_ptr<ItemComponent> item_component = std::make_shared<ItemComponent>();
	std::shared_ptr<SkillComponent> skill_component = std::make_shared<SkillComponent>();
	std::shared_ptr<UnitComponent> unit_component = std::make_shared<UnitComponent>();

	combat_component->sync_definitions(lua);
	combat_component->sync_data_types(lua);
	combat_component->sync_functions(lua);

	item_component->sync_definitions(lua);
	item_component->sync_data_types(lua);
	item_component->sync_functions(lua);

	skill_component->sync_definitions(lua);
	skill_component->sync_data_types(lua);
	skill_component->sync_functions(lua);

	unit_component->sync_definitions(lua);
	unit_component->sync_data_types(lua);
	unit_component->sync_functions(lua);

	/**
	 * Skill DB
	 */
    try {
		int total_entries = 0;
		std::string file_path = sZone->config().get_static_db_path().string() + "skill_db.lua";
        sol::load_result fx = lua->load_file(file_path);
		sol::table status_tbl = fx();
		status_tbl.for_each([this, &total_entries] (sol::object const &key, sol::object const &value) {
			total_entries += load_internal_skill_db(key, value) ? 1 : 0;
		});
		HLog(info) << "Loaded " << total_entries << " entries from '" << file_path << "'.";
    } catch (sol::error &e) {
        HLog(error) << "SkillDB::load: " << e.what();
        return false;
    }

	/**
	 * Skill Tree DB
	 */
	try {
		int total_entries = 0;
		std::string file_path = sZone->config().get_static_db_path().string() + "skill_tree_db.lua";
        sol::load_result fx = lua->load_file(file_path);
		sol::table skill_tree_tbl = fx();
		skill_tree_tbl.for_each([this, &total_entries] (sol::object const &key, sol::object const &value) {
			total_entries += load_internal_skill_tree(key, value) ? 1 : 0;
		});
		HLog(info) << "Loaded " << total_entries << " entries from '" << file_path << "'.";
	} catch(const std::exception &e) {
		HLog(error) << "SkillDB::load: error loading skill_tree_db: " << e.what();
		return false;
	}

	return true;
}

bool SkillDatabase::load_internal_skill_db(sol::object const &key, sol::object const &tvalue)
{
	skill_config_data data;

	sol::table stbl = tvalue.as<sol::table>();

	// ------------------------------ Mandatory Fields ----------------------------
	data.name = key.as<std::string>();
	data.skill_id = stbl.get_or<uint16_t>("Id", 0);

	if (data.skill_id == 0) {
		HLog(warning) << "Skill ID for '" << data.name << "' was 0... skipping...";
		return false;
	}

	if (data.skill_id > MAX_SKILL_ID) {
		HLog(warning) << "Skill ID for '" << data.name << "' exceeds maximum " << MAX_SKILL_ID << "... skipping.";
		return false;
	}

	data.max_level = stbl.get_or("MaxLevel", 0);

	if (data.max_level == 0) {
		HLog(warning) << "Max Level for '" << data.name << "' was 0... skipping...";
		return false;
	}

	// ------------------------------ Optional Fields -----------------------------

	data.desc = stbl.get_or<std::string>("Description", "");

	if (parse_range(stbl, data) == false)
		return false;

	if (parse_hit(stbl, data) == false)
		return false;

	if (parse_skill_type(stbl, data) == false)
		return false;

	if (parse_skill_info(stbl, data) == false)
		return false;

	if (parse_attack_type(stbl, data) == false)
		return false;

	if (parse_element(stbl, data) == false)
		return false;

	if (parse_damage_type(stbl, data) == false)
		return false;

	if (parse_splash_range(stbl, data) == false)
		return false;

	if (parse_number_of_hits(stbl, data) == false)
		return false;

	if (parse_interrupt_cast(stbl, data) == false)
		return false;

	if (parse_cast_defense_rate(stbl, data) == false)
		return false;

	if (parse_max_skill_instances(stbl, data) == false)
		return false;

	if (parse_knock_back_tiles(stbl, data) == false)
		return false;

	if (parse_cast_time(stbl, data) == false)
		return false;

	if (parse_after_cast_act_delay(stbl, data) == false)
		return false;

	if (parse_after_cast_walk_delay(stbl, data) == false)
		return false;

	if (parse_skill_data1(stbl, data) == false)
		return false;

	if (parse_skill_data2(stbl, data) == false)
		return false;

	if (parse_cooldown(stbl, data) == false)
		return false;

	if (parse_fixed_cast_time(stbl, data) == false)
		return false;

	if (parse_cast_time_options(stbl, data) == false)
		return false;

	if (parse_skill_delay_options(stbl, data) == false)
		return false;

	if (parse_requirements(stbl, data) == false)
		return false;

	if (parse_placement(stbl, data) == false)
		return false;

	_skill_db.insert(data.skill_id, std::make_shared<skill_config_data>(data));
	_skill_str_db.insert(data.name, std::make_shared<skill_config_data>(data));

	return true;
}

bool SkillDatabase::parse_range(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Range");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL)
				HLog(warning) << "Level table of Ranges for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &key = tp.first;
				sol::object const &svalue = tp.second;

				if (key.is<std::string>()) {
					HLog(warning) << "Key for Range of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = key.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Range of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.use_range[lvl - 1] = svalue.as<int>();
			}
		}

		sol::optional<int> maybe_intval = table.get<sol::optional<int>>("Range");
		if (maybe_intval) {
			int range = maybe_intval.value();
			fill_lvl_range(data.use_range, range);
		}

	} catch (sol::error &error) {
		HLog(error) << "Error parsing Range for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_hit(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Hit");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL)
				HLog(warning) << "Level table of Hit for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &key = tp.first;
				sol::object const &svalue = tp.second;

				if (key.is<std::string>()) {
					HLog(warning) << "Key for Hit of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = key.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Hit of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				std::string hit = svalue.as<std::string>();
				battle_damage_type bdt;
				
				if (hit.compare("BDT_SKILL") == 0) {
					bdt = BDT_SKILL;
				} else if (hit.compare("BDT_MULTIHIT") == 0) {
					bdt = BDT_MULTIHIT;
				} else if (hit.compare("BDT_NORMAL") == 0) {
					bdt = BDT_NORMAL;
				} else {
					HLog(warning) << "Value for Hit of skill '" << data.name << "' is unknown: " << hit << "... skipping.";
					return false;
				}

				data.damage_type[lvl - 1] = bdt;
			}
		}

		sol::optional<std::string> maybe_strval = table.get<sol::optional<std::string>>("Hit");
		if (maybe_strval) {
			std::string hit = maybe_strval.value();
			battle_damage_type bdt;
			
			if (hit.compare("BDT_SKILL") == 0) {
				bdt = BDT_SKILL;
			} else if (hit.compare("BDT_MULTIHIT") == 0) {
				bdt = BDT_MULTIHIT;
			} else if (hit.compare("BDT_NORMAL") == 0) {
				bdt = BDT_NORMAL;
			} else {
				HLog(warning) << "Value for Hit of skill '" << data.name << "' is unknown: " << hit << "... skipping.";
				return false;
			}

			fill_lvl_range(data.damage_type, bdt);
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Hit for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}


bool SkillDatabase::parse_skill_type(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SkillType");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			for (auto const &t : tbl) {
				sol::object const &k = t.first;
				sol::object const &v = t.second;

				int type = v.as<int>();

				if (type < SK_TYPE_PASSIVE || type > SK_TYPE_TRAP) {
					HLog(error) << "In config for skill '" << data.name << "' SkillType is out of range.";
					continue;
				}

				data.primary_type |= type;
			}
		} else {
			data.primary_type = SK_TYPE_PASSIVE;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SkillType for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_skill_info(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SkillInfo");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			for (auto const &t : tbl) {
				sol::object const &k = t.first;
				sol::object const &v = t.second;

				int type = v.as<int>();

				if (type < SK_SUBTYPE_NONE || type > SK_SUBTYPE_IS_COMBO_SKILL) {
					HLog(error) << "In config for skill '" << data.name << "' SkillInfo is out of range.";
					continue;
				}
				
				data.secondary_type |= type;
			}
		} else {
			data.secondary_type = SK_SUBTYPE_NONE;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SkillInfo for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_attack_type(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("AttackType");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of AttackType for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for AttackType of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for AttackType of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.attack_type[lvl - 1] = (battle_attack_type) v.as<int>();
			}
		}

		sol::optional<int> maybe_intval = table.get<sol::optional<int>>("AttackType");
		if (maybe_intval) {
			battle_attack_type bat = (battle_attack_type) maybe_intval.value();
			fill_lvl_range(data.attack_type, bat);
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AttackType for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}


bool SkillDatabase::parse_element(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Element");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of Element for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for Element of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Element of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.element[lvl - 1] = (element_type) v.as<int>();
			}
		}

		sol::optional<int> maybe_intval = table.get<sol::optional<int>>("Element");
		if (maybe_intval) {
			element_type e = (element_type) maybe_intval.value();
			fill_lvl_range(data.element, e);
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Element for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_damage_type(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("DamageType");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of DamageType for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for DamageType of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for DamageType of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.damage_type[lvl - 1] = (battle_damage_type) v.as<int>();
			}
		}

		sol::optional<int> maybe_intval = table.get<sol::optional<int>>("DamageType");
		if (maybe_intval) {
			fill_lvl_range(data.damage_type, (battle_damage_type) maybe_intval.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing DamageType for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}


bool SkillDatabase::parse_splash_range(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SplashRange");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of SplashRange for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for SplashRange of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for SplashRange of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.splash_range[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_intval = table.get<sol::optional<int>>("SplashRange");
		if (maybe_intval) {
			fill_lvl_range(data.splash_range, maybe_intval.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SplashRange for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_number_of_hits(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("NumberOfHits");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of NumberOfHits for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for NumberOfHits of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for NumberOfHits of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.number_of_hits[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_intval = table.get<sol::optional<int>>("NumberOfHits");
		if (maybe_intval) {
			fill_lvl_range(data.number_of_hits, maybe_intval.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing NumberOfHits for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_interrupt_cast(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("InterruptCast");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of InterruptCast for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for InterruptCast of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for InterruptCast of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.interrupt_cast[lvl - 1] = v.as<bool>();
			}
		}

		sol::optional<bool> maybe_val = table.get<sol::optional<bool>>("InterruptCast");
		if (maybe_val) {
			fill_lvl_range(data.interrupt_cast, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing InterruptCast for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_cast_defense_rate(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("CastDefRate");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of CastDefRate for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for CastDefRate of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for CastDefRate of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.cast_defense_rate[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("CastDefRate");
		if (maybe_val) {
			fill_lvl_range(data.cast_defense_rate, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing CastDefRate for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_max_skill_instances(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("MaxSkillInstances");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of MaxSkillInstances for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for MaxSkillInstances of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for MaxSkillInstances of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.max_skill_instances[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("MaxSkillInstances");
		if (maybe_val) {
			fill_lvl_range(data.max_skill_instances, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing MaxSkillInstances for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_knock_back_tiles(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("KnockBackTiles");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of KnockBackTiles for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for KnockBackTiles of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for KnockBackTiles of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.knock_back_tiles[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("KnockBackTiles");
		if (maybe_val) {
			fill_lvl_range(data.knock_back_tiles, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing KnockBackTiles for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_cast_time(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("CastTime");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of CastTime for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for CastTime of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for CastTime of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.cast_time[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("CastTime");
		if (maybe_val) {
			fill_lvl_range(data.cast_time, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing CastTime for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_after_cast_act_delay(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("AfterCastActDelay");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of AfterCastActDelay for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for AfterCastActDelay of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for AfterCastActDelay of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.after_cast_actor_delay[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("AfterCastActDelay");
		if (maybe_val) {
			fill_lvl_range(data.after_cast_actor_delay, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AfterCastActDelay for skill '" << data.name << "' - " << error.what() << ".";
	}
	
	return true;
}

bool SkillDatabase::parse_after_cast_walk_delay(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("AfterCastWalkDelay");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of AfterCastWalkDelay for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for AfterCastWalkDelay of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for AfterCastWalkDelay of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.after_cast_walk_delay[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("AfterCastWalkDelay");
		if (maybe_val) {
			fill_lvl_range(data.after_cast_walk_delay, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AfterCastWalkDelay for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_skill_data1(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SkillData1");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of SkillData1 for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for SkillData1 of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for SkillData1 of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.upkeep_time[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("SkillData1");
		if (maybe_val) {
			fill_lvl_range(data.upkeep_time, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SkillData1 for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_skill_data2(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SkillData2");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of SkillData2 for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for SkillData2 of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for SkillData2 of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.upkeep_time2[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("SkillData2");
		if (maybe_val) {
			fill_lvl_range(data.upkeep_time2, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SkillData2 for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_cooldown(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Cooldown");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of Cooldown for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for Cooldown of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Cooldown of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.cooldown[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Cooldown");
		if (maybe_val) {
			fill_lvl_range(data.cooldown, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Cooldown for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_fixed_cast_time(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("FixedCastTime");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of FixedCastTime for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for FixedCastTime of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for FixedCastTime of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.fixed_cast_time[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("FixedCastTime");
		if (maybe_val) {
			fill_lvl_range(data.fixed_cast_time, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing FixedCastTime for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_cast_time_options(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("CastTimeOptions");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			int options = 0;
				
			for (const auto &tp : tbl) {
				sol::object const &v = tp.second;

				if (v.get_type() == sol::type::string) {
					std::string option = v.as<std::string>();
					if (option.compare("IgnoreDex") == 0)
						options |= 1;
					else if (option.compare("IgnoreStatusEffect") == 0)
						options |= 2;
					else if (option.compare("IgnoreItemBonus") == 0)
						options |= 4;
				} else {
					HLog(warning) << "Invalid type provided for CastTimeOptions of skill '" << data.name << "'.";
					continue;
				}
			}
			data.cast_time_options = options;
		}

		sol::optional<std::string> maybe_val = table.get<sol::optional<std::string>>("CastTimeOptions");
		if (maybe_val) {
			std::string option = maybe_val.value();
			int options = 0;

			if (option.compare("IgnoreDex") == 0)
				options |= 1;
			else if (option.compare("IgnoreStatusEffect") == 0)
				options |= 2;
			else if (option.compare("IgnoreItemBonus") == 0)
				options |= 4;

			data.cast_time_options = options;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing CastTimeOptions for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_skill_delay_options(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SkillDelayOptions");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			int options = 0;
				
			for (const auto &tp : tbl) {
				sol::object const &v = tp.second;

				if (v.get_type() == sol::type::string) {
					std::string option = v.as<std::string>();
					if (option.compare("IgnoreDex") == 0)
						options |= 1;
					else if (option.compare("IgnoreStatusEffect") == 0)
						options |= 2;
					else if (option.compare("IgnoreItemBonus") == 0)
						options |= 4;
				} else {
					HLog(warning) << "Invalid type provided for SkillDelayOptions of skill '" << data.name << "'.";
					continue;
				}
			}
			data.skill_delay_options = options;
		}

		sol::optional<std::string> maybe_val = table.get<sol::optional<std::string>>("SkillDelayOptions");
		if (maybe_val) {
			std::string option = maybe_val.value();
			int options = 0;

			if (option.compare("IgnoreDex") == 0)
				options |= 1;
			else if (option.compare("IgnoreStatusEffect") == 0)
				options |= 2;
			else if (option.compare("IgnoreItemBonus") == 0)
				options |= 4;

			data.skill_delay_options = options;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SkillDelayOptions for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_requirements(sol::table const &table, skill_config_data &data)
{
	sol::optional<sol::table> maybe_requirements = table.get<sol::optional<sol::table>>("Requirements");

	if (maybe_requirements) {
		sol::table req_tbl = maybe_requirements.value();

		if (parse_req_hp_cost(req_tbl, data) == false)
			return false;

		if (parse_req_sp_cost(req_tbl, data) == false)
			return false;

		if (parse_req_hp_rate_cost(req_tbl, data) == false)
			return false;

		if (parse_req_sp_rate_cost(req_tbl, data) == false)
			return false;

		if (parse_req_max_hp_trigger(req_tbl, data) == false)
			return false;

		if (parse_req_max_sp_trigger(req_tbl, data) == false)
			return false;

		if (parse_req_zeny_cost(req_tbl, data) == false)
			return false;

		if (parse_req_weapon_types(req_tbl, data) == false)
			return false;

		if (parse_req_ammunition_type(req_tbl, data) == false)
			return false;

		if (parse_req_ammunition_cost(req_tbl, data) == false)
			return false;

		if (parse_req_state(req_tbl, data) == false)
			return false;

		if (parse_req_spirit_sphere_cost(req_tbl, data) == false)
			return false;

		if (parse_req_items(req_tbl, data, false) == false)
			return false;

		if (parse_req_items(req_tbl, data, true) == false)
			return false;
	}
	
	return true;
}

bool SkillDatabase::parse_req_hp_cost(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("HPCost");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of HPCost for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for HPCost of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for HPCost of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.hp_cost[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("HPCost");
		if (maybe_val) {
			fill_lvl_range(data.hp_cost, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing HPCost for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_sp_cost(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SPCost");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of SPCost for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for SPCost of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for SPCost of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.sp_cost[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("SPCost");
		if (maybe_val) {
			fill_lvl_range(data.sp_cost, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SPCost for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_hp_rate_cost(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("HPRateCost");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of HPRateCost for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for HPRateCost of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for HPRateCost of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.hp_percent_cost[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("HPRateCost");
		if (maybe_val) {
			fill_lvl_range(data.hp_percent_cost, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing HPRateCost for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_sp_rate_cost(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SPRateCost");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of SPRateCost for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for SPRateCost of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for SPRateCost of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.sp_percent_cost[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("SPRateCost");
		if (maybe_val) {
			fill_lvl_range(data.sp_percent_cost, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SPRateCost for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_max_hp_trigger(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("MaxHPTrigger");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of MaxHPTrigger for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for MaxHPTrigger of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for MaxHPTrigger of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.max_hp_trigger[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("MaxHPTrigger");
		if (maybe_val) {
			fill_lvl_range(data.max_hp_trigger, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing MaxHPTrigger for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_max_sp_trigger(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("MaxSPTrigger");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of MaxSPTrigger for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for MaxSPTrigger of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for MaxSPTrigger of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.max_sp_trigger[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("MaxSPTrigger");
		if (maybe_val) {
			fill_lvl_range(data.max_sp_trigger, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing MaxSPTrigger for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_zeny_cost(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("ZenyCost");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of ZenyCost for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for ZenyCost of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for ZenyCost of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.zeny_cost[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("ZenyCost");
		if (maybe_val) {
			fill_lvl_range(data.zeny_cost, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing ZenyCost for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_weapon_types(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("WeaponTypes");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			for (auto const &t : tbl) {
				sol::object const &k = t.first;
				sol::object const &v = t.second;

				int type = v.as<int>();

				if (type < IT_WT_FIST || type > IT_WT_MAX_WEAPON_TYPE) {
					HLog(error) << "In config for skill '" << data.name << "' WeaponTypes is out of range.";
					continue;
				}
				
				data.weapon_type |= type;
			}
		} else {
			data.weapon_type = IT_WT_FIST;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing WeaponTypes for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_ammunition_type(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("AmmoTypes");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			for (auto const &t : tbl) {
				sol::object const &k = t.first;
				sol::object const &v = t.second;

				int type = v.as<int>();

				if (type < IT_AT_NONE || type > IT_AT_MAX) {
					HLog(error) << "In config for skill '" << data.name << "' AmmoTypes is out of range.";
					continue;
				}
				
				data.ammunition_type |= type;
			}
		} else {
			data.ammunition_type = IT_AT_NONE;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AmmoTypes for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_ammunition_cost(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("AmmoAmount");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of AmmoAmount for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for AmmoAmount of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for AmmoAmount of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.ammunition_cost[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("AmmoAmount");
		if (maybe_val) {
			fill_lvl_range(data.ammunition_cost, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AmmoAmount for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_state(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("State");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			for (auto const &t : tbl) {
				sol::object const &k = t.first;
				sol::object const &v = t.second;

				int lvl = k.as<int>();

				if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1) {
					HLog(warning) << "Level table of State for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";
					return false;
				}

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Level for State of skill '" << data.name << "' was out of range.";
					return false;
				}

				if (v.get_type() == sol::type::table) {
					int states = 0;
					for (auto const &x : v.as<sol::table>()) {
						sol::object const &y = x.second;
						int state = y.as<int>();

						if (state < SRS_NONE || state > SRS_MAX) {
							HLog(error) << "In config for skill '" << data.name << "' State is out of range for level " << lvl << ".";
							continue;
						}

						states |= state;
					}

					data.required_state[lvl - 1] = states;
				} else if (v.get_type() == sol::type::number) {
					int state = v.as<int>();

					if (state < SRS_NONE || state > SRS_MAX) {
						HLog(error) << "In config for skill '" << data.name << "' State is out of range for level " << lvl << ".";
						continue;
					}
					
					data.required_state[lvl - 1] |= state;
				}
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("State");
		if (maybe_val) {
			fill_lvl_range(data.required_state, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing State for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_spirit_sphere_cost(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("SpiritSphereCost");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of SpiritSphereCost for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for SpiritSphereCost of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for SpiritSphereCost of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.spirit_sphere_cost[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("SpiritSphereCost");
		if (maybe_val) {
			fill_lvl_range(data.spirit_sphere_cost, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SpiritSphereCost for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_req_items(sol::table const &table, skill_config_data &data, bool equips)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>(equips ? "Equips" : "Items");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();
			skill_required_item_data &rdata = equips ? data.req_equip : data.req_items;

			int i = 0;
			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;
				std::shared_ptr<const item_config_data> i_data = nullptr;

				if (i >= MAX_SKILL_ITEM_REQUIRE) {
					HLog(warning) << "Size of list of required items is greater than allowed " << MAX_SKILL_ITEM_REQUIRE << " for skill '" << data.name << "'... skipping.";
					break;
				}

				if (k.is<std::string>()) {
					std::string item_name = k.as<std::string>();

					if (item_name.compare("Any") == 0) {
						if (v.get_type() == sol::type::number) {
							bool any = v.as<bool>();
							fill_lvl_range(rdata.any, any);
						} else if (v.get_type() == sol::type::table) {
							sol::table lvls = v.as<sol::table>();

							for (auto const &l : lvls) {
								sol::object const &lk = l.first;
								sol::object const &lv = l.second;

								if (lk.is<std::string>()) {
									HLog(warning) << "Key of any-flag for skill '" << data.name << "' was string, expected int.";
									continue;
								}

								int lvl = lk.as<int>();

								if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
									HLog(warning) << "Level of any-flag for skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
									continue;
								}

								if (lv.get_type() != sol::type::boolean) {
									HLog(warning) << "Level of any-flag for skill '" << data.name << "' is not of boolean type.";
									continue;
								}

								rdata.any[lvl - 1] = lv.as<bool>();
							}
						} else {
							HLog(warning) << "Invalid type provided at as amount of 'Items' for skill '" << data.name << "'... skipping.";
							continue;
						}
					}

					i_data = ItemDB->get_item_by_key_name(item_name);
					if (i_data == nullptr) {
						HLog(warning) << "Item with Name " << item_name << " couldn't be found in 'Requirements > Items' for skill '" << data.name << "'... skipping.";
						continue;
					}
				} else if (k.is<int>()) {
					int id = k.as<int>();
					i_data = ItemDB->get_item_by_id(id);
					
					if (i_data == nullptr) {
						HLog(warning) << "Item with ID " << id << " couldn't be found in 'Requirements > Items' for skill '" << data.name << "'... skipping.";
						continue;
					}
				} else {
					HLog(warning) << "Invalid type provided at 'Requirements > Items' for skill '" << data.name << "'... skipping.";
					continue;
				}

				if (v.get_type() == sol::type::number) {
					int amount = v.as<int>();
					fill_lvl_range(rdata.item[i].amount, amount);
				} else if (v.get_type() == sol::type::table) {
					sol::table lvls = v.as<sol::table>();

					for (auto const &l : lvls) {
						sol::object const &lk = l.first;
						sol::object const &lv = l.second;

						if (lk.is<std::string>()) {
							HLog(warning) << "Key of 'Level' for skill '" << data.name << "' was string, expected int.";
							continue;
						}

						int lvl = lk.as<int>();

						if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
							HLog(warning) << "Key of 'Amount' for skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
							continue;
						}

						rdata.item[i].amount[lvl - 1] = lv.as<int>();
					}
				} else {
					HLog(warning) << "Invalid type provided at as amount of 'Items' for skill '" << data.name << "'... skipping.";
					continue;
				}
				i++;
			}

			if (equips)
				data.req_equip = rdata;
			else
				data.req_items = rdata;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing SpiritSphereCost for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_placement(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Placement");
		if (maybe_tbl) {
			sol::table p_tbl = maybe_tbl.value();

			if (parse_placement_id(p_tbl, data) == false)
				return false;

			if (parse_placement_layout(p_tbl, data) == false)
				return false;

			if (parse_placement_range(p_tbl, data) == false)
				return false;

			if (parse_placement_interval(p_tbl, data) == false)
				return false;

			if (parse_placement_interval(p_tbl, data) == false)
				return false;

			if (parse_placement_target(p_tbl, data) == false)
				return false;

			if (parse_placement_flag(p_tbl, data) == false)
				return false;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Placement for skill '" << data.name << "' : " << error.what() << ".";
		return false;
	}

	return true;
}

bool SkillDatabase::parse_placement_id(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Id");
		if (maybe_tbl) {
			sol::table p_tbl = maybe_tbl.value();

			if (p_tbl.size() == 2) {
				if (p_tbl[1].get_type() != sol::type::number || p_tbl[2].get_type() != sol::type::number) {
					HLog(warning) << "Value for Placement > Id of skill '" << data.name << "' is of an invalid type, excepted number.";
					return false;
				}

				for(int i = 0; i < MAX_SKILL_LEVEL; i++) {
					data.placement_id[i][0] = p_tbl[1];
					data.placement_id[i][1] = p_tbl[2];
				}
			} else if (p_tbl.size() > 2) {
				if (p_tbl.size() > MAX_SKILL_LEVEL || p_tbl.size() < 1)
					HLog(warning) << "Level table of Placement > Id for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << p_tbl.size() << " was provided.";

				for (auto const &c : p_tbl) {
					sol::object ck = c.first;
					sol::object cv = c.second;

					if (ck.is<std::string>()) {
						HLog(warning) << "Key for Placement > Id of skill '" << data.name << "' was string, expected int.";
						return false;
					}

					int lvl = ck.as<int>();

					if (cv.get_type() == sol::type::table) {
						sol::table cvt = cv.as<sol::table>();
						if (cvt[1].get_type() != sol::type::number || cvt[2].get_type() != sol::type::number) {
							HLog(warning) << "Value for Placement > Id of skill '" << data.name << "' is of an invalid type, excepted number.";
							return false;
						}
						data.placement_id[lvl - 1][0] = cvt[1];
						data.placement_id[lvl - 1][1] = cvt[2];
					} else if (cv.get_type() == sol::type::number) {
						data.placement_id[lvl - 1][0] = cv.as<int>();;
					} else {
						HLog(warning) << "Value for Placement > Id of skill '" << data.name << "' is of an invalid type, excepted number.";
						return false;
					}
				}
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Id");
		if (maybe_val) {
			int value = maybe_val.value();

			for(int i = 0; i < MAX_SKILL_LEVEL; i++) {
				data.placement_id[i][0] = value;
				data.placement_id[i][1] = 0;
			}
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Unit > Id for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_placement_layout(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Layout");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of Placement > Layout for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for Placement > Layout of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Placement > Layout of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.placement_layout_type[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Layout");
		if (maybe_val) {
			fill_lvl_range(data.placement_layout_type, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Placement > Layout for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_placement_range(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Range");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of Placement > Range for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for Placement > Range of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Placement > Range of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.placement_range[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Range");
		if (maybe_val) {
			fill_lvl_range(data.placement_range, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Placement > Range for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_placement_interval(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Interval");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of Placement > Interval for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for Placement > Interval of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Placement > Interval of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.placement_interval[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Interval");
		if (maybe_val) {
			fill_lvl_range(data.placement_interval, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Placement > Interval for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_placement_target(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Target");
		if (maybe_tbl) {
			sol::table tbl = maybe_tbl.value();

			if (tbl.size() > MAX_SKILL_LEVEL || tbl.size() < 1)
				HLog(warning) << "Level table of Placement > Target for skill " << data.name << " should be of size " << MAX_SKILL_LEVEL << ", but a table of size " << tbl.size() << " was provided.";

			for (const auto &tp : tbl) {
				sol::object const &k = tp.first;
				sol::object const &v = tp.second;

				if (k.is<std::string>()) {
					HLog(warning) << "Key for Placement > Target of skill '" << data.name << "' was string, expected int.";
					return false;
				}
				
				int lvl = k.as<int>();

				if (lvl > MAX_SKILL_LEVEL || lvl < 0) {
					HLog(warning) << "Key for Placement > Target of skill '" << data.name << "' was greater than " << MAX_SKILL_LEVEL << ".";
					return false;
				}

				data.placement_target[lvl - 1] = v.as<int>();
			}
		}

		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Target");
		if (maybe_val) {
			fill_lvl_range(data.placement_target, maybe_val.value());
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Placement > Target for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::parse_placement_flag(sol::table const &table, skill_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Flag");
		if (maybe_val)
			data.placement_flag = maybe_val.value();
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Placement > Flag for skill '" << data.name << "' - " << error.what() << ".";
	}

	return true;
}

bool SkillDatabase::load_internal_skill_tree(sol::object const &key, sol::object const &value)
{
	try {
		std::vector<std::shared_ptr<const skill_tree_config>> final_tree_vector;
		std::shared_ptr<const job_config_data> job = nullptr;

		if (value.get_type() != sol::type::table) {
				HLog(error) << "Value for Job " << job->id << " was not a table... skipping.";
				return false;
		}

		sol::table jsk = value.as<sol::table>();

		std::string job_name = jsk.get<std::string>("Name");

		job_class_type job_id = JobDB->get_job_class_by_name(job_name);
		if (job_id == JOB_INVALID) {
			HLog(error) << "Invalid job name '" << job_name << "' provided in skill_tree, job wasn't found.";
			return false;
		}
		job = JobDB->get_job_by_id((int) job_id);
		if (job == nullptr) {
			HLog(error) << "Job '" << job_name << "' with ID " << job_id << " wasn't found.";
			return false;
		}
		
		sol::optional<sol::table> maybe_inherit = jsk.get<sol::optional<sol::table>>("Inherit");

		if (maybe_inherit) {
			sol::table i_tbl = maybe_inherit.value();

			for(auto const &ih : i_tbl) {
				sol::object const &v = ih.second;
				job_class_type jclass;
				std::vector<std::shared_ptr<const skill_tree_config>> c;

				if (v.get_type() == sol::type::string) {
					std::string job_name = v.as<std::string>();
					jclass = JobDB->get_job_class_by_name(job_name);
					c = SkillDB->get_skill_tree_by_job_id(jclass);

					if (c.size() == 0) {
						HLog(error) << "Non-existent job '" << job_name << "' couldn't be inherited for job '" << job->name << "'.";
						continue;
					}
				} else if (v.get_type() == sol::type::number) {
					jclass = (job_class_type) v.as<int>();
					c = SkillDB->get_skill_tree_by_job_id(jclass);

					if (c.size() == 0) {
						HLog(error) << "Non-existent job " << (int) jclass << " couldn't be inherited for job '" << job->name << "'.";
						continue;
					}
				}

				for (auto it = c.begin(); it != c.end(); it++) {
					std::shared_ptr<skill_tree_config> ihdata = std::const_pointer_cast<skill_tree_config>(*it);
					ihdata->inherited_from = jclass;
					final_tree_vector.push_back(std::const_pointer_cast<const skill_tree_config>(ihdata));
				}
			}
		}

		sol::optional<sol::table> maybe_tbl = jsk.get<sol::optional<sol::table>>("Skills");
		if (maybe_tbl) {
			sol::table s_tbl = maybe_tbl.value();
			for (auto const &sk : s_tbl) {
				sol::object const &k = sk.first;
				sol::object const &v = sk.second;
				skill_tree_config tskill;
				std::shared_ptr<const skill_config_data> skill = nullptr;

				if (k.is<std::string>()) {
					std::string skill_name = k.as<std::string>();
					skill = get_skill_by_name(skill_name);
					if (skill == nullptr) {
						HLog(error) << "Skill '" << skill_name << "' for job '" << job->name << "' doesn't exist in the skill database... skipping.";
						continue;
					}
				} else if (k.is<int>()) {
					int skill_id = k.as<int>();
					skill = get_skill_by_id(skill_id);
					if (skill == nullptr) {
						HLog(error) << "Skill by id (" << skill_id << ") for job '" << job->name << "'  doesn't exist in the skill database... skipping.";
						continue;
					}
				} else {
					HLog(error) << "A Key in skills of job '" << job->name << "' was of unexpected type... skipping.";
					continue;
				}

				tskill.skill_id = skill->skill_id;
				tskill.inherited_from = JOB_INVALID;

				if (v.get_type() == sol::type::table) {
					sol::table n_tbl = v.as<sol::table>();

					for (auto const &n : n_tbl) {
						sol::object const &nk = n.first;
						sol::object const &nv = n.second;
						std::shared_ptr<const skill_config_data> pre_skill = nullptr;

						if (nk.is<std::string>()) {
							std::string setting_name = nk.as<std::string>();

							if (setting_name.compare("MaxLevel") == 0) {
								if (nv.get_type() != sol::type::number) {
									HLog(error) << "MaxLevel for skill " << skill->name << " of job " << job->id << " was of non-numeric type... skipping;";
									continue;
								}

								tskill.max_level = nv.as<int>();
								continue;
							} else if (setting_name.compare("MinJobLevel") == 0) {
								if (nv.get_type() != sol::type::number) {
									HLog(error) << "MinJobLevel for skill " << skill->name << " of job " << job->id << " was of non-numeric type... skipping;";
									continue;
								}
								
								tskill.job_level = nv.as<int>();
								continue;
							}

							pre_skill = get_skill_by_name(setting_name);
							if (pre_skill == nullptr) {
								HLog(error) << "Pre-requisite skill '" << setting_name << "' doesn't exist in the skill database... skipping.";
								continue;
							}
						} else if (nk.is<int>()) {
							int skill_id = nk.as<int>();
							pre_skill = get_skill_by_id(skill_id);
							if (pre_skill == nullptr) {
								HLog(error) << "Pre-requisite skill by id (" << skill_id << ") doesn't exist in the skill database... skipping.";
								continue;
							}
						} else {
							HLog(error) << "Unknown key type provided for pre-requisite skill of job " << job->id << " in skill database... skipping.";
							continue; 
						}

						if (nv.get_type() != sol::type::number) {
							HLog(error) << "Unknown value type for pre-requisite skill " << pre_skill->name << " of job " << job->id << "... skipping.";
							continue;
						}
						skill_tree_config::requirement req;
						req.skill_id = pre_skill->skill_id;
						req.level = nv.as<int>();
						tskill.requirements.push_back(req);
					}
				} else if (v.get_type() == sol::type::number) {
					tskill.max_level = v.as<int>();
				} else {
					HLog(error) << "Invalid value type for skill " << skill->skill_id << ", expected number or table. (Job: '" << job->name << "')";
					continue;
				}

				final_tree_vector.push_back(std::make_shared<const skill_tree_config>(tskill));
			}
		}

		_skill_tree_db.insert((job_class_type) job->id, final_tree_vector);

	} catch (sol::error &err) {
		HLog(error) << "SkillDatabase::load_internal_skill_tree:" << err.what();
		return false;
	}

	return true;
}
