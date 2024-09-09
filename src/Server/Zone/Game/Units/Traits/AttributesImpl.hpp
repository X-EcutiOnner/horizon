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

#ifndef HORIZON_ZONE_GAME_TRAITS_ATTRIBUTESIMPL_HPP
#define HORIZON_ZONE_GAME_TRAITS_ATTRIBUTESIMPL_HPP

#include "Attribute.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Definitions/MonsterDefinitions.hpp"
#include "Server/Zone/Game/Units/Traits/ObservableStatus.hpp"
 // Linux

namespace Horizon
{
namespace Zone
{
class Unit;
class Player;
namespace Traits
{
	class StatusATK;
	class EquipATK;
	class StatusMATK;
	class SoftDEF;
	class SoftMDEF;
	class HIT;
	class CRIT;
	class FLEE;
	class StrengthPointCost;
	class AgilityPointCost;
	class VitalityPointCost;
	class IntelligencePointCost;
	class DexterityPointCost;
	class LuckPointCost;
	class BaseExperience;
	class JobExperience;
	class Zeny;
	class Honor;
	class Manner;
	class Virtue;
	class StatusPoint;
	class SkillPoint;
	class NextBaseExperience;
	class NextJobExperience;
	class MaxWeight;
	class AttackSpeed;
	class MovementSpeed;
	class AttackMotion;
	class AttackDelay;
	class DamageMotion;

