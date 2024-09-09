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

#include "Server/Zone/Game/Units/Mob/Hostile/Monster.hpp"
#include "Server/Zone/Game/Units/Unit.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Player/Assets/Inventory.hpp"
#include "Server/Zone/Game/StaticDB/JobDB.hpp"
#include "Server/Zone/Game/StaticDB/ExpDB.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Interface/ZoneClientInterface.hpp"

using namespace Horizon::Zone;
using namespace Horizon::Zone::Traits;


void Attribute::add_base(int32_t val) { 
	unit()->status()->status_registry()->add_to_base(this, val);
}
void Attribute::sub_base(int32_t val) { 
	unit()->status()->status_registry()->subtract_from_base(this, std::min(_base_val, val));
}
void Attribute::add_equip(int32_t val) { 
	unit()->status()->status_registry()->add_to_equip(this, val);
}
void Attribute::sub_equip(int32_t val) { 
	unit()->status()->status_registry()->subtract_from_equip(this, std::min(_equip_val, val));
}
void Attribute::add_status(int32_t val) { 
	unit()->status()->status_registry()->add_to_status(this, val);
}
void Attribute::sub_status(int32_t val) { 
	unit()->status()->status_registry()->subtract_from_status(this, std::min(_status_val, val));
}

void Attribute::notify()
{
	if (unit()->type() != UNIT_PLAYER) 
		return;
		
	switch(_status_point_type)
	{
	case STATUS_POINT_INVALID:
		break;
	case STATUS_MOVEMENT_SPEED:
	case STATUS_MAX_WEIGHT:
	case STATUS_CURRENT_WEIGHT:      
	case STATUS_BASELEVEL:    
	case STATUS_JOBLEVEL:
	case STATUS_KARMA:
	case STATUS_MANNER:     
	case STATUS_STATUSPOINT:
	case STATUS_SKILLPOINT:   
	case STATUS_HIT:
	case STATUS_FLEE:
	case STATUS_PERFECT_DODGE:
	case STATUS_CRITICAL:
	case STATUS_MAXHP:    
	case STATUS_MAXSP:        
	case STATUS_CURRENTHP:
	case STATUS_CURRENTSP:        
	case STATUS_ASPD:       
	case STATUS_STATUS_ATK:
	case STATUS_EQUIP_ATK:
	case STATUS_EQUIP_MATK:
	case STATUS_STATUS_MATK:
	case STATUS_SOFT_DEF:
	case STATUS_HARD_DEF:
	case STATUS_SOFT_MDEF:
	case STATUS_HARD_MDEF:
	case STATUS_ZENY:
	{
		unit()->downcast<Horizon::Zone::Units::Player>()->get_session()->clif()->notify_compound_attribute_update(_status_point_type, total());
	}      
	break;  
	case STATUS_BASEEXP:
	case STATUS_JOBEXP:
	case STATUS_NEXTBASEEXP:
	case STATUS_NEXTJOBEXP:
	{
		unit()->downcast<Horizon::Zone::Units::Player>()->get_session()->clif()->notify_experience_update(_status_point_type, total());
	}
	break;
	case STATUS_STRENGTH_COST:
	case STATUS_AGILITY_COST:
	case STATUS_VITALITY_COST:
	case STATUS_INTELLIGENCE_COST:
	case STATUS_DEXTERITY_COST:
	case STATUS_LUCK_COST:
	{
		unit()->downcast<Horizon::Zone::Units::Player>()->get_session()->clif()->notify_required_attribute_update(_status_point_type, total());
	}
	break;
	case STATUS_ATTACKRANGE:
	{
		unit()->downcast<Horizon::Zone::Units::Player>()->get_session()->clif()->notify_attack_range_update(total());
	}
	break;            
	case STATUS_STRENGTH:
	case STATUS_AGILITY:
	case STATUS_VITALITY:
	case STATUS_INTELLIGENCE:
	case STATUS_DEXTERITY:
	case STATUS_LUCK:
	{
		unit()->downcast<Horizon::Zone::Units::Player>()->get_session()->clif()->notify_attribute_update(_status_point_type, get_base(), get_status() + get_equip());
	}
	break;
	case STATUS_CARTINFO:
	{
		unit()->downcast<Horizon::Zone::Units::Player>()->get_session()->clif()->notify_cart_weight_update();
	}
	break;
	default:
		HLog(debug) << "Tried to send attribute update for status point type " << _status_point_type << " which is not implemented yet.";
	break;
	}
}
template <class STATUS_COST_T, class STATUS_T>
void set_new_point_cost(std::shared_ptr<Horizon::Zone::Unit> unit, STATUS_COST_T *cost_t, STATUS_T *stat)
{
	if (unit == nullptr || stat == nullptr)
		return;

	int32_t new_cost = unit->status()->get_required_statpoints(stat->get_base(), stat->get_base() + 1);

	cost_t->set_base(new_cost);
}

