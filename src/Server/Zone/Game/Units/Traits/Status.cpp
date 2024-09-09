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

#include "Status.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Definitions/MonsterDefinitions.hpp"
#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Game/StaticDB/JobDB.hpp"
#include "Server/Zone/Game/StaticDB/ExpDB.hpp"
#include "Server/Zone/Game/Units/Traits/Appearance.hpp"
#include "Server/Zone/Game/Units/Traits/ObservableStatus.hpp"
#include "Server/Zone/Game/Units/Mob/Mob.hpp"
#include "Server/Zone/Game/Units/NPC/NPC.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Unit.hpp"
#include "Server/Zone/Packets/TransmittedPackets.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone::Traits;


void Status::StatusRegistry::add_to_base(Attribute *attribute, uint16_t value) 
{ 
	StatusOperation *operation = new StatusOperation(attribute, STATUS_OPERATION_ADD_TO_BASE, value);
	_status_operation_queue.push(operation); 
}
void Status::StatusRegistry::subtract_from_base(Attribute *attribute, uint16_t value) 
{ 
	StatusOperation *operation = new StatusOperation(attribute, STATUS_OPERATION_SUBTRACT_FROM_BASE, value);
	_status_operation_queue.push(operation); 
}
void Status::StatusRegistry::add_to_equip(Attribute *attribute, uint16_t value) 
{ 
	StatusOperation *operation = new StatusOperation(attribute, STATUS_OPERATION_ADD_TO_EQUIP, value);
	_status_operation_queue.push(operation); 
}
void Status::StatusRegistry::subtract_from_equip(Attribute *attribute, uint16_t value) 
{ 
	StatusOperation *operation = new StatusOperation(attribute, STATUS_OPERATION_SUBTRACT_FROM_EQUIP, value);
	_status_operation_queue.push(operation); 
}
void Status::StatusRegistry::add_to_status(Attribute *attribute, uint16_t value) 
{ 
	StatusOperation *operation = new StatusOperation(attribute, STATUS_OPERATION_ADD_TO_STATUS, value);
	_status_operation_queue.push(operation); 
}
void Status::StatusRegistry::subtract_from_status(Attribute *attribute, uint16_t value) 
{ 
	StatusOperation *operation = new StatusOperation(attribute, STATUS_OPERATION_SUBTRACT_FROM_STATUS, value);
	_status_operation_queue.push(operation); 
}

void Status::StatusRegistry::StatusOperation::execute()
{
	switch (get_type()) {
	case STATUS_OPERATION_ADD_TO_BASE:
		_attribute->set_base(_attribute->get_base() + _value);
		break;
	case STATUS_OPERATION_SUBTRACT_FROM_BASE:
		_attribute->set_base(_attribute->get_base() - _value);
		break;
	case STATUS_OPERATION_ADD_TO_EQUIP:
		_attribute->set_equip(_attribute->get_equip() + _value);
		break;
	case STATUS_OPERATION_SUBTRACT_FROM_EQUIP:
		_attribute->set_equip(_attribute->get_equip() - _value);
		break;
	case STATUS_OPERATION_ADD_TO_STATUS:
		_attribute->set_status(_attribute->get_status() + _value);
		break;
	case STATUS_OPERATION_SUBTRACT_FROM_STATUS:
		_attribute->set_status(_attribute->get_status() - _value);
		break;
	}
	
	// notify through walk packet of hp change.
    if (_attribute != nullptr && _attribute->get_type() == STATUS_CURRENTHP && _attribute->unit()->type() == UNIT_MONSTER && !_attribute->unit()->is_dead()) {
        _attribute->unit()->notify_nearby_players_of_movement(true);
    }
}

void Status::StatusRegistry::process_queue()
{
	while (has_next_operation()) {
		StatusOperation *operation = get_next_operation();
		_status_operation_queue.pop();
		operation->execute();
		delete operation;
	}
}

Status::Status(std::weak_ptr<Horizon::Zone::Unit> unit, unit_type type)
	: _unit(unit), _type(type)
{
	_status_registry = std::make_shared<StatusRegistry>();
}

Status::~Status()
{
}

