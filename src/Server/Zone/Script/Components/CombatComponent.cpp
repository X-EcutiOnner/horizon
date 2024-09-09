/***************************************************
 *	   _   _			_						*
 *	  | | | |		  (_)					   *
 *	  | |_| | ___  _ __ _ _______  _ __		  *
 *	  |  _  |/ _ \| '__| |_  / _ \| '_  \		*
 *	  | | | | (_) | |  | |/ / (_) | | | |		*
 *	  \_| |_/\___/|_|  |_/___\___/|_| |_|		*
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

#include "CombatComponent.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Game/Units/Traits/Attribute.hpp"
#include "Server/Zone/Game/Units/Battle/Combat.hpp"
#include "BattleDefinitions.hpp"

using namespace Horizon::Zone;

void CombatComponent::sync_definitions(std::shared_ptr<sol::state> state)
{
	sol::table tbl_1 = state->create_named_table("TargetCheckType");
	tbl_1["Noone"] = (int)BTC_NONE;
	tbl_1["Self"] = (int)BTC_SELF;
	tbl_1["Enemy"] = (int)BTC_ENEMY;
	tbl_1["Party"] = (int)BTC_PARTY;
	tbl_1["AllianceOnly"] = (int)BTC_GUILD_ALLY;
	tbl_1["Neutral"] = (int)BTC_NEUTRAL;
	tbl_1["Guild"] = (int)BTC_GUILD;
	tbl_1["GuildAndAlliance"] = (int)BTC_GUILD_AND_ALLY;
	tbl_1["AllExceptGuild"] = (int)BTC_ALL_EXCEPT_GUILD;
	tbl_1["AllExceptParty"] = (int)BTC_ALL_EXCEPT_PARTY;
	tbl_1["AllExceptEnemy"] = (int)BTC_ALL_EXCEPT_ENEMY;
	tbl_1["All"] = (int)BTC_ALL;


	sol::table tbl_2 = state->create_named_table("AttackType");
	tbl_2["None"] = (int)BAT_NONE;
	tbl_2["Weapon"] = (int)BAT_WEAPON;
	tbl_2["Magic"] = (int)BAT_MAGIC;
	tbl_2["Misc"] = (int)BAT_MISC;

	sol::table tbl_3 = state->create_named_table("DamageBehavior");
	tbl_3["NoDamage"] = (int)DMG_BHVR_NONE;
	tbl_3["NonOffensive"] = (int)DMG_BHVR_NON_OFFENSIVE;
	tbl_3["SplashArea"] = (int)DMG_BHVR_SPLASH;
	tbl_3["SplitBetweenTargets"] = (int)DMG_BHVR_SPLIT_BW_TARGETS;
	tbl_3["IgnoreEquipATK"] = (int)DMG_BHVR_IGNORE_EQUIP_ATK;
	tbl_3["IgnoreDefElement"] = (int)DMG_BHVR_IGNORE_DEF_ELE;
	tbl_3["IgnoreSoftDef"] = (int)DMG_BHVR_IGNORE_SOFT_DEF;
	tbl_3["IgnoreFLEE"] = (int)DMG_BHVR_IGNORE_FLEE;
	tbl_3["IgnoreEquipDef"] = (int)DMG_BHVR_IGNORE_EQUIP_DEF;

	sol::table tbl_4 = state->create_named_table("AttributeOperationType");
	tbl_4["AddToBase"] = (int)CombatRegistry::AttributeOperation::ATTRIBUTE_OPERATION_ADD_TO_BASE;
	tbl_4["SubFromBase"] = (int)CombatRegistry::AttributeOperation::ATTRIBUTE_OPERATION_SUBTRACT_FROM_BASE;
	tbl_4["AddToEquip"] = (int)CombatRegistry::AttributeOperation::ATTRIBUTE_OPERATION_ADD_TO_EQUIP;
	tbl_4["SubFromEquip"] = (int)CombatRegistry::AttributeOperation::ATTRIBUTE_OPERATION_SUBTRACT_FROM_EQUIP;
	tbl_4["AddToStatus"] = (int)CombatRegistry::AttributeOperation::ATTRIBUTE_OPERATION_ADD_TO_STATUS;
	tbl_4["SubtractFromStatus"] = (int)CombatRegistry::AttributeOperation::ATTRIBUTE_OPERATION_SUBTRACT_FROM_STATUS;

	sol::table tbl_5 = state->create_named_table("StatusOperationType");
	tbl_5["Add"] = (int)CombatRegistry::StatusOperation::STATUS_OPERATION_ADD;
	tbl_5["Remove"] = (int)CombatRegistry::StatusOperation::STATUS_OPERATION_REMOVE;

	sol::table tbl_6 = state->create_named_table("SkillExecutionOperationType");
	tbl_6["Cast"] = (int)CombatRegistry::SkillExecutionOperation::SKILL_EXECUTION_OPERATION_CAST;
	tbl_6["Target"] = (int)CombatRegistry::SkillExecutionOperation::SKILL_EXECUTION_OPERATION_TARGET;
	tbl_6["Ground"] = (int)CombatRegistry::SkillExecutionOperation::SKILL_EXECUTION_OPERATION_GROUND;
	
	sol::table tbl_7 = state->create_named_table("SkillResultOperationType");
	tbl_7["Damage"] = (int)CombatRegistry::SkillResultOperation::SKILL_RESULT_OPERATION_DAMAGE;
	tbl_7["Healing"] = (int)CombatRegistry::SkillResultOperation::SKILL_RESULT_OPERATION_HEALING;

	sol::table tbl_8 = state->create_named_table("MeleeExecutionOperationType");
	tbl_8["Target"] = (int)CombatRegistry::MeleeExecutionOperation::MELEE_EXECUTION_OPERATION_TARGET;
	
	sol::table tbl_9 = state->create_named_table("MeleeResultOperationType");
	tbl_9["Damage"] = (int)CombatRegistry::MeleeResultOperation::MELEE_RESULT_OPERATION_DAMAGE;
	tbl_9["Healing"] = (int)CombatRegistry::MeleeResultOperation::MELEE_RESULT_OPERATION_HEALING;

	sol::table tbl_10 = state->create_named_table("CombatDamageHitType");
	tbl_10["Normal"] = (int)combat_damage_hit_type::CBT_DMG_HIT_NORMAL;
	tbl_10["Endure"] = (int)combat_damage_hit_type::CBT_DMG_HIT_ENDURE;
	tbl_10["Splash"] = (int)combat_damage_hit_type::CBT_DMG_HIT_SPLASH;
	tbl_10["Skill"] = (int)combat_damage_hit_type::CBT_DMG_HIT_SKILL;
	tbl_10["MultiHit"] = (int)combat_damage_hit_type::CBT_DMG_HIT_MULTIHIT;
	tbl_10["MultiEndure"] = (int)combat_damage_hit_type::CBT_DMG_HIT_MULTIENDURE;
	tbl_10["Critical"] = (int)combat_damage_hit_type::CBT_DMG_HIT_CRIT;
	tbl_10["PerfectDodge"] = (int)combat_damage_hit_type::CBT_DMG_HIT_PDODGE;
}

void CombatComponent::sync_data_types(std::shared_ptr<sol::state> state)
{
	sol::usertype<CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config> config_1 = state->new_usertype<CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config>("s_combat_status_operation_config");
	config_1["type"] = &CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config::type;
	config_1["total_time"] = &CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config::total_time;
	config_1["val1"] = &CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config::val1;
	config_1["val2"] = &CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config::val2;
	config_1["val3"] = &CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config::val3;
	config_1["val4"] = &CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config::val4;

	sol::usertype<CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config> config_2 = state->new_usertype<CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config>("s_skill_execution_operation_config");
	config_2["skill_id"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::skill_id;
	config_2["skill_lv"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::skill_lv;
	config_2["pos_x"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::pos_x;
	config_2["pos_y"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::pos_y;
	config_2["contents"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::contents;
	config_2["skd"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::skd;
	config_2["skill_execution"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::skill_execution;
	config_2["cast_time"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::cast_time;
	config_2["element"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::element;
	config_2["cast_end_function"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::cast_end_function;
	config_2["skill_cast_data"] = &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config::skill_cast_data;

	sol::usertype<CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config> config_3 = state->new_usertype<CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config>("s_skill_result_operation_config");
	config_3["skill_id"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::skill_id;
	config_3["skill_lv"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::skill_lv;
	config_3["pos_x"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::pos_x;
	config_3["pos_y"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::pos_y;
	config_3["contents"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::contents;
	config_3["skd"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::skd;
	config_3["attack_motion"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::attack_motion;
	config_3["delay_motion"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::delay_motion;
	config_3["action_type"] = &CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config::action_type;

	state->new_usertype<CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::s_melee_execution_operation_config>("s_melee_execution_operation_config",
		"continuous", &CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::s_melee_execution_operation_config::continuous
	);
	
	sol::usertype<combat_damage> config_4 = state->new_usertype<combat_damage>("combat_damage");
	config_4["right_damage"] = &combat_damage::right_damage;
	config_4["left_damage"] = &combat_damage::left_damage;
	config_4["type"] = &combat_damage::type;
	config_4["number_of_hits"] = &combat_damage::number_of_hits;
	config_4["amotion"] = &combat_damage::amotion;
	config_4["dmotion"] = &combat_damage::dmotion;
	config_4["blewcount"] = &combat_damage::blewcount;
	config_4["damage_type"] = &combat_damage::damage_type;
	config_4["ret_type"] = &combat_damage::ret_type;
	config_4["ele_type"] = &combat_damage::ele_type;
	config_4["element_level"] = &combat_damage::element_level;
	config_4["skill_id"] = &combat_damage::skill_id;
	config_4["skill_lv"] = &combat_damage::skill_lv;
	config_4["skill_cast_data"] = &combat_damage::skill_cast_data;
	
	sol::usertype<combat_healing> config_5 = state->new_usertype<combat_healing>("combat_healing");
	config_5["heal_amount"] = &combat_healing::heal_amount;
	config_5["skill_id"] = &combat_healing::skill_id;
	config_5["success"] = &combat_healing::success;
	config_5["skill_lv"] = &combat_healing::skill_lv;
	config_5["skill_cast_data"] = &combat_healing::skill_cast_data;

	state->new_usertype<CombatRegistry::CombatOperationValue>("CombatOperationValue",
		sol::constructors<CombatRegistry::CombatOperationValue()>()
	);

	state->new_usertype<CombatRegistry::CombatValueInteger>("CombatValueInteger",
		sol::constructors<CombatRegistry::CombatValueInteger(int)>(),
		"get_value", &CombatRegistry::CombatValueInteger::get_value
	);

	state->new_usertype<CombatRegistry::CombatValueDamage>("CombatValueDamage",
		sol::constructors<CombatRegistry::CombatValueDamage(combat_damage)>(),
		"get_damage", &CombatRegistry::CombatValueDamage::get_damage
	);

	state->new_usertype<CombatRegistry::CombatValueHealing>("CombatValueHealing",
		sol::constructors<CombatRegistry::CombatValueHealing(combat_healing)>(),
		"get_healing", &CombatRegistry::CombatValueHealing::get_healing
	);

	state->new_usertype<CombatRegistry>("CombatRegistry",
		"create_combat_stage", &CombatRegistry::create_combat_stage,
		"queue_combat_stage", &CombatRegistry::queue_combat_stage
	);

	state->new_usertype<CombatRegistry::AttributeOperation::AttributeOperand>("AttributeOperand",
		sol::constructors<CombatRegistry::AttributeOperation::AttributeOperand(std::shared_ptr<Horizon::Zone::Unit>, std::shared_ptr<Horizon::Zone::Unit>, std::shared_ptr<Horizon::Zone::Traits::Attribute>)>(),
		"get_attribute", &CombatRegistry::AttributeOperation::AttributeOperand::get_attribute
	);

	state->new_usertype<CombatRegistry::AttributeOperation>("AttributeOperation",
		sol::constructors<CombatRegistry::AttributeOperation(
			CombatRegistry::AttributeOperation::AttributeOperand *,
			CombatRegistry::AttributeOperation::attribute_operation_type,
			CombatRegistry::CombatValueInteger *)>(), 
		"get_operation_type", &CombatRegistry::AttributeOperation::get_operation_type,
		"get_operation_sub_type", &CombatRegistry::AttributeOperation::get_operation_sub_type,
		"get_operand", &CombatRegistry::AttributeOperation::get_operand,
		"get_operation_value", &CombatRegistry::AttributeOperation::get_operation_value
	);

	state->new_usertype<CombatRegistry::StatusOperation::StatusOperand>("StatusOperand",
		sol::constructors<CombatRegistry::StatusOperation::StatusOperand(std::shared_ptr<Horizon::Zone::Unit>, std::shared_ptr<Horizon::Zone::Unit>, CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config)>(),
		"get_config", &CombatRegistry::StatusOperation::StatusOperand::get_config
	);

	state->new_usertype<CombatRegistry::StatusOperation>("StatusOperation",
		sol::constructors<
			CombatRegistry::StatusOperation(CombatRegistry::StatusOperation::StatusOperand *, CombatRegistry::StatusOperation::status_operation_type)
		>(),
		"get_operation_type", &CombatRegistry::StatusOperation::get_operation_type,
		"get_operation_sub_type", &CombatRegistry::StatusOperation::get_operation_sub_type,
		"get_operand", &CombatRegistry::StatusOperation::get_operand
	);
	
	state->new_usertype<CombatRegistry::SkillExecutionOperation::SkillExecutionOperand>("SkillExecutionOperand",
		sol::constructors<CombatRegistry::SkillExecutionOperation::SkillExecutionOperand(std::shared_ptr<Horizon::Zone::Unit>, std::shared_ptr<Horizon::Zone::Unit>, CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config)>(),
		"get_config", &CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::get_config
	);

	state->new_usertype<CombatRegistry::SkillExecutionOperation>("SkillExecutionOperation",
		sol::constructors<CombatRegistry::SkillExecutionOperation(CombatRegistry::SkillExecutionOperation::SkillExecutionOperand *, CombatRegistry::SkillExecutionOperation::skill_execution_operation_type)>(), 
		"get_operation_type", &CombatRegistry::SkillExecutionOperation::get_operation_type,
		"get_operation_sub_type", &CombatRegistry::SkillExecutionOperation::get_operation_sub_type,
		"get_operand", &CombatRegistry::SkillExecutionOperation::get_operand,
		"get_operation_value", &CombatRegistry::SkillExecutionOperation::get_operation_value
	);
	
	state->new_usertype<CombatRegistry::SkillResultOperation::SkillResultOperand>("SkillResultOperand",
		sol::constructors<CombatRegistry::SkillResultOperation::SkillResultOperand(std::shared_ptr<Horizon::Zone::Unit>, std::shared_ptr<Horizon::Zone::Unit>, CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config)>(),
		"get_config", &CombatRegistry::SkillResultOperation::SkillResultOperand::get_config
	);

	state->new_usertype<CombatRegistry::SkillResultOperation>("SkillResultOperation",
		sol::constructors<
			CombatRegistry::SkillResultOperation(CombatRegistry::SkillResultOperation::SkillResultOperand *, CombatRegistry::SkillResultOperation::skill_result_operation_type, CombatRegistry::CombatValueDamage *),
			CombatRegistry::SkillResultOperation(CombatRegistry::SkillResultOperation::SkillResultOperand *, CombatRegistry::SkillResultOperation::skill_result_operation_type, CombatRegistry::CombatValueHealing *)
		>(), 
		"get_operation_type", &CombatRegistry::SkillResultOperation::get_operation_type,
		"get_operation_sub_type", &CombatRegistry::SkillResultOperation::get_operation_sub_type,
		"get_operand", &CombatRegistry::SkillResultOperation::get_operand,
		"get_operation_value", &CombatRegistry::SkillResultOperation::get_operation_value
	);
	
	state->new_usertype<CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand>("MeleeExecutionOperand",
		sol::constructors<CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand(std::shared_ptr<Horizon::Zone::Unit>, std::shared_ptr<Horizon::Zone::Unit>, CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::s_melee_execution_operation_config)>(),
		"get_config", &CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::get_config
	);

	state->new_usertype<CombatRegistry::MeleeExecutionOperation>("MeleeExecutionOperation",
		sol::constructors<CombatRegistry::MeleeExecutionOperation(CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand *, CombatRegistry::MeleeExecutionOperation::melee_execution_operation_type)>(), 
		"get_operation_type", &CombatRegistry::MeleeExecutionOperation::get_operation_type,
		"get_operation_sub_type", &CombatRegistry::MeleeExecutionOperation::get_operation_sub_type,
		"get_operand", &CombatRegistry::MeleeExecutionOperation::get_operand,
		"get_operation_value", &CombatRegistry::MeleeExecutionOperation::get_operation_value
	);
	
	state->new_usertype<CombatRegistry::MeleeResultOperation::MeleeResultOperand>("MeleeResultOperand",
		sol::constructors<CombatRegistry::MeleeResultOperation::MeleeResultOperand(std::shared_ptr<Horizon::Zone::Unit>, std::shared_ptr<Horizon::Zone::Unit>)>()
	);

	state->new_usertype<CombatRegistry::MeleeResultOperation>("MeleeResultOperation",
		sol::constructors<
			CombatRegistry::MeleeResultOperation(CombatRegistry::MeleeResultOperation::MeleeResultOperand *, CombatRegistry::MeleeResultOperation::melee_result_operation_type, CombatRegistry::CombatValueDamage *),
			CombatRegistry::MeleeResultOperation(CombatRegistry::MeleeResultOperation::MeleeResultOperand *, CombatRegistry::MeleeResultOperation::melee_result_operation_type, CombatRegistry::CombatValueHealing *)
		>(), 
		"get_operation_type", &CombatRegistry::MeleeResultOperation::get_operation_type,
		"get_operation_sub_type", &CombatRegistry::MeleeResultOperation::get_operation_sub_type,
		"get_operand", &CombatRegistry::MeleeResultOperation::get_operand,
		"get_operation_value", &CombatRegistry::MeleeResultOperation::get_operation_value
	);

	sol::usertype<CombatRegistry::CombatStage> registry = state->new_usertype<CombatRegistry::CombatStage>("CombatStage",
		sol::constructors<CombatRegistry::CombatStage(int)>());
	registry["get_priority"] = &CombatRegistry::CombatStage::get_priority;
	registry["push_attribute_operation"] = &CombatRegistry::CombatStage::push_attribute_operation;
	registry["push_status_add_operation"] = &CombatRegistry::CombatStage::push_status_add_operation;
	registry["push_status_remove_operation"] = &CombatRegistry::CombatStage::push_status_remove_operation;
	registry["push_skill_execution_operation"] = &CombatRegistry::CombatStage::push_skill_execution_operation;
	registry["push_skill_result_damage_operation"] = &CombatRegistry::CombatStage::push_skill_result_damage_operation;
	registry["push_skill_result_healing_operation"] = &CombatRegistry::CombatStage::push_skill_result_healing_operation;
	registry["push_melee_execution_operation"] = &CombatRegistry::CombatStage::push_melee_execution_operation;
	registry["push_melee_result_damage_operation"] = &CombatRegistry::CombatStage::push_melee_result_damage_operation;
	registry["push_melee_result_healing_operation"] = &CombatRegistry::CombatStage::push_melee_result_healing_operation;
}

void CombatComponent::sync_functions(std::shared_ptr<sol::state> state)
{
}