void StrengthPointCost::on_observable_changed(Strength *str)
{
	set_new_point_cost(unit(), this, str);
}

void AgilityPointCost::on_observable_changed(Agility *agi)
{
	set_new_point_cost(unit(), this, agi);
}

void VitalityPointCost::on_observable_changed(Vitality *vit)
{
	set_new_point_cost(unit(), this, vit);
}

void IntelligencePointCost::on_observable_changed(Intelligence *_int)
{
	set_new_point_cost(unit(), this, _int);
}

void DexterityPointCost::on_observable_changed(Dexterity *dex)
{
	set_new_point_cost(unit(), this, dex);
}

void LuckPointCost::on_observable_changed(Luck *luk)
{
	set_new_point_cost(unit(), this, luk);
}

void BaseLevel::on_observable_changed(BaseExperience *bexp)
{
	if (unit() == nullptr || bexp == nullptr)
		return;

	if (get_base() >= MAX_LEVEL)
		return;

	if (bexp->get_base() >= unit()->status()->next_base_experience()->get_base()) {
		int carried_over = bexp->get_base() - unit()->status()->next_base_experience()->get_base();
		add_base(1);
		bexp->set_base(carried_over);
	}
}

void JobLevel::on_observable_changed(JobExperience *jexp)
{
	if (unit() == nullptr || jexp == nullptr)
		return;

	std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(unit()->job_id());
	std::shared_ptr<const exp_group_data> jexpg = ExpDB->get_exp_group(job->job_exp_group, EXP_GROUP_TYPE_JOB);

	if (unit()->status()->job_level()->get_base() >= jexpg->max_level)
		return;

	if (jexp->get_base() >= unit()->status()->next_job_experience()->get_base()) {
		int carried_over = jexp->get_base() - unit()->status()->next_job_experience()->get_base();
		add_base(1);
		jexp->set_base(carried_over);
	}
}

void CurrentHP::damage(int damage)
{
	if (damage >= total())
		set_base(0);
	else
		sub_base(damage);
}

void CurrentSP::reduce(int amount)
{
	if (amount >= total())
		set_base(0);
	else
		sub_base(amount);
}

void NextBaseExperience::on_observable_changed(BaseLevel *blvl)
{
	if (unit() == nullptr || blvl == nullptr)
		return;

	std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(unit()->job_id());
	std::shared_ptr<const exp_group_data> bexpg = ExpDB->get_exp_group(job->base_exp_group, EXP_GROUP_TYPE_BASE);

	set_base(bexpg->exp[blvl->get_base() - 1]);
}

void NextJobExperience::on_observable_changed(JobLevel *jlvl)
{
	if (unit() == nullptr || jlvl == nullptr)
		return;

	std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(unit()->job_id());
	std::shared_ptr<const exp_group_data> jexpg = ExpDB->get_exp_group(job->job_exp_group, EXP_GROUP_TYPE_JOB);

	set_base(jexpg->exp[jlvl->get_base() - 1]);
}

void StatusPoint::on_observable_changed(BaseLevel *blvl)
{
	if (unit() == nullptr || blvl == nullptr)
		return;

	add_base(ExpDB->get_status_point(blvl->get_base()) - ExpDB->get_status_point(blvl->get_base() - 1));
}

void SkillPoint::on_observable_changed(JobLevel *jlvl)
{
	if (unit() == nullptr || jlvl == nullptr)
		return;

	add_base(1);
}

void SkillPoint::set_base(int32_t val)
{
	Attribute::set_base(val);
}