bool Status::initialize(std::shared_ptr<Horizon::Zone::Units::Player> player)
{
	load(player);

	std::shared_ptr<const job_config_data> job = player->job();

	if (job == nullptr) {
		HLog(error) << "Status::initialize_compound_attributes: Job was null.";
		return false;
	}

	set_attack_range(std::make_shared<AttackRange>(_unit));
	attack_range()->compute();

	set_max_weight(std::make_shared<MaxWeight>(_unit, job->max_weight));
	max_weight()->set_strength(strength().get());
	max_weight()->compute();

	// Calculated when inventory is synced.
	set_current_weight(std::make_shared<CurrentWeight>(_unit, 0));

	set_status_atk(std::make_shared<StatusATK>(_unit));
	status_atk()->set_base_level(base_level().get());
	status_atk()->set_strength(strength().get());
	status_atk()->set_dexterity(dexterity().get());
	status_atk()->set_luck(luck().get());
	status_atk()->compute();

	set_equip_atk(std::make_shared<EquipATK>(_unit));
	equip_atk()->set_strength(strength().get());
	equip_atk()->set_dexterity(dexterity().get());
	equip_atk()->compute();

	set_status_matk(std::make_shared<StatusMATK>(_unit));
	status_matk()->set_base_level(base_level().get());
	status_matk()->set_intelligence(intelligence().get());
	status_matk()->set_dexterity(dexterity().get());
	status_matk()->set_luck(luck().get());
	status_matk()->compute();

	set_soft_def(std::make_shared<SoftDEF>(_unit));
	soft_def()->set_vitality(vitality().get());
	soft_def()->compute();

	set_soft_mdef(std::make_shared<SoftMDEF>(_unit));
	soft_mdef()->set_base_level(base_level().get());
	soft_mdef()->set_intelligence(intelligence().get());
	soft_mdef()->set_dexterity(dexterity().get());
	soft_mdef()->set_vitality(vitality().get());
	soft_mdef()->compute();

	set_hit(std::make_shared<HIT>(_unit));
	hit()->set_base_level(base_level().get());
	hit()->set_dexterity(dexterity().get());
	hit()->set_luck(luck().get());
	hit()->compute();

	set_crit(std::make_shared<CRIT>(_unit));
	crit()->set_luck(luck().get());
	crit()->compute();

	set_flee(std::make_shared<FLEE>(_unit));
	flee()->set_base_level(base_level().get());
	flee()->set_agility(agility().get());
	flee()->set_luck(luck().get());
	flee()->compute();

	set_attack_speed(std::make_shared<AttackSpeed>(_unit));
	attack_speed()->set_base_level(base_level().get());
	attack_speed()->set_agility(agility().get());
	attack_speed()->set_dexterity(dexterity().get());
	attack_speed()->compute();

	// Register Status Observables
	strength()->register_observable(strength().get());
	agility()->register_observable(agility().get());
	vitality()->register_observable(vitality().get());
	intelligence()->register_observable(intelligence().get());
	dexterity()->register_observable(dexterity().get());
	luck()->register_observable(luck().get());
	base_experience()->register_observable(base_experience().get());
	job_experience()->register_observable(job_experience().get());
	base_level()->register_observable(base_level().get());
	job_level()->register_observable(job_level().get());

	// Register Status Observers
	strength()->register_observers(
		strength_cost().get(),
		max_weight().get(),
		status_atk().get(),
		equip_atk().get());

	agility()->register_observers(
		agility_cost().get(),
		flee().get(),
		attack_speed().get());

	vitality()->register_observers(
		vitality_cost().get(),
		soft_def().get(),
		soft_mdef().get());

	intelligence()->register_observers(
		intelligence_cost().get(),
		status_matk().get(),
		soft_mdef().get());

	dexterity()->register_observers(
		dexterity_cost().get(),
		status_atk().get(),
		equip_atk().get(),
		status_matk().get(),
		soft_mdef().get(),
		hit().get(),
		attack_speed().get());

	luck()->register_observers(
		luck_cost().get(),
		status_atk().get(),
		status_matk().get(),
		hit().get(),
		crit().get(),
		flee().get());

	base_level()->register_observers(
		status_point().get(),
		next_base_experience().get(),
		status_atk().get(),
		status_matk().get(),
		soft_mdef().get(),
		hit().get(),
		flee().get(),
		attack_speed().get());

	job_level()->register_observers(
		skill_point().get(),
		next_job_experience().get());

	base_experience()->register_observers(base_level().get());

	job_experience()->register_observers(job_level().get());
	
	/* Combat Status Attributes */
	set_attack_motion(std::make_shared<AttackMotion>(_unit));
	attack_motion()->set_attack_speed(attack_speed().get());
	attack_motion()->set_agility(agility().get());
	attack_motion()->register_observable(attack_motion().get());
	attack_motion()->register_observers(attack_speed().get(), agility().get());
	attack_motion()->compute();

	set_attack_delay(std::make_shared<AttackDelay>(_unit));
	attack_delay()->set_attack_motion(attack_motion().get());
	attack_delay()->register_observable(attack_delay().get());
	attack_delay()->register_observers(attack_motion().get());
	attack_delay()->compute();

	set_damage_motion(std::make_shared<DamageMotion>(_unit));
	damage_motion()->set_agility(agility().get());
	damage_motion()->register_observable(damage_motion().get());
	damage_motion()->register_observers(agility().get());
	damage_motion()->compute();

	set_base_attack(std::make_shared<BaseAttack>(_unit));
	base_attack()->set_strength(strength().get());
	base_attack()->set_dexterity(dexterity().get());
	base_attack()->set_luck(luck().get());
	base_attack()->set_base_level(base_level().get());
	base_attack()->register_observable(base_attack().get());
	base_attack()->register_observers(strength().get(), dexterity().get(), luck().get(), base_level().get());

	player->get_session()->clif()->notify_initial_status();

	set_initialized(true);
	return true;
}