	class BaseLevel
	: public Attribute,
	  public ObservableStatus<BaseLevel *, StatusPoint *, NextBaseExperience *, StatusATK *, StatusMATK *, SoftMDEF *, HIT *, FLEE *, AttackSpeed *>
	{
	public:
		BaseLevel(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_BASELEVEL, base, 0, 0),
		  ObservableStatus(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
		{ }
		~BaseLevel() { };

		void on_observable_changed(BaseExperience *wbexp);
		void notify();

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
		
		void set_equip(int32_t val) override
		{
			Attribute::set_equip(val);
			this->notify_observers();
		}
		
		void set_status(int32_t val) override
		{
			Attribute::set_status(val);
			this->notify_observers();
		}
	};

	class JobLevel
	: public Attribute,
	  public ObservableStatus<JobLevel *, SkillPoint *, NextJobExperience *>
	{
	public:
		JobLevel(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_JOBLEVEL, base, 0, 0),
		  ObservableStatus(nullptr, nullptr)
		{ }
		~JobLevel() { };

		void on_observable_changed(JobExperience *wjexp);
		
		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
		
		void set_equip(int32_t val) override
		{
			Attribute::set_equip(val);
			this->notify_observers();
		}
		
		void set_status(int32_t val) override
		{
			Attribute::set_status(val);
			this->notify_observers();
		}
	};

	class MaxHP
	: public Attribute
	{
	public:
		MaxHP(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_MAXHP, base, equip, status)
		{ }
		~MaxHP() { };
	};

	class MaxSP
	: public Attribute
	{
	public:
		MaxSP(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_MAXSP, base, equip, status)
		{ }
		~MaxSP() { };
	};

	class CurrentHP
	: public Attribute
	{
	public:
		CurrentHP(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_CURRENTHP, base, equip, status)
		{ }
		~CurrentHP() { };

		void damage(int damage);
	};

	class CurrentSP
	: public Attribute
	{
	public:
		CurrentSP(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_CURRENTSP, base, equip, status)
		{ }
		~CurrentSP() { };

		void reduce(int amount);
	};

	class StatusPoint
	: public Attribute
	{
	public:
		StatusPoint(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_STATUSPOINT, base, 0, 0)
		{ }
		~StatusPoint() { };

		void on_observable_changed(BaseLevel *wblvl);
	};

	class SkillPoint
	: public Attribute
	{
	public:
		SkillPoint(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_SKILLPOINT, base, 0, 0)
		{ }
		~SkillPoint() { };

		virtual void set_base(int32_t val) override;
		
		void on_observable_changed(JobLevel *wjlvl);
	};

	class Strength
	: public Attribute,
	  public ObservableStatus<Strength *, StrengthPointCost *, MaxWeight *, StatusATK *, EquipATK *>
	{
	public:
		Strength(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_STRENGTH, base, equip, status),
		  ObservableStatus(nullptr, nullptr, nullptr, nullptr)
		{ }
		~Strength() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
	};

	class Agility
	: public Attribute,
	  public ObservableStatus<Agility *, AgilityPointCost *, FLEE *, AttackSpeed *>
	{
	public:
		Agility(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_AGILITY, base, equip, status),
		  ObservableStatus(nullptr, nullptr, nullptr)
		{ }
		~Agility() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
	};

	class Vitality
	: public Attribute,
	  public ObservableStatus<Vitality *, VitalityPointCost *, SoftDEF *, SoftMDEF *>
	{
	public:
		Vitality(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_VITALITY, base, equip, status),
		  ObservableStatus(nullptr, nullptr, nullptr)
		{ }
		~Vitality() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
	};

	class Intelligence
	: public Attribute,
	  public ObservableStatus<Intelligence *, IntelligencePointCost *, StatusMATK *, SoftMDEF *>
	{
	public:
		Intelligence(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_INTELLIGENCE, base, equip, status),
		  ObservableStatus(nullptr, nullptr, nullptr)
		{ }
		~Intelligence() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
	};

	class Dexterity
	: public Attribute,
	  public ObservableStatus<Dexterity *, DexterityPointCost *, StatusATK *, EquipATK *, StatusMATK *, SoftMDEF *, HIT *, AttackSpeed *>
	{
	public:
		Dexterity(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_DEXTERITY, base, equip, status),
		  ObservableStatus(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
		{ }
		~Dexterity() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
	};

	class Luck
	: public Attribute,
	  public ObservableStatus<Luck *, LuckPointCost *, StatusATK *, StatusMATK *, HIT *, CRIT *, FLEE *>
	{
	public:
		Luck(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_LUCK, base, equip, status),
		  ObservableStatus(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
		{ }
		~Luck() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
	};

	class StrengthPointCost
	: public Attribute
	{
	public:
		StrengthPointCost(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_STRENGTH_COST, base, 0, 0)
		{ }
		~StrengthPointCost() { };

		void on_observable_changed(Strength *);
	};

	class AgilityPointCost
	: public Attribute
	{
	public:
		AgilityPointCost(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_AGILITY_COST, base, 0, 0)
		{ }
		~AgilityPointCost() { };

		void on_observable_changed(Agility *);
	};

	class VitalityPointCost
	: public Attribute
	{
	public:
		VitalityPointCost(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_VITALITY_COST, base, 0, 0)
		{ }
		~VitalityPointCost() { };

		void on_observable_changed(Vitality *);
	};

	class IntelligencePointCost
	: public Attribute
	{
	public:
		IntelligencePointCost(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_INTELLIGENCE_COST, base, 0, 0)
		{ }
		~IntelligencePointCost() { };

		void on_observable_changed(Intelligence *);
	};

	class DexterityPointCost
	: public Attribute
	{
	public:
		DexterityPointCost(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_DEXTERITY_COST, base, 0, 0)
		{ }
		~DexterityPointCost() { };

		void on_observable_changed(Dexterity *);
	};

	class LuckPointCost
	: public Attribute
	{
	public:
		LuckPointCost(std::weak_ptr<Unit> unit, int32_t base = 0)
		: Attribute(unit, STATUS_LUCK_COST, base, 0, 0)
		{ }
		~LuckPointCost() { };

		void on_observable_changed(Luck *);
	};

	class BaseExperience
	: public Attribute,
	  public ObservableStatus<BaseExperience *, BaseLevel *>
	{
	public:
		BaseExperience(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_BASEEXP, base, 0, 0),
		  ObservableStatus(nullptr)
		{ }
		~BaseExperience() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
		
		void set_equip(int32_t val) override
		{
			Attribute::set_equip(val);
			this->notify_observers();
		}
		
		void set_status(int32_t val) override
		{
			Attribute::set_status(val);
			this->notify_observers();
		}
	};

	class JobExperience
	: public Attribute,
	  public ObservableStatus<JobExperience *, JobLevel *>
	{
	public:
		JobExperience(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_JOBEXP, base, 0, 0),
		ObservableStatus(nullptr)
		{ }
		~JobExperience() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
			this->notify_observers();
		}
		
		void set_equip(int32_t val) override
		{
			Attribute::set_equip(val);
			this->notify_observers();
		}
		
		void set_status(int32_t val) override
		{
			Attribute::set_status(val);
			this->notify_observers();
		}
	};

	class Zeny
	: public Attribute
	{
	public:
		Zeny(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_ZENY, base, 0, 0)
		{ }
		~Zeny() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
		}
	};

	class Honor
	: public Attribute
	{
	public:
		Honor(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_KARMA, base, 0, 0)
		{ }
		~Honor() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
		}
	};