int32_t MaxWeight::compute()
{
	if (unit() == nullptr || _str == nullptr)
		return 0;

	std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(unit()->job_id());

	set_base(job->max_weight + _str->get_base() * 300);
	
	return total();
}

void MovementSpeed::set_base(int32_t val)
{
	Attribute::set_base(val);
}

int32_t StatusATK::compute()
{
	int32_t blvl = 1, str = 1, dex = 1, luk = 1;

	if (_blvl != nullptr)
		blvl = _blvl->get_base();

	if (_str != nullptr)
		str = _str->total();

	if (_dex != nullptr)
		dex = _dex->total();

	if (_luk != nullptr)
		luk = _luk->total();

	// Ranged: floor[(BaseLevel ÷ 4) + (Str ÷ 5) + Dex + (Luk ÷ 3)]
	if (((1ULL << _weapon_type) & IT_WTM_RANGED) & ~(1ULL<<IT_WT_FIST))
		set_base(dex + (blvl / 4) + (str / 5) + (luk / 3));

	// Melee: floor[(BaseLevel ÷ 4) + Str + (Dex ÷ 5) + (Luk ÷ 3)]
	set_base(str + (blvl / 4) + (dex / 5) + (luk / 3));
	
	return total();
}

int32_t StatusMATK::compute()
{
	int32_t blvl = 1, int_ = 1, dex = 1, luk = 1;

	if (_blvl != nullptr)
		blvl = _blvl->get_base();

	if (_int != nullptr)
		int_ = _int->total();

	if (_dex != nullptr)
		dex = _dex->total();

	if (_luk != nullptr)
		luk = _luk->total();

	// floor[floor[BaseLevel ÷ 4] + Int + floor[Int ÷ 2] + floor[Dex ÷ 5] + floor[Luk ÷ 3]]
	set_base(int_ + (blvl / 4) + (int_ / 2) + (dex / 5) + (luk / 3));
	
	return total();
}

int32_t SoftDEF::compute()
{
	int32_t vit = 1;

	if (_vit != nullptr)
		vit = _vit->total();

	// (VIT ÷ 2) + Max[(VIT × 0.3), (VIT ^ 2 ÷ 150) − 1]
	set_base((vit / 2) + std::max((vit * 0.3), (std::pow(vit, 2) / 150) - 1));
	
	return total();
}

int32_t SoftMDEF::compute()
{
	int32_t blvl = 1, int_ = 1, dex = 1, vit = 1;

	if (_blvl != nullptr)
		blvl = _blvl->get_base();

	if (_int != nullptr)
		int_ = _int->total();

	if (_dex != nullptr)
		dex = _dex->total();

	if (_vit != nullptr)
		vit = _vit->total();

	// INT + VIT ÷ 5 + DEX ÷ 5 + BaseLv ÷ 4
	set_base(int_ + (vit / 5) + (dex / 5) + (blvl / 4));
	
	return total();
}

int32_t HIT::compute()
{
	int32_t blvl = 1, dex = 1, luk = 1;

	if (_blvl != nullptr)
		blvl = _blvl->get_base();

	if (_dex != nullptr)
		dex = _dex->total();

	if (_luk != nullptr)
		luk = _luk->total();

	// 175 + BaseLv + DEX + Floor(LUK ÷ 3) + Bonus
	set_base(175 + blvl + dex + (luk / 3));
	
	return total();
}

int32_t CRIT::compute()
{
	int32_t luk = 1;

	if (_luk != nullptr)
		luk = _luk->total();

	// LUK × 0.3 + Bonus
	set_base(luk / 3);
	
	return total();
}

//! @brief Computes FLEE status based on agility, luck and Base Level.
//! FLEE = 100 + BaseLv + AGI + Floor(LUK ÷ 5)
int32_t FLEE::compute()
{
	int32_t blvl = 1, agi = 1, luk = 1;

	if (_agi != nullptr)
		agi = _agi->total();

	if (_blvl != nullptr)
		blvl = _blvl->get_base();

	if (_luk != nullptr)
		luk = _luk->total();

	set_base(100 + blvl + agi + (luk / 5));
	
	return total();
}