bool Status::initialize(std::shared_ptr<Horizon::Zone::Units::NPC> npc)
{
	set_movement_speed(std::make_shared<MovementSpeed>(_unit, 100));
	set_base_level(std::make_shared<BaseLevel>(_unit, 1));

	set_current_hp(std::make_shared<CurrentHP>(_unit, 1));
	set_current_sp(std::make_shared<CurrentSP>(_unit, 1));
	set_max_hp(std::make_shared<MaxHP>(_unit, 1));
	set_max_sp(std::make_shared<MaxSP>(_unit, 1));
	set_hair_style(std::make_shared<HairStyle>(_unit, 0));
	set_hair_color(std::make_shared<HairColor>(_unit, 0));
	set_robe_sprite(std::make_shared<RobeSprite>(_unit, 0));
	set_base_appearance(std::make_shared<BaseAppearance>(_unit, npc->job_id()));

	set_initialized(true);
	return true;
}

bool Status::initialize(std::shared_ptr<Horizon::Zone::Units::Mob> creature, std::shared_ptr<const monster_config_data> md)
{
	set_movement_speed(std::make_shared<MovementSpeed>(_unit, md->move_speed));

	set_base_level(std::make_shared<BaseLevel>(_unit, md->level));

	set_current_hp(std::make_shared<CurrentHP>(_unit, md->hp));
	set_current_sp(std::make_shared<CurrentSP>(_unit, md->sp));
	set_max_hp(std::make_shared<MaxHP>(_unit, md->hp));
	set_max_sp(std::make_shared<MaxSP>(_unit, md->sp));
	set_size(std::make_shared<UnitSize>(_unit, (int) md->size));
	set_base_appearance(std::make_shared<BaseAppearance>(_unit, md->monster_id));
	set_hair_color(std::make_shared<HairColor>(_unit, md->view.hair_color_id));
	set_cloth_color(std::make_shared<ClothColor>(_unit, md->view.body_color_id));
	set_head_top_sprite(std::make_shared<HeadTopSprite>(_unit, md->view.headgear_top_id));
	set_head_mid_sprite(std::make_shared<HeadMidSprite>(_unit, md->view.headgear_middle_id));
	set_head_bottom_sprite(std::make_shared<HeadBottomSprite>(_unit, md->view.headgear_bottom_id));
	set_hair_style(std::make_shared<HairStyle>(_unit, md->view.hair_style_id));
	set_shield_sprite(std::make_shared<ShieldSprite>(_unit, md->view.shield_id));
	set_weapon_sprite(std::make_shared<WeaponSprite>(_unit, md->view.weapon_id));
	set_robe_sprite(std::make_shared<RobeSprite>(_unit, md->view.robe_id));
	set_body_style(std::make_shared<BodyStyle>(_unit, md->view.body_style_id));

	set_strength(std::make_shared<Strength>(_unit, md->stats.str));
	set_agility(std::make_shared<Agility>(_unit, md->stats.agi, 0, 0));
	set_vitality(std::make_shared<Vitality>(_unit, md->stats.vit, 0, 0));
	set_intelligence(std::make_shared<Intelligence>(_unit, md->stats.int_, 0, 0));
	set_dexterity(std::make_shared<Dexterity>(_unit, md->stats.dex, 0, 0));
	set_luck(std::make_shared<Luck>(_unit, md->stats.luk, 0, 0));

	set_soft_def(std::make_shared<SoftDEF>(_unit));
	soft_def()->set_vitality(vitality().get());
	soft_def()->compute();

	set_soft_mdef(std::make_shared<SoftMDEF>(_unit));
	soft_mdef()->set_base_level(base_level().get());
	soft_mdef()->set_intelligence(intelligence().get());
	soft_mdef()->set_dexterity(dexterity().get());
	soft_mdef()->set_vitality(vitality().get());
	soft_mdef()->compute();

	set_hit(std::make_shared<HIT>(_unit));
	hit()->set_base_level(base_level().get());
	hit()->set_dexterity(dexterity().get());
	hit()->set_luck(luck().get());
	hit()->compute();

	set_crit(std::make_shared<CRIT>(_unit));
	crit()->set_luck(luck().get());
	crit()->compute();

	set_flee(std::make_shared<FLEE>(_unit));
	flee()->set_base_level(base_level().get());
	flee()->set_agility(agility().get());
	flee()->set_luck(luck().get());
	flee()->compute();

	set_attack_range(std::make_shared<AttackRange>(_unit));
	attack_range()->compute();

	set_attack_motion(std::make_shared<AttackMotion>(_unit));
	attack_motion()->compute();

	set_attack_delay(std::make_shared<AttackDelay>(_unit));
	attack_delay()->compute();

	set_damage_motion(std::make_shared<DamageMotion>(_unit));
	damage_motion()->compute();

	set_attack_range(std::make_shared<AttackRange>(_unit));
	attack_range()->compute();

	set_creature_weapon_attack(std::make_shared<MobWeaponAttack>(_unit, md->attack_damage[0]));
	set_creature_weapon_attack_magic(std::make_shared<MobWeaponAttack>(_unit, md->attack_damage[1]));
	
	set_creature_attack_damage(std::make_shared<MobAttackDamage>(_unit));
	creature_attack_damage()->set_strength(strength().get());
	creature_attack_damage()->set_base_level(base_level().get());
	creature_attack_damage()->set_creature_weapon_attack(creature_weapon_attack_magic().get());
	creature_attack_damage()->compute();
	creature_attack_damage()->register_observable(creature_attack_damage().get());
	creature_attack_damage()->register_observers(strength().get(), base_level().get(), creature_weapon_attack().get());

	set_creature_magic_attack_damage(std::make_shared<MobMagicAttackDamage>(_unit));
	creature_magic_attack_damage()->set_intelligence(intelligence().get());
	creature_magic_attack_damage()->set_base_level(base_level().get());
	creature_magic_attack_damage()->set_creature_weapon_attack(creature_weapon_attack_magic().get());
	creature_magic_attack_damage()->compute();
	creature_magic_attack_damage()->register_observable(creature_magic_attack_damage().get());
	creature_magic_attack_damage()->register_observers(intelligence().get(), base_level().get(), creature_weapon_attack().get());

	set_creature_view_range(std::make_shared<MobViewRange>(_unit, (int) md->view_range));
	set_creature_chase_range(std::make_shared<MobChaseRange>(_unit, (int) md->chase_range));
	set_creature_primary_race(std::make_shared<MobPrimaryRace>(_unit, (int) md->primary_race));
	set_creature_secondary_race(std::make_shared<MobSecondaryRace>(_unit, (int) md->secondary_race));
	set_creature_element(std::make_shared<MobElement>(_unit, (int) md->element));
	set_creature_element_level(std::make_shared<MobElementLevel>(_unit, (int) md->element_level));
	set_creature_mode(std::make_shared<MobMode>(_unit, (int) md->mode));

	set_initialized(true);
	return true;
}

