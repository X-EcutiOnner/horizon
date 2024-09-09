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

#include "MonsterDB.hpp"

#include "Server/Zone/Game/StaticDB/ItemDB.hpp"
#include "Server/Zone/Game/StaticDB/SkillDB.hpp"
#include "Server/Zone/Script/Components/MonsterComponent.hpp"
#include "Server/Zone/Script/Components/UnitComponent.hpp"
#include "Server/Zone/Script/Components/ItemComponent.hpp"

#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

MonsterDatabase::MonsterDatabase()
{
	//
}

MonsterDatabase::~MonsterDatabase()
{
	//
}

bool MonsterDatabase::load()
{
	std::shared_ptr<sol::state> lua = std::make_shared<sol::state>();

	lua->open_libraries(sol::lib::base);
	lua->open_libraries(sol::lib::package);

	std::shared_ptr<UnitComponent> unit_component = std::make_shared<UnitComponent>();
	std::shared_ptr<MonsterComponent> monster_component = std::make_shared<MonsterComponent>();
	std::shared_ptr<ItemComponent> item_component = std::make_shared<ItemComponent>();

	unit_component->sync_definitions(lua);
	unit_component->sync_data_types(lua);
	unit_component->sync_functions(lua);

	monster_component->sync_definitions(lua);
	monster_component->sync_data_types(lua);

	item_component->sync_definitions(lua);
	item_component->sync_data_types(lua);
	item_component->sync_functions(lua);

	try {
		int total_entries = 0;
		std::string file_path = sZone->config().get_static_db_path().string() + "monster_db.lua";
        sol::load_result fx = lua->load_file(file_path);
		sol::table mob_tbl = fx();
		mob_tbl.for_each([this, &total_entries] (sol::object const &key, sol::object const &value) {
			total_entries += load_internal(key, value);
		});
		HLog(info) << "Loaded " << total_entries << " entries from '" << file_path << "'.";
	} catch(const std::exception &e) {
		HLog(error) << "MonsterDatabase::load: " << e.what();
		return false;
	}

	try {
		int total_entries = 0;
		std::string file_path = sZone->config().get_static_db_path().string() + "monster_skill_db.lua";
		lua->script_file(file_path);
        sol::load_result fx = lua->load_file(file_path);
		sol::table mob_tbl = fx();
		mob_tbl.for_each([this, &total_entries] (sol::object const &key, sol::object const &value) {
			total_entries += load_skill_internal(key, value);
		});
		HLog(info) << "Loaded " << total_entries << " entries from '" << file_path << "'.";
	} catch(const std::exception &e) {
		HLog(error) << "MonsterDatabase::load: " << e.what();
		return false;
	}

	return true;
}

bool MonsterDatabase::load_internal(const sol::object &key, const sol::object &value)
{
	monster_config_data data;

// 	-- ================ Mandatory fields ==============================
	std::string sprite_name = key.as<std::string>();
	std::strncpy(data.sprite_name, sprite_name.c_str(), MAX_UNIT_NAME_LENGTH);

	if (value.get_type() != sol::type::table) {
		HLog(error) << "Value for '" << data.sprite_name << "' is not a table... skipping.";
		return false;
	}

	sol::table m_tbl = value.as<sol::table>();
	
	data.monster_id = m_tbl.get_or<uint16_t>("Id", 0);

	if (data.monster_id <= 0 || data.monster_id > MAX_MOB_DB) {
		HLog(warning) << "Monster ID for '" << data.name << "' is out of range... skipping.";
		return false;
	}

	std::string name = m_tbl.get_or<std::string>("Name", "");
	std::strncpy(data.name, name.c_str(), MAX_UNIT_NAME_LENGTH);

	if (name.size() == 0) {
		HLog(warning) << "Monster Name for '" << data.monster_id << "' is a mandatory field... skipping.";
		return false;
	}

// 	-- ================ Optional fields ==============================
	std::string alt_name = m_tbl.get_or<std::string>("JName", "");
	std::strncpy(data.alt_name, alt_name.c_str(), MAX_UNIT_NAME_LENGTH);

	if (parse_level(m_tbl, data) == false)
		return false;

	if (parse_hp(m_tbl, data) == false)
		return false;

	if (parse_sp(m_tbl, data) == false)
		return false;

	if (parse_reward_base_exp(m_tbl, data) == false)
		return false;

	if (parse_reward_job_exp(m_tbl, data) == false)
		return false;

	if (parse_attack_range(m_tbl, data) == false)
		return false;

	if (parse_attack(m_tbl, data) == false)
		return false;

	if (parse_defense(m_tbl, data) == false)
		return false;

	if (parse_magic_defense(m_tbl, data) == false)
		return false;

	if (parse_view_range(m_tbl, data) == false)
		return false;

	if (parse_chase_range(m_tbl, data) == false)
		return false;

	if (parse_size(m_tbl, data) == false)
		return false;

	if (parse_race(m_tbl, data) == false)
		return false;

	if (parse_element(m_tbl, data) == false)
		return false;

	if (parse_mode(m_tbl, data) == false)
		return false;

	if (parse_move_speed(m_tbl, data) == false)
		return false;

	if (parse_attack_delay(m_tbl, data) == false)
		return false;

	if (parse_attack_motion(m_tbl, data) == false)
		return false;

	if (parse_damage_motion(m_tbl, data) == false)
		return false;

	if (parse_mvp_exp(m_tbl, data) == false)
		return false;

	if (parse_damage_taken_rate(m_tbl, data) == false)
		return false;

	if (parse_stats(m_tbl, data) == false)
		return false;

	if (parse_drops(m_tbl, data, false) == false)
		return false;

	if (parse_drops(m_tbl, data, true) == false)
		return false;

	if (parse_view(m_tbl, data) == false)
		return false;

	_monster_db.insert(data.monster_id, std::make_shared<monster_config_data>(data));

	_monster_str_db.insert(data.sprite_name, std::make_shared<monster_config_data>(data));

	return true;
}

