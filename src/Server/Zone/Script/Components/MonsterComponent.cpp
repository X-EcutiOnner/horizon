/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
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

#include "MonsterComponent.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Game/Units/Mob/Hostile/Monster.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/StaticDB/MonsterDB.hpp"
#include "Server/Zone/ZoneSystem.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;
using namespace Horizon::Zone::Units;

void MonsterComponent::sync_definitions(std::shared_ptr<sol::state> state)
{
	sol::table tbl_1 = state->create_named_table("MonsterRace");
	tbl_1["Formless"] = (int) MONSTER_RACE_FORMLESS; 
	tbl_1["Undead"] = (int) MONSTER_RACE_UNDEAD; 
	tbl_1["Brute"] = (int) MONSTER_RACE_BRUTE; 
	tbl_1["Plant"] = (int) MONSTER_RACE_PLANT; 
	tbl_1["Insect"] = (int) MONSTER_RACE_INSECT; 
	tbl_1["Fish"] = (int) MONSTER_RACE_FISH; 
	tbl_1["Demon"] = (int) MONSTER_RACE_DEMON; 
	tbl_1["DemiHuman"] = (int) MONSTER_RACE_DEMIHUMAN; 
	tbl_1["Angel"] = (int) MONSTER_RACE_ANGEL; 
	tbl_1["Dragon"] = (int) MONSTER_RACE_DRAGON; 
	tbl_1["Player"] = (int) MONSTER_RACE_PLAYER; 
	tbl_1["Boss"] = (int) MONSTER_RACE_BOSS; 
	tbl_1["NonBoss"] = (int) MONSTER_RACE_NONBOSS;

	sol::table tbl_2 = state->create_named_table("MonsterMode");
	tbl_2["None"] = (int) MONSTER_MODE_MASK_NONE; 
	tbl_2["CanMove"] = (int) MONSTER_MODE_MASK_CANMOVE; 
	tbl_2["Looter"] = (int) MONSTER_MODE_MASK_LOOTER; 
	tbl_2["Aggressive"] = (int) MONSTER_MODE_MASK_AGGRESSIVE; 
	tbl_2["Assist"] = (int) MONSTER_MODE_MASK_ASSIST; 
	tbl_2["CastSensorIdle"] = (int) MONSTER_MODE_MASK_CASTSENSOR_IDLE; 
	tbl_2["Boss"] = (int) MONSTER_MODE_MASK_BOSS; 
	tbl_2["Plant"] = (int) MONSTER_MODE_MASK_PLANT; 
	tbl_2["CanAttack"] = (int) MONSTER_MODE_MASK_CANATTACK; 
	tbl_2["Detector"] = (int) MONSTER_MODE_MASK_DETECTOR; 
	tbl_2["CastSensorChase"] = (int) MONSTER_MODE_MASK_CASTSENSOR_CHASE; 
	tbl_2["ChangeChase"] = (int) MONSTER_MODE_MASK_CHANGECHASE; 
	tbl_2["Angry"] = (int) MONSTER_MODE_MASK_ANGRY; 
	tbl_2["ChangeTargetMelee"] = (int) MONSTER_MODE_MASK_CHANGETARGET_MELEE; 
	tbl_2["ChangeTargetChase"] = (int) MONSTER_MODE_MASK_CHANGETARGET_CHASE; 
	tbl_2["TargetWeak"] = (int) MONSTER_MODE_MASK_TARGETWEAK; 
	tbl_2["NoKnockBack"] = (int) MONSTER_MODE_MASK_NOKNOCKBACK;

	sol::table tbl_3 = state->create_named_table("MonsterSkillState");
	tbl_3["Any"] = (int) MONSTER_SKILL_STATE_ANY; 
	tbl_3["Idle"] = (int) MONSTER_SKILL_STATE_IDLE; 
	tbl_3["Walk"] = (int) MONSTER_SKILL_STATE_WALK; 
	tbl_3["Loot"] = (int) MONSTER_SKILL_STATE_LOOT; 
	tbl_3["Dead"] = (int) MONSTER_SKILL_STATE_DEAD; 
	tbl_3["Berserk"] = (int) MONSTER_SKILL_STATE_BERSERK;  //Aggressive mob attacking
	tbl_3["Angry"] = (int) MONSTER_SKILL_STATE_ANGRY;    //Mob retaliating from being attacked.
	tbl_3["Rush"] = (int) MONSTER_SKILL_STATE_RUSH;     //Mob following a player after being attacked.
	tbl_3["Follow"] = (int) MONSTER_SKILL_STATE_FOLLOW;   //Mob following a player without being attacked.
	tbl_3["AnyTarget"] = (int) MONSTER_SKILL_STATE_ANYTARGET;

	sol::table tbl_4 = state->create_named_table("MonsterSkillTarget");
	tbl_4["CurrentTarget"] = (int) MONSTER_SKILL_TARGET_CURRENT; 
	tbl_4["Random"] = (int) MONSTER_SKILL_TARGET_RANDOM; 
	tbl_4["Self"] = (int) MONSTER_SKILL_TARGET_SELF; 
	tbl_4["Friend"] = (int) MONSTER_SKILL_TARGET_FRIEND; 
	tbl_4["Master"] = (int) MONSTER_SKILL_TARGET_MASTER; 
	tbl_4["Around5"] = (int) MONSTER_SKILL_TARGET_AROUND5; 
	tbl_4["Around6"] = (int) MONSTER_SKILL_TARGET_AROUND6; 
	tbl_4["Around7"] = (int) MONSTER_SKILL_TARGET_AROUND7; 
	tbl_4["Around8"] = (int) MONSTER_SKILL_TARGET_AROUND8; 
	tbl_4["Around1"] = (int) MONSTER_SKILL_TARGET_AROUND1; 
	tbl_4["Around2"] = (int) MONSTER_SKILL_TARGET_AROUND2; 
	tbl_4["Around3"] = (int) MONSTER_SKILL_TARGET_AROUND3; 
	tbl_4["Around4"] = (int) MONSTER_SKILL_TARGET_AROUND4; 
	tbl_4["Around"] = (int) MONSTER_SKILL_TARGET_AROUND;

	sol::table tbl_5 = state->create_named_table("MonsterSkillCastCondition");
	tbl_5["Always"] = (int) MONSTER_SKILL_CC_ALWAYS; 
	tbl_5["MyHpLtMaxRate"] = (int) MONSTER_SKILL_CC_MYHPLTMAXRATE; 
	tbl_5["MyHpInRate"] = (int) MONSTER_SKILL_CC_MYHPINRATE; 
	tbl_5["FriendHpLtMaxRate"] = (int) MONSTER_SKILL_CC_FRIENDHPLTMAXRATE; 
	tbl_5["FriendHpInRate"] = (int) MONSTER_SKILL_CC_FRIENDHPINRATE; 
	tbl_5["MyStatusOn"] = (int) MONSTER_SKILL_CC_MYSTATUSON; 
	tbl_5["MyStatusOff"] = (int) MONSTER_SKILL_CC_MYSTATUSOFF; 
	tbl_5["FriendStatusOn"] = (int) MONSTER_SKILL_CC_FRIENDSTATUSON; 
	tbl_5["FriendStatusOff"] = (int) MONSTER_SKILL_CC_FRIENDSTATUSOFF; 
	tbl_5["AttackPCGT"] = (int) MONSTER_SKILL_CC_ATTACKPCGT; 
	tbl_5["AttackPCGE"] = (int) MONSTER_SKILL_CC_ATTACKPCGE; 
	tbl_5["SlaveLT"] = (int) MONSTER_SKILL_CC_SLAVELT; 
	tbl_5["SlaveLE"] = (int) MONSTER_SKILL_CC_SLAVELE; 
	tbl_5["CloseRangeAttacked"] = (int) MONSTER_SKILL_CC_CLOSERANGEATTACKED; 
	tbl_5["LongRangeAttacked"] = (int) MONSTER_SKILL_CC_LONGRANGEATTACKED; 
	tbl_5["AfterSkill"] = (int) MONSTER_SKILL_CC_AFTERSKILL; 
	tbl_5["SkillUsed"] = (int) MONSTER_SKILL_CC_SKILLUSED; 
	tbl_5["CastTargeted"] = (int) MONSTER_SKILL_CC_CASTTARGETED; 
	tbl_5["RudeAttacked"] = (int) MONSTER_SKILL_CC_RUDEATTACKED; 
	tbl_5["MasterHPLTMaxRate"] = (int) MONSTER_SKILL_CC_MASTERHPLTMAXRATE; 
	tbl_5["MasterAttacked"] = (int) MONSTER_SKILL_CC_MASTERATTACKED; 
	tbl_5["Alchemist"] = (int) MONSTER_SKILL_CC_ALCHEMIST; 
	tbl_5["Spawn"] = (int) MONSTER_SKILL_CC_SPAWN; 
	tbl_5["MagicAttacked"] = (int) MONSTER_SKILL_CC_MAGICATTACKED;
}