bool Status::load(std::shared_ptr<Horizon::Zone::Units::Player> pl)
{
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `job_id`, `strength`, `agility`, `vitality`, `intelligence`, `dexterity`, "
			"`luck`, `status_points`, `skill_points`, `hp`, `sp`, `maximum_hp`, `maximum_sp`, `base_level`, `job_level`, `base_experience`, `job_experience`, "
			"`hair_color_id`, `cloth_color_id`, `head_top_view_id`, `head_mid_view_id`, `head_bottom_view_id`, `hair_style_id`, `shield_view_id`, `weapon_view_id`, `robe_view_id`, "
			"`body_id`, `zeny`, `virtue`, `honor`, `manner` FROM `character_status` WHERE `id` = ?");
		auto b1 = stmt.bind(pl->character()._character_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (results.rows().empty()) {
			HLog(error) << "Error loading status, character with ID " << pl->character()._character_id << " does not exist.";
			return false;
		}

		auto r = results.rows()[0];

		int32_t job_id = r[0].get_uint64();

		std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(job_id);
		std::shared_ptr<const exp_group_data> bexpg = ExpDB->get_exp_group(job->base_exp_group, EXP_GROUP_TYPE_BASE);
		std::shared_ptr<const exp_group_data> jexpg = ExpDB->get_exp_group(job->job_exp_group, EXP_GROUP_TYPE_JOB);

		pl->set_job_id(job_id);
		pl->set_job(job);

		int32_t str = r[1].as_uint64();
		int32_t agi = r[2].as_uint64();
		int32_t vit = r[3].as_uint64();
		int32_t _int = r[4].as_uint64();
		int32_t dex = r[5].as_uint64();
		int32_t luk = r[6].as_uint64();

		/**
		 * Main Attributes.
		 */
		set_strength(std::make_shared<Strength>(_unit, str));
		set_agility(std::make_shared<Agility>(_unit, agi, 0, 0));
		set_vitality(std::make_shared<Vitality>(_unit, vit, 0, 0));
		set_intelligence(std::make_shared<Intelligence>(_unit, _int, 0, 0));
		set_dexterity(std::make_shared<Dexterity>(_unit, dex, 0, 0));
		set_luck(std::make_shared<Luck>(_unit, luk, 0, 0));

		set_size(std::make_shared<UnitSize>(_unit, (int)ESZ_MEDIUM));

		set_strength_cost(std::make_shared<StrengthPointCost>(_unit, get_required_statpoints(str, str + 1)));
		set_agility_cost(std::make_shared<AgilityPointCost>(_unit, get_required_statpoints(agi, agi + 1)));
		set_vitality_cost(std::make_shared<VitalityPointCost>(_unit, get_required_statpoints(vit, vit + 1)));
		set_intelligence_cost(std::make_shared<IntelligencePointCost>(_unit, get_required_statpoints(_int, _int + 1)));
		set_dexterity_cost(std::make_shared<DexterityPointCost>(_unit, get_required_statpoints(dex, dex + 1)));
		set_luck_cost(std::make_shared<LuckPointCost>(_unit, get_required_statpoints(luk, luk + 1)));

		set_status_point(std::make_shared<StatusPoint>(_unit, uint32_t(r[7].as_uint64())));
		set_skill_point(std::make_shared<SkillPoint>(_unit, uint32_t(r[8].as_uint64())));

		set_current_hp(std::make_shared<CurrentHP>(_unit, uint32_t(r[9].as_uint64())));
		set_current_sp(std::make_shared<CurrentSP>(_unit, uint32_t(r[10].as_uint64())));
		set_max_hp(std::make_shared<MaxHP>(_unit, uint32_t(r[11].as_uint64())));
		set_max_sp(std::make_shared<MaxSP>(_unit, uint32_t(r[12].as_uint64())));

		uint32_t base_level = uint32_t(r[13].as_uint64());
		uint32_t job_level = uint32_t(r[14].as_uint64());

		set_base_level(std::make_shared<BaseLevel>(_unit, base_level));
		set_job_level(std::make_shared<JobLevel>(_unit, job_level));

		set_base_experience(std::make_shared<BaseExperience>(_unit, uint64_t(r[15].as_uint64())));
		set_job_experience(std::make_shared<JobExperience>(_unit, uint64_t(r[16].as_uint64())));
		set_next_base_experience(std::make_shared<NextBaseExperience>(_unit, base_level == bexpg->max_level ? 0 : bexpg->exp[base_level - 1]));
		set_next_job_experience(std::make_shared<NextJobExperience>(_unit, job_level == jexpg->max_level ? 0 : jexpg->exp[job_level - 1]));
		set_movement_speed(std::make_shared<MovementSpeed>(_unit, DEFAULT_MOVEMENT_SPEED));

		set_base_appearance(std::make_shared<BaseAppearance>(_unit, job_id));
		set_hair_color(std::make_shared<HairColor>(_unit, uint32_t(r[17].as_uint64())));
		set_cloth_color(std::make_shared<ClothColor>(_unit, uint32_t(r[18].as_uint64())));
		set_head_top_sprite(std::make_shared<HeadTopSprite>(_unit, uint32_t(r[19].as_uint64())));
		set_head_mid_sprite(std::make_shared<HeadMidSprite>(_unit, uint32_t(r[20].as_uint64())));
		set_head_bottom_sprite(std::make_shared<HeadBottomSprite>(_unit, uint32_t(r[21].as_uint64())));
		set_hair_style(std::make_shared<HairStyle>(_unit, uint32_t(r[22].as_uint64())));
		set_shield_sprite(std::make_shared<ShieldSprite>(_unit, uint32_t(r[23].as_uint64())));
		set_weapon_sprite(std::make_shared<WeaponSprite>(_unit, uint32_t(r[24].as_uint64())));
		set_robe_sprite(std::make_shared<RobeSprite>(_unit, uint32_t(r[25].as_uint64())));
		set_body_style(std::make_shared<BodyStyle>(_unit, uint32_t(r[26].as_uint64())));

		/**
		 * Misc
		 */
		set_zeny(std::make_shared<Zeny>(_unit, int32_t(r[27].as_uint64())));
		set_virtue(std::make_shared<Virtue>(_unit, int32_t(r[28].as_int64())));
		set_honor(std::make_shared<Honor>(_unit, int32_t(r[29].as_uint64())));
		set_manner(std::make_shared<Manner>(_unit, int32_t(r[30].as_int64())));

		HLog(info) << "Status loaded for character " << pl->name() << "(" << pl->character()._character_id << ").";

	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << "Status::load:" << error.what();
		return false;
	}
	catch (std::exception& error) {
		HLog(error) << "Status::load:" << error.what();
		return false;
	}
	
	return true;
}