bool MonsterDatabase::parse_level(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Lv");
		if (maybe_val)
			data.level = maybe_val.value();
		else
			data.level = 1;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Lv for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_hp(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Hp");
		if (maybe_val)
			data.hp = maybe_val.value();
		else
			data.hp = 1;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Hp for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_sp(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Sp");
		if (maybe_val)
			data.sp = maybe_val.value();
		else
			data.sp = 1;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Sp for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}
	
	return true;
}

bool MonsterDatabase::parse_reward_base_exp(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Exp");
		if (maybe_val)
			data.rewards.base_exp = maybe_val.value();
		else
			data.rewards.base_exp = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Exp for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_reward_job_exp(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("JExp");
		if (maybe_val)
			data.rewards.job_exp = maybe_val.value();
		else
			data.rewards.job_exp = 1;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing JExp for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_attack_range(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("AttackRange");
		if (maybe_val)
			data.attack_range = maybe_val.value();
		else
			data.attack_range = 1;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AttackRange for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_attack(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_val = table.get<sol::optional<sol::table>>("Attack");
		if (maybe_val) {
			sol::table r_tbl = maybe_val.value();
			if (r_tbl[1].get_type() != sol::type::number || r_tbl[2].get_type() != sol::type::number) {
				HLog(error) << "Attack for monster '" << data.sprite_name << "' is not a numeric type... skipping.";
				return false;
			}
			data.attack_damage[0] = r_tbl[1];
			data.attack_damage[1] = r_tbl[2];
		} else {
			data.attack_damage[0] = 0;
			data.attack_damage[1] = 0;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Attack for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_defense(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Def");
		if (maybe_val)
			data.defense = maybe_val.value();
		else
			data.defense = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Def for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_magic_defense(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("MDef");
		if (maybe_val)
			data.magic_defense = maybe_val.value();
		else
			data.magic_defense = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing MDef for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_view_range(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("ViewRange");
		if (maybe_val)
			data.view_range = maybe_val.value();
		else
			data.view_range = 1;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing ViewRange for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_chase_range(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("ChaseRange");
		if (maybe_val)
			data.chase_range = maybe_val.value();
		else
			data.chase_range = 1;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing ChaseRange for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_size(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Size");
		if (maybe_val)
			data.size = (unit_size_type) maybe_val.value();
		else
			data.size = ESZ_SMALL;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Size for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_race(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("Race");
		if (maybe_val)
			data.primary_race = (monster_primary_race_type) maybe_val.value();
		else
			data.primary_race = MONSTER_RACE_FORMLESS;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Race for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_element(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Element");
		if (maybe_tbl) {
			sol::table r_tbl = maybe_tbl.value();
			if (r_tbl[1].get_type() != sol::type::number || r_tbl[2].get_type() != sol::type::number) {
				HLog(error) << "Element for monster '" << data.sprite_name << "' is not a numeric type... skipping.";
				return false;
			}
			data.element = (element_type) r_tbl[1];
			data.element_level = r_tbl[2];
		} else {
			data.element = ELE_NEUTRAL;
			data.element_level = 1;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Element for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_mode(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Mode");
		if (maybe_tbl) {
			sol::table mode_tbl = maybe_tbl.value();
			for (auto const &m : mode_tbl) {
				sol::object const &v = m.second;
				data.mode |= v.as<int>();
			}
		} else {
			data.mode = 0;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Mode for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_move_speed(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("MoveSpeed");
		if (maybe_val)
			data.move_speed = maybe_val.value();
		else
			data.move_speed = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing MoveSpeed for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_attack_delay(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("AttackDelay");
		if (maybe_val)
			data.attack_delay = maybe_val.value();
		else
			data.attack_delay = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AttackDelay for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_attack_motion(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("AttackMotion");
		if (maybe_val)
			data.attack_motion = maybe_val.value();
		else
			data.attack_motion = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing AttackMotion for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_damage_motion(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("DamageMotion");
		if (maybe_val)
			data.damage_motion = maybe_val.value();
		else
			data.damage_motion = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing DamageMotion for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_mvp_exp(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("MvpExp");
		if (maybe_val)
			data.rewards.mvp_exp = maybe_val.value();
		else
			data.rewards.mvp_exp = 0;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing MvpExp for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_damage_taken_rate(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<int> maybe_val = table.get<sol::optional<int>>("DamageTakenRate");
		if (maybe_val)
			data.damage_taken_rate = maybe_val.value();
		else
			data.damage_taken_rate = 100;
	} catch (sol::error &error) {
		HLog(error) << "Error parsing DamageTakenRate for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_stats(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("Stats");
		if (maybe_tbl) {
			sol::table s_tbl = maybe_tbl.value();

			sol::optional<int> maybe_val = s_tbl.get<sol::optional<int>>("Str");
			if (maybe_val)
				data.stats.str = maybe_val.value();
			else
				data.stats.str = 1;

			maybe_val = s_tbl.get<sol::optional<int>>("Agi");
			if (maybe_val)
				data.stats.agi = maybe_val.value();
			else
				data.stats.agi = 1;

			maybe_val = s_tbl.get<sol::optional<int>>("Vit");
			if (maybe_val)
				data.stats.vit = maybe_val.value();
			else
				data.stats.vit = 1;

			maybe_val = s_tbl.get<sol::optional<int>>("Int");
			if (maybe_val)
				data.stats.int_ = maybe_val.value();
			else
				data.stats.int_ = 1;

			maybe_val = s_tbl.get<sol::optional<int>>("Dex");
			if (maybe_val)
				data.stats.dex = maybe_val.value();
			else
				data.stats.dex = 1;

			maybe_val = s_tbl.get<sol::optional<int>>("Luk");
			if (maybe_val)
				data.stats.luk = maybe_val.value();
			else
				data.stats.luk = 1;
		} else {
			data.stats.str = 1;
			data.stats.agi = 1;
			data.stats.vit = 1;
			data.stats.int_ = 1;
			data.stats.dex = 1;
			data.stats.luk = 1;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Stats for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_drops(sol::table const &table, monster_config_data &data, bool mvp)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>(mvp ? "MvpDrops" : "Drops");
		if (maybe_tbl) {
			sol::table drop_tbl = maybe_tbl.value();
			int i = 0;
			std::vector<monster_config_data::rewards::drops> items;
			for (auto const &d : drop_tbl) {
				sol::object const &k = d.first;
				sol::object const &v = d.second;
				std::shared_ptr<const item_config_data> it = nullptr;
				monster_config_data::rewards::drops drop;

				if (k.is<std::string>()) {
					std::string name = k.as<std::string>();
					it = ItemDB->get_item_by_key_name(name);
					if (it == nullptr) {
						HLog(error) << "Error parsing MvpDrop for monster '" << data.sprite_name << "', item '" << name << "' doesn't exist.";
						continue;
					}
				} else if (k.is<int>()) {
					int item_id = k.as<int>();
					it = ItemDB->get_item_by_id(item_id);
					if (it == nullptr) {
						HLog(error) << "Error parsing MvpDrop for monster '" << data.sprite_name << "', item with ID '" << item_id << "' doesn't exist.";
						continue;
					}
				}

				if (v.get_type() != sol::type::number) {
					HLog(error) << "Error parsing chance for item '" << it->key_name << "', non-numeric value provided.";
					continue;
				}

				if ((mvp && i >= MAX_MVP_DROP) || (!mvp && i >= MAX_MOB_DROP)) {
					HLog(warning) << "Number of "<< (mvp ? "mvp " : "") << "drops for monster '" << data.sprite_name << "', exceeds maximum limit... skipping.";
					continue;
				}

				drop.item_id = it->item_id;
				drop.chance = v.as<int>();

				i++;

				items.push_back(drop);
			}

			i = 0;
			for (auto it : items) {
				if (mvp) {
					data.rewards.items_mvp[i] = items[i];
				} else {
					data.rewards.items[i] = items[i];
				}
				i++;
			}
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing " << (mvp ? "MvpDrops" : "Drops") << " for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::parse_view(sol::table const &table, monster_config_data &data)
{
	try {
		sol::optional<sol::table> maybe_tbl = table.get<sol::optional<sol::table>>("ViewData");
		if (maybe_tbl) {
			sol::table s_tbl = maybe_tbl.value();

			sol::optional<int> maybe_val = s_tbl.get<sol::optional<int>>("SpriteId");
			if (maybe_val)
				data.view.sprite_id = maybe_val.value();
			else
				data.view.sprite_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("WeaponId");
			if (maybe_val)
				data.view.weapon_id = maybe_val.value();
			else
				data.view.weapon_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("ShieldId");
			if (maybe_val)
				data.view.shield_id = maybe_val.value();
			else
				data.view.shield_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("RobeId");
			if (maybe_val)
				data.view.robe_id = maybe_val.value();
			else
				data.view.robe_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("HeadTopId");
			if (maybe_val)
				data.view.headgear_top_id = maybe_val.value();
			else
				data.view.headgear_top_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("HeadMidId");
			if (maybe_val)
				data.view.headgear_middle_id = maybe_val.value();
			else
				data.view.headgear_middle_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("HeadLowId");
			if (maybe_val)
				data.view.headgear_bottom_id = maybe_val.value();
			else
				data.view.headgear_bottom_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("HairStyleId");
			if (maybe_val)
				data.view.hair_style_id = maybe_val.value();
			else
				data.view.hair_style_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("BodyStyleId");
			if (maybe_val)
				data.view.body_style_id = maybe_val.value();
			else
				data.view.body_style_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("HairColorId");
			if (maybe_val)
				data.view.hair_color_id = maybe_val.value();
			else
				data.view.hair_color_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("BodyColorId");
			if (maybe_val)
				data.view.body_color_id = maybe_val.value();
			else
				data.view.body_color_id = 0;

			maybe_val = s_tbl.get<sol::optional<int>>("Gender");
			if (maybe_val)
				data.view.gender = (unit_gender_types) maybe_val.value();
			else
				data.view.gender = UNIT_GENDER_FEMALE;
		} else {
			data.view.sprite_id = 0;
			data.view.weapon_id = 0;
			data.view.shield_id = 0;
			data.view.robe_id = 0;
			data.view.headgear_top_id = 0;
			data.view.headgear_middle_id = 0;
			data.view.headgear_bottom_id = 0;
			data.view.hair_style_id = 0;
			data.view.body_style_id = 0;
			data.view.hair_color_id = 0;
			data.view.body_color_id = 0;
			data.view.gender = UNIT_GENDER_FEMALE;
		}
	} catch (sol::error &error) {
		HLog(error) << "Error parsing Stats for monster '" << data.sprite_name << "' - " << error.what() << ".";
	}

	return true;
}

bool MonsterDatabase::load_skill_internal(const sol::object &key, const sol::object &value)
{
	try {
		std::vector<std::shared_ptr<const monster_skill_config_data>> skills;
		std::shared_ptr<const monster_config_data> monster = nullptr;
		if (key.is<std::string>()) {
			std::string mob_name = key.as<std::string>();
			if ((monster = MonsterDB->get_monster_by_name(mob_name)) == nullptr) {
				HLog(error) << "Error parsing skills for non-existent monster '" << mob_name << "'.";
				return false;
			}
		} else if (key.is<int>()) {
			int mob_id = key.as<int>();
			if ((monster = MonsterDB->get_monster_by_id(mob_id)) == nullptr) {
				HLog(error) << "Error parsing skills for non-existent monster with ID " << mob_id << ".";
				return false;
			}
		}

		if (value.get_type() != sol::type::table) {
				HLog(error) << "MonsterDatabase::load_skill_internal: Error parsing value of unexpected type, value should be a table.";
				return false;
		}

		sol::table skill_tbl = value.as<sol::table>();

		for (auto const &s : skill_tbl) {
			sol::object const &k = s.first;
			sol::object const &v = s.second;
			monster_skill_config_data mskd;

			std::shared_ptr<const skill_config_data> skill = nullptr;
			if (k.is<std::string>()) {
				std::string skill_name = k.as<std::string>();

				if ((skill = SkillDB->get_skill_by_name(skill_name)) == nullptr) {
					HLog(error) << "Error loading non-existent skill '" << skill_name << "' for monster '" << monster->sprite_name << "'.";
					continue;
				}
			} else if (k.is<int>()) {
				int skill_id = k.as<int>();

				if ((skill = SkillDB->get_skill_by_id(skill_id)) == nullptr) {
					HLog(error) << "Error loading non-existent skill with Id " << skill_id << " for monster '" << monster->sprite_name << "'.";
					continue;
				}
			}

			mskd.skill_id = skill->skill_id;

			if (v.get_type() != sol::type::table) {
				HLog(error) << "MonsterDatabase::load_skill_internal: Error loading value for skill '" << skill->name << "' of monster '" << monster->sprite_name << "'.";
				continue;
			}

			sol::table stbl = v.as<sol::table>();

			sol::optional<int> maybe_val = stbl.get<sol::optional<int>>("SkillLevel");
			if (maybe_val)
				mskd.skill_level = maybe_val.value();
			else
				mskd.skill_level = 1;

			maybe_val = stbl.get<sol::optional<int>>("SkillTarget");
			if (maybe_val)
				mskd.target = (monster_skill_target_type) maybe_val.value();
			else
				mskd.target = MONSTER_SKILL_TARGET_CURRENT;

			maybe_val = stbl.get<sol::optional<int>>("SkillState");
			if (maybe_val)
				mskd.state = (monster_skill_state_type) maybe_val.value();
			else
				mskd.state = MONSTER_SKILL_STATE_ANY;

			maybe_val = stbl.get<sol::optional<int>>("Rate");
			if (maybe_val)
				mskd.skill_invoke_rate = maybe_val.value();
			else
				mskd.skill_invoke_rate = 500;

			maybe_val = stbl.get<sol::optional<int>>("CastTime");
			if (maybe_val)
				mskd.cast_time = maybe_val.value();
			else
				mskd.cast_time = 0;

			maybe_val = stbl.get<sol::optional<int>>("Delay");
			if (maybe_val)
				mskd.delay = maybe_val.value();
			else
				mskd.delay = 0;

			sol::optional<bool> maybe_bool = stbl.get<sol::optional<bool>>("Cancellable");
			if (maybe_bool)
				mskd.cancelable = maybe_bool.value();

			maybe_val = stbl.get<sol::optional<int>>("CastCondition");
			if (maybe_val)
				mskd.cast_condition = maybe_val.value();
			else
				mskd.cast_condition = 0;

			maybe_val = stbl.get<sol::optional<int>>("ConditionData");
			if (maybe_val)
				mskd.condition_data = maybe_val.value();
			else
				mskd.condition_data = 0;

			maybe_val = stbl.get<sol::optional<int>>("val0");
			if (maybe_val)
				mskd.val[0] = maybe_val.value();
			else
				mskd.val[0] = 0;

			maybe_val = stbl.get<sol::optional<int>>("val1");
			if (maybe_val)
				mskd.val[1] = maybe_val.value();
			else
				mskd.val[1] = 0;

			maybe_val = stbl.get<sol::optional<int>>("val2");
			if (maybe_val)
				mskd.val[2] = maybe_val.value();
			else
				mskd.val[2] = 0;

			maybe_val = stbl.get<sol::optional<int>>("val3");
			if (maybe_val)
				mskd.val[3] = maybe_val.value();
			else
				mskd.val[3] = 0;

			maybe_val = stbl.get<sol::optional<int>>("val4");
			if (maybe_val)
				mskd.val[4] = maybe_val.value();
			else
				mskd.val[4] = 0;

			maybe_val = stbl.get<sol::optional<int>>("Emotion");
			if (maybe_val)
				mskd.emotion_id = maybe_val.value();
			else
				mskd.emotion_id = 0;

			maybe_val = stbl.get<sol::optional<int>>("ChatMsgID");
			if (maybe_val)
				mskd.msg_id = maybe_val.value();
			else
				mskd.msg_id = 0;

			skills.push_back(std::make_shared<const monster_skill_config_data>(mskd));
		}

		_monster_skill_db.insert(monster->monster_id, std::make_shared<std::vector<std::shared_ptr<const monster_skill_config_data>>>(skills));
		
	}	catch (sol::error &e) { 
		HLog(error) << "MonsterDatabase::load_skill_internal:" << e.what();
	}

	return true;
}