	class Manner
	: public Attribute
	{
	public:
		Manner(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_MANNER, base, 0, 0)
		{ }
		~Manner() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
		}
	};

	class Virtue
	: public Attribute
	{
	public:
		Virtue(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_VIRTUE, base, 0, 0)
		{ }
		~Virtue() { };

		void set_base(int32_t val) override
		{
			Attribute::set_base(val);
		}
	};

	class NextBaseExperience
	: public Attribute
	{
	public:
		NextBaseExperience(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_NEXTBASEEXP, base, 0, 0)
		{ }
		~NextBaseExperience() { };

		void on_observable_changed(BaseLevel *blvl);
	};

	class NextJobExperience
	: public Attribute
	{
	public:
		NextJobExperience(std::weak_ptr<Unit> unit,  int32_t base = 0)
		: Attribute(unit, STATUS_NEXTJOBEXP, base, 0, 0)
		{ }
		~NextJobExperience() { };

		void on_observable_changed(JobLevel *jlvl);
	};

	class MaxWeight
	: public Attribute
	{
	public:
		MaxWeight(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_MAX_WEIGHT, base, equip, status)
		{ }
		~MaxWeight() { };

		void on_observable_changed(Strength *wstr) { compute(); }

		int32_t compute();

		void set_strength(Strength *str) { _str = str; }

	private:
		Strength *_str{nullptr};
	};

	class MovementSpeed
	: public Attribute
	{
	public:
		MovementSpeed(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_MOVEMENT_SPEED, base, equip, status)
		{ }
		~MovementSpeed() { }

		virtual void set_base(int32_t val) override;

		int32_t get_with_cost(int cost) { return (total() * cost) / 10; };
	};