bool Status::save(std::shared_ptr<Horizon::Zone::Units::Player> pl)
{
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("UPDATE `character_status` SET `job_id` = ?, `base_level` = ?, `job_level` = ?, `base_experience` = ?, `job_experience` = ?, "
			"`zeny` = ?, `strength` = ?, `agility` = ?, `vitality` = ?, `intelligence` = ?, `dexterity` = ?, `luck` = ?, `maximum_hp` = ?, `hp` = ?, `maximum_sp` = ?, `sp` = ?, "
			"`status_points` = ?, `skill_points` = ?, `body_state` = ?, `virtue` = ?, `honor` = ?, `manner` = ?, `hair_style_id` = ?, `hair_color_id` = ?, `cloth_color_id` = ?, `body_id` = ?, "
			"`weapon_view_id` = ?, `shield_view_id` = ?, `head_top_view_id` = ?, `head_mid_view_id` = ?, `head_bottom_view_id` = ?, `robe_view_id` = ? "
			"WHERE id = ?");
		auto b1 = stmt.bind(pl->job_id(), base_level()->total(), job_level()->total(), base_experience()->total(),
			job_experience()->total(), zeny()->total(), strength()->total(), agility()->total(), vitality()->total(),
			intelligence()->total(), dexterity()->total(),
			luck()->total(), max_hp()->total(), current_hp()->total(), max_sp()->total(), current_sp()->total(), status_point()->total(),
			skill_point()->total(), 0, virtue()->total(), honor()->total(), manner()->total(), hair_style()->get(),
			hair_color()->get(), cloth_color()->get(), body_style()->get(), weapon_sprite()->get(), shield_sprite()->get(),
			head_top_sprite()->get(), head_mid_sprite()->get(), head_bottom_sprite()->get(), robe_sprite()->get(),
			pl->character()._character_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		HLog(info) << "Status saved for character " << pl->name() << "(" << pl->character()._character_id << ").";
	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << "Status::save:" << error.what();
		return false;
	}
	catch (std::exception& error) {
		HLog(error) << "Status::save:" << error.what();
		return false;
	}
	
	return true;
}