void MonsterComponent::sync_data_types(std::shared_ptr<sol::state> state)
{
	sol::usertype<Monster> config_1 = state->new_usertype<Monster>("Monster");
	config_1["unit"] = [](std::shared_ptr<Horizon::Zone::Units::Monster> monster) { return monster->shared_from_this(); };
	config_1["stop_movement"] = &Monster::stop_movement;
	config_1["on_pathfinding_failure"] = &Monster::on_pathfinding_failure;
	config_1["on_movement_begin"] = &Monster::on_movement_begin;
	config_1["on_movement_step"] = &Monster::on_movement_step;
	config_1["on_movement_end"] = &Monster::on_movement_end;
	config_1["set_spotted"] = &Monster::set_spotted,
	config_1["was_spotted_once"] = &Monster::was_spotted_once;
	config_1["on_status_effect_start"] = &Monster::on_status_effect_start;
	config_1["on_status_effect_end"] = &Monster::on_status_effect_end;
	config_1["on_status_effect_change"] = &Monster::on_status_effect_change;
	config_1["set_next_walk_time"] = &Monster::set_next_walk_time;
	config_1["next_walk_time"] = &Monster::next_walk_time;
	config_1["on_damage_received"] = &Monster::on_damage_received;
}

void MonsterComponent::sync_functions(std::shared_ptr<sol::state> state)
{
	state->set_function("cast_unit_to_monster",
					[] (std::shared_ptr<Unit> e)
					{
						return e->template downcast<Monster>();
					});

	// Monster Spawn Script Function
	state->set_function("Monster",
		[this] (std::string const &map_name, uint16_t x, uint16_t y, uint16_t x_area, uint16_t y_area, std::string const &name, uint16_t monster_id, uint16_t amount, uint16_t spawn_delay_base, uint16_t spawn_delay_variance) 
		{	
			std::shared_ptr<Map> map;

			int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, map_name, nullptr);

			if (segment_number == 0)
				return;

			std::shared_ptr<Horizon::Zone::GameLogicProcess> container = sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number);

			monster_spawn_data spwd;

			spwd.spawn_dataset_id = container->get_monster_spawn_agent()._last_monster_spawn_id++;
			spwd.map_name = map_name;
			spwd.x = x;
			spwd.y = y;
			spwd.x_area = x_area;
			spwd.y_area = y_area;
			spwd.mob_name = name;
			spwd.monster_id = monster_id;
			spwd.amount = amount;
			spwd.spawn_delay_base = spawn_delay_base;
			spwd.spawn_delay_variance = spawn_delay_variance;

			std::shared_ptr<Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN> scenario = std::make_shared<Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN>(container);
			std::shared_ptr<Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN::RegisterMonsterSpawn> work = std::make_shared<Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN::RegisterMonsterSpawn>(scenario);
			scenario->get_runtime_synchronization_mutex().lock();
			Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN::s_register_monster_spawn_request request;
			request.data = spwd;
			work->set_request(request);
			scenario->get_runtime_synchronization_mutex().unlock();
			scenario->push(std::move(work));
			container->system_routine_queue_push(std::move(scenario));
			HLog(debug) << "Monster spawn set (" << name << ") in " << map_name << " at (" << x << "," << y << ")[" << x_area << "," << y_area << "]" " for a total of " << amount << " monsters is initializing...";

		});
}