	class CurrentWeight
	: public Attribute
	{
	public:
		CurrentWeight(std::weak_ptr<Unit> unit, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: Attribute(unit, STATUS_CURRENT_WEIGHT, base, equip, status)
		{ }
		~CurrentWeight() { };
	};

	class StatusATK
	: public Attribute,
	  public ObservableStatus<StatusATK *, Strength *, Dexterity *, Luck *, BaseLevel *>
	{
	public:
		StatusATK(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_STATUS_ATK),
		ObservableStatus(nullptr, nullptr, nullptr, nullptr)
		{ }
		~StatusATK() { }

		void on_observable_changed(Strength *) { compute(); }
		void on_observable_changed(Dexterity *) { compute(); }
		void on_observable_changed(Luck *) { compute(); }
		void on_observable_changed(BaseLevel *) { compute(); }

		int32_t compute();

		void set_base_level(BaseLevel *blvl) { _blvl = blvl; }
		void set_strength(Strength *str) { _str = str; }
		void set_dexterity(Dexterity *dex) { _dex = dex; }
		void set_luck(Luck *luk) { _luk = luk; }
		void set_weapon_type(item_weapon_type type) { _weapon_type = type; compute(); }

	private:
		BaseLevel *_blvl{nullptr};
		Strength *_str{nullptr};
		Dexterity *_dex{nullptr};
		Luck *_luk{nullptr};
		item_weapon_type _weapon_type;
	};

	class EquipATK
	: public Attribute,
	  public ObservableStatus<EquipATK *, Strength *, Dexterity *>
	{
	public:
		EquipATK(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_EQUIP_ATK),
		ObservableStatus(nullptr, nullptr)
		{ }
		~EquipATK() { }

		void on_observable_changed(Strength *) { compute(); }
		void on_observable_changed(Dexterity *) { compute(); }
		void on_weapon_changed() { compute(); }

		void set_strength(Strength *str) { _str = str; }
		void set_dexterity(Dexterity *dex) { _dex = dex; }

		int32_t compute();
		int32_t compute_variance(int8_t weapon_lvl, int32_t base_weapon_dmg);

		//void notify_update() override { _notifier.notify_sum(); }

		int32_t get_lhw_overupgrade() { return _lhw_overupgrade; }
		int32_t get_rhw_overupgrade() { return _rhw_overupgrade; }

	private:
		Strength *_str{nullptr};
		Dexterity *_dex{nullptr};
		int32_t _left_hand_val{0};
		int32_t _right_hand_val{0};
		int32_t _lhw_overupgrade{0};
		int32_t _rhw_overupgrade{0};
	};

	class StatusMATK
	: public Attribute,
	  public ObservableStatus<StatusMATK *, Intelligence *, Dexterity *, Luck *, BaseLevel *>
	{
	public:
		StatusMATK(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_STATUS_MATK),
		ObservableStatus(nullptr, nullptr, nullptr, nullptr)
		{ }
		~StatusMATK() { }

		void on_observable_changed(Intelligence *) { compute(); }
		void on_observable_changed(Dexterity *) { compute(); }
		void on_observable_changed(Luck *) { compute(); }
		void on_observable_changed(BaseLevel *) { compute(); }

		int32_t compute();

		void set_base_level(BaseLevel *blvl) { _blvl = blvl; }
		void set_intelligence(Intelligence *int_) { _int = int_; }
		void set_dexterity(Dexterity *dex) { _dex = dex; }
		void set_luck(Luck *luk) { _luk = luk; }

	private:
		BaseLevel *_blvl{nullptr};
		Intelligence *_int{nullptr};
		Dexterity *_dex{nullptr};
		Luck *_luk{nullptr};
	};

	class SoftDEF
	: public Attribute,
	  public ObservableStatus<SoftDEF *, Vitality *>
	{
	public:
		SoftDEF(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_SOFT_DEF),
		ObservableStatus(nullptr)
		{ }
		~SoftDEF() { }

		void on_observable_changed(Vitality *) { compute(); }

		int32_t compute();

		void set_vitality(Vitality *vit) { _vit = vit; }

	private:
		Vitality *_vit{nullptr};
	};

	class SoftMDEF
	: public Attribute,
	  public ObservableStatus<SoftMDEF *, Intelligence *, Dexterity *, Vitality *, BaseLevel *>
	{
	public:
		SoftMDEF(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_SOFT_MDEF),
		ObservableStatus(nullptr, nullptr, nullptr, nullptr)
		{ }
		~SoftMDEF() { }

		void on_observable_changed(Intelligence *) { compute(); }
		void on_observable_changed(Dexterity *) { compute(); }
		void on_observable_changed(Vitality *) { compute(); }
		void on_observable_changed(BaseLevel *) { compute(); }

		int32_t compute();

		void set_base_level(BaseLevel *blvl) { _blvl = blvl; }
		void set_intelligence(Intelligence *int_) { _int = int_; }
		void set_dexterity(Dexterity *dex) { _dex = dex; }
		void set_vitality(Vitality *vit) { _vit = vit; }

	private:
		BaseLevel *_blvl{nullptr};
		Intelligence *_int{nullptr};
		Dexterity *_dex{nullptr};
		Vitality *_vit{nullptr};
	};

	class HIT
	: public Attribute,
	  public ObservableStatus<HIT *, Dexterity *, Luck *, BaseLevel *>
	{
	public:
		HIT(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_HIT),
		ObservableStatus(nullptr, nullptr, nullptr)
		{ }
		~HIT() { }

		void on_observable_changed(Dexterity *) { compute(); }
		void on_observable_changed(Luck *) { compute(); }
		void on_observable_changed(BaseLevel *) { compute(); }

		int32_t compute();

		void set_base_level(BaseLevel *blvl) { _blvl = blvl; }
		void set_dexterity(Dexterity *dex) { _dex = dex; }
		void set_luck(Luck *luk) { _luk = luk; }

	private:
		BaseLevel *_blvl{nullptr};
		Dexterity *_dex{nullptr};
		Luck *_luk{nullptr};
	};

	class CRIT
	: public Attribute,
	  public ObservableStatus<CRIT *, Luck *>
	{
	public:
		CRIT(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_CRITICAL),
		ObservableStatus(nullptr)
		{ }
		~CRIT() { }

		void on_observable_changed(Luck *) { compute(); }

		int32_t compute();

		void set_luck(Luck *luk) { _luk = luk; }

	private:
		Luck *_luk{nullptr};
	};

	class FLEE
	: public Attribute,
	  public ObservableStatus<FLEE *, Agility *, BaseLevel *, Luck *>
	{
	public:
		FLEE(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_FLEE),
		ObservableStatus(nullptr, nullptr, nullptr)
		{ }
		~FLEE() { }

		void on_observable_changed(Agility *) { compute(); }
		void on_observable_changed(BaseLevel *) { compute(); }
		void on_observable_changed(Luck *) { compute(); }

		int32_t compute();

		void set_agility(Agility *agi) { _agi = agi; }
		void set_base_level(BaseLevel *blvl) { _blvl = blvl; }
		void set_luck(Luck *luk) { _luk = luk; }

	private:
		Agility *_agi{nullptr};
		BaseLevel *_blvl{nullptr};
		Luck *_luk{nullptr};
	};

	class AttackSpeed
	: public Attribute,
	  public ObservableStatus<AttackSpeed *, Agility *, Dexterity *, BaseLevel *>
	{
	public:
		AttackSpeed(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_ASPD),
		ObservableStatus(nullptr, nullptr, nullptr)
		{ }
		~AttackSpeed() { }

		void on_observable_changed(Agility *) { compute(); }
		void on_observable_changed(Dexterity *) { compute(); }
		void on_observable_changed(BaseLevel *) { compute(); }
		void on_equipment_changed() { compute(); }

		int32_t compute();

		void set_agility(Agility *agi) { _agi = agi; }
		void set_dexterity(Dexterity *dex) { _dex = dex; }
		void set_base_level(BaseLevel *blvl) { _blvl = blvl; }

	private:
		Agility *_agi{nullptr};
		Dexterity *_dex{nullptr};
		BaseLevel *_blvl{nullptr};
	};

	class AttackRange
	: public Attribute
	{
	public:
		AttackRange(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_ATTACKRANGE)
		{ }
		~AttackRange() { }

		void on_equipment_changed() { compute(); }

		int32_t compute();
	};

	class AttackMotion
	: public Attribute,
	  public ObservableStatus<AttackMotion *, AttackSpeed *, Agility *>
	{
	public:
		AttackMotion(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_AMOTION),
		ObservableStatus(nullptr, nullptr)
		{ }
		~AttackMotion() { }

		void on_observable_changed(AttackSpeed *) { compute(); }
		void on_observable_changed(Agility *) { compute(); }

		int32_t compute();

		void on_equipment_changed() { compute(); }

		void set_attack_speed(AttackSpeed *aspd) { _attack_speed = aspd; }
		void set_agility(Agility *agi) { _agi = agi; }

	private:
		Agility *_agi{nullptr};
		AttackSpeed *_attack_speed{nullptr};
	};

	class AttackDelay
	: public Attribute,
	  public ObservableStatus<AttackDelay *, AttackMotion *>
	{
	public:
		AttackDelay(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_ADELAY),
		ObservableStatus(nullptr)
		{ }
		~AttackDelay() { }

		void on_observable_changed(AttackMotion *) { compute(); }
		void on_equipment_changed() { compute(); }
		
		int32_t compute();

		void set_attack_motion(AttackMotion *amotion) { _attack_motion = amotion; }

	private:
		AttackMotion *_attack_motion{nullptr};
	};

	class DamageMotion
	: public Attribute,
	  public ObservableStatus<DamageMotion *, Agility *>
	{
	public:
		DamageMotion(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_DMOTION),
		ObservableStatus(nullptr)
		{ }
		~DamageMotion() { }

		void on_observable_changed(Agility *) { compute(); }
		void on_equipment_changed() { compute(); }

		int32_t compute();

		void set_agility(Agility *agi) { _agi = agi; }

	private:
		Agility *_agi{nullptr};
	};

	class BaseAttack
	: public Attribute,
	  public ObservableStatus<BaseAttack *, Strength *, Dexterity *, Luck *, BaseLevel *>
	{
	public:
		BaseAttack(std::weak_ptr<Unit> unit)
		: Attribute(unit, STATUS_BASE_ATK),
		ObservableStatus(nullptr, nullptr, nullptr, nullptr)
		{ }
		~BaseAttack() { }

		void on_observable_changed(Strength *) { compute(); }
		void on_observable_changed(Dexterity *) { compute(); }
		void on_observable_changed(Luck *) { compute(); }
		void on_observable_changed(BaseLevel *) { compute(); }

		void on_equipment_changed() { compute(); }

		int32_t compute();

		void set_strength(Strength *str) { _str = str; }
		void set_dexterity(Dexterity *dex) { _dex = dex; }
		void set_luck(Luck *luk) { _luk = luk; }
		void set_base_level(BaseLevel *blvl) { _blvl = blvl; }

	private:
		Strength *_str{nullptr};
		Dexterity *_dex{nullptr};
		Luck *_luk{nullptr};
		BaseLevel *_blvl{nullptr};
	};

	class UnitSize
	: public Attribute
	{
	public:
		UnitSize(std::weak_ptr<Unit> unit, int32_t size)
		: Attribute(unit, STATUS_SIZE, size)
		{ }
		~UnitSize() { }
	};

	class MobWeaponAttack
		: public Attribute
	{
	public:
		MobWeaponAttack(std::weak_ptr<Unit> unit, int32_t val)
			: Attribute(unit, STATUS_MOB_WEAPON_ATK, val)
		{ }
		~MobWeaponAttack() { }
	};

	class MobAttackDamage
		: public Attribute,
		public ObservableStatus<MobAttackDamage*, Strength*, BaseLevel*, MobWeaponAttack*>,
		public AttributeVariance
	{
	public:
		MobAttackDamage(std::weak_ptr<Unit> unit)
			: Attribute(unit, STATUS_CREATE_ATTACK_DAMAGE),
			ObservableStatus(nullptr, nullptr, nullptr),
			AttributeVariance()
		{ }
		~MobAttackDamage() { }

		void on_observable_changed(Strength*) { compute(); }
		void on_observable_changed(BaseLevel*) { compute(); }
		void on_observable_changed(MobWeaponAttack*) { compute(); }

		void set_strength(Strength* str) { _str = str; }
		void set_base_level(BaseLevel* blvl) { _blvl = blvl; }
		void set_creature_weapon_attack(MobWeaponAttack* cw_atk) { _cw_atk = cw_atk; }

		int32_t compute();

	private:
		Strength* _str{ nullptr };
		BaseLevel* _blvl{ nullptr };
		MobWeaponAttack* _cw_atk{ nullptr };
	};

	class MobMagicAttackDamage
		: public Attribute,
		public ObservableStatus<MobMagicAttackDamage*, Intelligence*, BaseLevel*, MobWeaponAttack*>,
		public AttributeVariance
	{
	public:
		MobMagicAttackDamage(std::weak_ptr<Unit> unit)
			: Attribute(unit, STATUS_MOB_MAGIC_ATTACK_DAMAGE),
			ObservableStatus(nullptr, nullptr, nullptr),
			AttributeVariance()
		{ }
		~MobMagicAttackDamage() { }

		void on_observable_changed(Intelligence*) { compute(); }
		void on_observable_changed(BaseLevel*) { compute(); }
		void on_observable_changed(MobWeaponAttack*) { compute(); }

		void set_intelligence(Intelligence* _intelligence) { _int = _intelligence; }
		void set_base_level(BaseLevel* blvl) { _blvl = blvl; }
		void set_creature_weapon_attack(MobWeaponAttack* cw_atk) { _cw_atk = cw_atk; }

		int32_t compute();

	private:
		Intelligence* _int{ nullptr };
		BaseLevel* _blvl{ nullptr };
		MobWeaponAttack* _cw_atk{ nullptr };
	};

	class MobViewRange
		: public Attribute
	{
	public:
		MobViewRange(std::weak_ptr<Unit> unit, int32_t val) 
		: Attribute(unit, STATUS_MOB_VIEW_RANGE, val) { }
		~MobViewRange() { }
	};

	class MobChaseRange
		: public Attribute
	{
	public:
		MobChaseRange(std::weak_ptr<Unit> unit, int32_t val) : Attribute(unit, STATUS_MOB_CHASE_RANGE, val) { }
		~MobChaseRange() { }
	};

	class MobPrimaryRace
		: public Attribute
	{
	public:
		MobPrimaryRace(std::weak_ptr<Unit> unit, int32_t val) : Attribute(unit, STATUS_MOB_PRIMARY_RACE, val) { }
		~MobPrimaryRace() { }
	};

	class MobSecondaryRace
		: public Attribute
	{
	public:
		MobSecondaryRace(std::weak_ptr<Unit> unit, int32_t val) : Attribute(unit, STATUS_MOB_SECONDARY_RACE, val) { }
		~MobSecondaryRace() { }
	};

	class MobElement
		: public Attribute
	{
	public:
		MobElement(std::weak_ptr<Unit> unit, int32_t val) : Attribute(unit, STATUS_MOB_ELEMENT, val) { }
		~MobElement() { }
	};

	class MobElementLevel
		: public Attribute
	{
	public:
		MobElementLevel(std::weak_ptr<Unit> unit, int32_t val) : Attribute(unit, STATUS_MOB_ELEMENT_LEVEL, val) { }
		~MobElementLevel() { }
	};

	class MobMode
		: public Attribute
	{
	public:
		MobMode(std::weak_ptr<Unit> unit, int32_t val) : Attribute(unit, STATUS_MOB_MODE, val) { }
		~MobMode() { }
	};
}
}
}

#endif /* HORIZON_ZONE_GAME_TRAITS_ATTRIBUTESIMPL_HPP */