void Status::on_equipment_changed(bool equipped, std::shared_ptr<const item_entry_data> item)
{
	if (item == nullptr)
		return;

	if (item->type == IT_TYPE_WEAPON) {
		status_atk()->set_weapon_type(equipped ? item->config->sub_type.weapon_t : IT_WT_FIST);
		equip_atk()->on_weapon_changed();
	}

	attack_speed()->on_equipment_changed();
	attack_range()->on_equipment_changed();
	base_attack()->on_equipment_changed();
	attack_motion()->on_equipment_changed();
	attack_delay()->on_equipment_changed();
	damage_motion()->on_equipment_changed();
}

uint32_t Status::get_required_statpoints(uint16_t from, uint16_t to)
{
	uint32_t sp = 0;

	for (; from < to; from++)
		sp += (from < 100) ? (2 + (from - 1) / 10) : (16 + 4 * ((from - 100) / 5));

	return sp;
}

uint32_t Status::get_status_base(status_point_type type)
{
	uint32_t value = 0;
	switch (type)
	{
		case STATUS_STRENGTH: value = strength()->get_base(); break;
		case STATUS_AGILITY: value = agility()->get_base(); break;
		case STATUS_VITALITY: value = vitality()->get_base(); break;
		case STATUS_INTELLIGENCE: value = intelligence()->get_base(); break;
		case STATUS_DEXTERITY: value = dexterity()->get_base(); break;
		case STATUS_LUCK: value = luck()->get_base(); break;
		default: return 0;
	}

	return value;
}