//! @brief Computes the EquipATK property of physical attacks.
//! EquipATK = floor[((BaseWeaponDamage + Variance + StatBonus + RefinementBonus + OverUpgradeBonus) × SizePenaltyMultiplier]
int32_t EquipATK::compute()
{
	int32_t str = 1, dex = 1;

	if (unit() == nullptr || unit()->type() != UNIT_PLAYER)
		return 0;

	if (_str != nullptr)
		str = _str->total();

	if (_dex != nullptr)
		dex = _dex->total();

	std::shared_ptr<Horizon::Zone::Units::Player> player = unit()->downcast<Horizon::Zone::Units::Player>();
	EquipmentListType const &equipments = player->inventory()->equipments();

	std::shared_ptr<const item_entry_data> lhw = equipments[IT_EQPI_HAND_L].second.lock();
	std::shared_ptr<const item_entry_data> rhw = equipments[IT_EQPI_HAND_R].second.lock();

	if (lhw && lhw->type == IT_TYPE_WEAPON) {
		// Base Weapon Damage
		_left_hand_val = lhw->config->attack;
		// StatBonus = BaseWeaponDamage × (Melee: Str / Ranged: Dex) ÷ 200
		_left_hand_val += (lhw->config->attack * (((1ULL << lhw->config->sub_type.weapon_t) & IT_WTM_MELEE) ? str : dex) / 200.00f);
	} else {
		_left_hand_val = 0;
	}

	if (rhw && rhw->type == IT_TYPE_WEAPON) {
		// Base Weapon Damage
		_right_hand_val = rhw->config->attack;
		// StatBonus = BaseWeaponDamage × (Melee: Str / Ranged: Dex) ÷ 200
		_right_hand_val += (rhw->config->attack * ((( 1ULL << rhw->config->sub_type.weapon_t) & IT_WTM_MELEE) ? str : dex) / 200.00f);
	} else {
		_right_hand_val = 0;
	}

	set_base(_left_hand_val + _right_hand_val);

	return total();
}

int32_t EquipATK::compute_variance(int8_t weapon_lvl, int32_t base_weapon_dmg)
{
	srand(time(0));

	return floor(((rand() % 1000 + (-500)) / 10000.f) * weapon_lvl * base_weapon_dmg);
}

int32_t AttackSpeed::compute()
{
	float temp_aspd = 0.00f;
	int amotion = 0;

	if (unit()->type() == UNIT_PLAYER) {
		EquipmentListType const &equipments = unit()->downcast<Horizon::Zone::Units::Player>()->inventory()->equipments();
		std::shared_ptr<const item_entry_data> rhw = equipments[IT_EQPI_HAND_R].second.lock();
		std::shared_ptr<const item_entry_data> lhw = equipments[IT_EQPI_HAND_L].second.lock();
		std::shared_ptr<const job_config_data> job = JobDB->get_job_by_id(unit()->job_id());

		item_weapon_type rhw_type, lhw_type;

		if (rhw == nullptr) { 
			rhw_type = IT_WT_FIST;
		} else {
			rhw_type = rhw->config->sub_type.weapon_t;
		}

		amotion = job->weapon_base_aspd[rhw_type];

		if (rhw_type > IT_WT_SINGLE_MAX && lhw != nullptr) {
			lhw_type = lhw->config->sub_type.weapon_t;
			amotion += job->weapon_base_aspd[lhw_type] / 4; // Dual-wield
		}

		if (_dex == nullptr || _blvl == nullptr || _agi == nullptr) {
			HLog(error) << "AttackSpeed::compute: Couldn't compute... Dex, Blvl or Agi was null.";
			return amotion;
		}

		switch(rhw_type) {
			case IT_WT_BOW:
			case IT_WT_MUSICAL:
			case IT_WT_WHIP:
			case IT_WT_REVOLVER:
			case IT_WT_RIFLE:
			case IT_WT_GATLING:
			case IT_WT_SHOTGUN:
			case IT_WT_GRENADE:
				temp_aspd = _dex->get_base() * _dex->get_base() / 7.0f + _agi->get_base() * _agi->get_base() * 0.5f;
				break;
			default:
				temp_aspd = _dex->get_base() * _dex->get_base() / 5.0f + _agi->get_base() * _agi->get_base() * 0.5f;
				break;
		}

		temp_aspd = (float)(sqrt(temp_aspd) * 0.25f) + 196;

#define ASPD_FROM_STATUS_EFFECTS 0
#define ASPD_FROM_SKILLS 0
		amotion = (int)(temp_aspd + ((float)((ASPD_FROM_STATUS_EFFECTS + ASPD_FROM_SKILLS) * _agi->get_base() / 200)) - std::min(amotion, 200));
		amotion += (std::max(0xc3 - amotion, 2) * (ASPD_FROM_STATUS_EFFECTS)) / 100;
		amotion = 10 * (200 - amotion);
#undef ASPD_FROM_STATUS_EFFECTS
#undef ASPD_FROM_SKILLS
	}

 	set_base(amotion);

	return total();
}