/**
 * @brief Increases a status point to a certain limit
 * and returns the current total of the stat.
 * @param type[in] Type of the status point @see status_point_type
 * @param limit[in] Limit to increase the status point to.
 * @return The total base value of the current status point.
 */
bool Status::increase_status_point(status_point_type type, uint16_t limit)
{
	uint32_t value = 0;

	if (limit <= 0 || limit > MAX_STATUS_POINTS)
		return false;

	if (_unit.expired())
		return false;

	uint32_t available_points = status_point()->get_base();
	uint32_t required_points = 0;

	limit += get_status_base(type);

#define notify_status(t, amount, result) \
		unit()->template downcast<Horizon::Zone::Units::Player>()->get_session()->clif()->notify_status_attribute_update(t, amount, result)
	while (limit > get_status_base(type)) {
		value = get_status_base(type);
		required_points = get_required_statpoints(value + 1, value + 2);

		if (required_points > available_points || value >= MAX_STATUS_POINTS)
			break;

		available_points -= required_points;
		switch (type)
		{
			case STATUS_STRENGTH:
				strength()->add_base(1);
				strength_cost()->set_base(required_points);
				break;
			case STATUS_AGILITY:
				agility()->add_base(1);
				agility_cost()->set_base(required_points);
				break;
			case STATUS_VITALITY:
				vitality()->add_base(1);
				vitality_cost()->set_base(required_points);
				break;
			case STATUS_INTELLIGENCE:
				intelligence()->add_base(1);
				intelligence_cost()->set_base(required_points);
				break;
			case STATUS_DEXTERITY:
				dexterity()->add_base(1);
				dexterity_cost()->set_base(required_points);
				break;
			case STATUS_LUCK:
				luck()->add_base(1);
				luck_cost()->set_base(required_points);
				break;
			default:
				notify_status(type, get_status_base(type), false);
				return false;
		}

		notify_status(type, get_status_base(type), true);
		status_point()->set_base(available_points);
	}
#undef notify_status
	return true;
}