int32_t AttackRange::compute()
{
	if (unit()->type() == UNIT_PLAYER) {
		EquipmentListType const &equipments = unit()->downcast<Horizon::Zone::Units::Player>()->inventory()->equipments();
		
		if (equipments[IT_EQPI_HAND_R].second.expired() == true)
			return 1;

		std::shared_ptr<const item_entry_data> rhw = equipments[IT_EQPI_HAND_R].second.lock();
		
		if (rhw == nullptr || rhw->config == nullptr)
			return 1;

		set_base(rhw->config->attack_range);

	} else if (unit()->type() == UNIT_MONSTER) {
		set_base(unit()->downcast<Horizon::Zone::Units::Monster>()->monster_config()->attack_range);
	}

	return total();
}

int32_t MobAttackDamage::compute()
{
	set_min(_blvl->total() + _str->total() + (_cw_atk->total() * 8 / 10));
	set_max(_blvl->total() + _str->total() + (_cw_atk->total() * 12 / 10));

	return std::rand() % ((_max - _min + 1) + _min);
}

int32_t MobMagicAttackDamage::compute()
{
	set_min(_blvl->total() + _int->total() + (_cw_atk->total() * 7 / 10));
	set_max(_blvl->total() + _int->total() + (_cw_atk->total() * 13 / 10));

	return std::rand() % (_max - _min + 1) + _min;
}

int32_t AttackMotion::compute()
{
	if (unit()->type() == UNIT_PLAYER)
		set_base(_attack_speed->get_base());
	else if (unit()->type() == UNIT_MONSTER)
		set_base(unit()->downcast<Horizon::Zone::Units::Monster>()->monster_config()->attack_motion);

	return total();
}

int32_t AttackDelay::compute()
{
	if (unit()->type() == UNIT_PLAYER)
		set_base(2 * _attack_motion->total());
	else if (unit()->type() == UNIT_MONSTER)
		set_base(unit()->downcast<Horizon::Zone::Units::Monster>()->monster_config()->attack_delay);

	return total();
}

int32_t DamageMotion::compute()
{
	if (unit()->type() == UNIT_PLAYER)
		set_base(800 - _agi->get_base() * 4);
	else if (unit()->type() == UNIT_MONSTER)
		set_base(unit()->downcast<Horizon::Zone::Units::Monster>()->monster_config()->damage_motion);

	return total();
}

int32_t BaseAttack::compute()
{
	if (unit()->type() == UNIT_PLAYER) {
		bool melee = true;
		EquipmentListType const &equipments = unit()->downcast<Horizon::Zone::Units::Player>()->inventory()->equipments();
		std::shared_ptr<const item_entry_data> rhw = nullptr;

		if (!equipments[IT_EQPI_HAND_R].second.expired()) {
			rhw = equipments[IT_EQPI_HAND_R].second.lock();

			switch (rhw->config->sub_type.weapon_t)
			{
			case IT_WT_BOW:
			case IT_WT_MUSICAL:
			case IT_WT_WHIP:
			case IT_WT_REVOLVER:
			case IT_WT_RIFLE:
			case IT_WT_GATLING:
			case IT_WT_SHOTGUN:
			case IT_WT_GRENADE:
				melee = false;
			default:
				break;
			}
		}
		set_base((melee ? _str->total() : _dex->total()) + (float) ((melee ? _dex->get_base() : _str->total()) / 5) + (float) (_luk->get_base() / 3) + (_blvl->get_base() / 4));
	}
	else if (unit()->type() == UNIT_MONSTER)
		set_base(_str->total() + _blvl->get_base());

	return total();
}