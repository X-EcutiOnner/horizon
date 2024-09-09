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

#ifndef HORIZON_ZONE_GAME_TRAITS_STATUS_HPP
#define HORIZON_ZONE_GAME_TRAITS_STATUS_HPP

#include "Server/Zone/Game/Units/Traits/Attribute.hpp"
#include "Server/Zone/Game/Units/Traits/AttributesImpl.hpp"
#include "Server/Zone/Game/Units/Traits/Appearance.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"

#include <ctime>
#include <queue>

 // Linux

struct item_entry_data;
struct monster_config_data;
struct job_config_data;

namespace Horizon
{
namespace Zone
{
class Unit;
namespace Units
{
	class Player;
	class NPC;
	class Mob;
}
namespace Traits
{
class Status
{
public:
	class StatusRegistry
	{
	public:
        enum status_operation_type
        {
            STATUS_OPERATION_ADD_TO_BASE = 0,
            STATUS_OPERATION_SUBTRACT_FROM_BASE,
            STATUS_OPERATION_ADD_TO_EQUIP,
            STATUS_OPERATION_SUBTRACT_FROM_EQUIP,
            STATUS_OPERATION_ADD_TO_STATUS,
            STATUS_OPERATION_SUBTRACT_FROM_STATUS
        };

		class StatusOperation
		{
		public:
			StatusOperation(Attribute *attribute, status_operation_type type, uint16_t value)
			: _priority(std::time(nullptr)), _attribute(attribute), _type(type), _value(value) { }
			virtual ~StatusOperation() {}

			void execute();

			uint16_t get_priority() { return _priority; }
			void set_priority(uint16_t priority) { _priority = priority; }

			status_operation_type get_type() { return _type; }
			uint16_t get_value() { return _value; }

		protected:
			Attribute *_attribute;
			status_operation_type _type;
			uint16_t _value;
			uint16_t _priority;
		};

    	// Define a comparison function for the priority queue
    	struct CompareStatusOperation {
    	    bool operator()(StatusOperation* op1, StatusOperation* op2) {
    	        return op1->get_priority() < op2->get_priority();
    	    }
    	};

		StatusRegistry() { }
		~StatusRegistry() { }

		void add_to_base(Attribute *attribute, uint16_t value);
		void subtract_from_base(Attribute *attribute, uint16_t value);
		void add_to_equip(Attribute *attribute, uint16_t value);
		void subtract_from_equip(Attribute *attribute, uint16_t value);
		void add_to_status(Attribute *attribute, uint16_t value);
		void subtract_from_status(Attribute *attribute, uint16_t value);

		bool has_next_operation() { return !_status_operation_queue.empty(); }
		StatusOperation *get_next_operation() { return _status_operation_queue.top(); }
		void process_queue();

	protected:
    	std::priority_queue<StatusOperation *, std::vector<StatusOperation *>, CompareStatusOperation> _status_operation_queue;
	};

	Status(std::weak_ptr<Unit> unit, unit_type type);
	~Status();

	bool initialize(std::shared_ptr<Horizon::Zone::Units::Mob> creature, std::shared_ptr<const monster_config_data> md);
	bool initialize(std::shared_ptr<Horizon::Zone::Units::Player> player);
	bool initialize(std::shared_ptr<Horizon::Zone::Units::NPC> npc);
	
	uint32_t get_required_statpoints(uint16_t from, uint16_t to);
	uint32_t get_status_base(status_point_type type);
	bool increase_status_point(status_point_type type, uint16_t amount);
	
	bool save(std::shared_ptr<Horizon::Zone::Units::Player> pl);
	bool load(std::shared_ptr<Horizon::Zone::Units::Player> pl);

	void on_equipment_changed(bool equipped, std::shared_ptr<const item_entry_data> item);

	/**
	 * Attributes
	 */
	std::shared_ptr<Strength> strength() { return _str; }
	void set_strength(std::shared_ptr<Strength> str) { _str = str; }

	std::shared_ptr<Agility> agility() { return _agi; }
	void set_agility(std::shared_ptr<Agility> agi) { _agi = agi; }

	std::shared_ptr<Vitality> vitality() { return _vit; }
	void set_vitality(std::shared_ptr<Vitality> vit) { _vit = vit; }

	std::shared_ptr<Dexterity> dexterity() { return _dex; }
	void set_dexterity(std::shared_ptr<Dexterity> dex) { _dex = dex; }

	std::shared_ptr<Intelligence> intelligence() { return _int; }
	void set_intelligence(std::shared_ptr<Intelligence> int_) { _int = int_; }

	std::shared_ptr<Luck> luck() { return _luk; }
	void set_luck(std::shared_ptr<Luck> luck) { _luk = luck; }

	std::shared_ptr<StrengthPointCost> strength_cost() { return _str_cost; }
	void set_strength_cost(std::shared_ptr<StrengthPointCost> str) { _str_cost = str; }

	std::shared_ptr<AgilityPointCost> agility_cost() { return _agi_cost; }
	void set_agility_cost(std::shared_ptr<AgilityPointCost> agi) { _agi_cost = agi; }

	std::shared_ptr<VitalityPointCost> vitality_cost() { return _vit_cost; }
	void set_vitality_cost(std::shared_ptr<VitalityPointCost> vit) { _vit_cost = vit; }

	std::shared_ptr<DexterityPointCost> dexterity_cost() { return _dex_cost; }
	void set_dexterity_cost(std::shared_ptr<DexterityPointCost> dex) { _dex_cost = dex; }

	std::shared_ptr<IntelligencePointCost> intelligence_cost() { return _int_cost; }
	void set_intelligence_cost(std::shared_ptr<IntelligencePointCost> int_) { _int_cost = int_; }

	std::shared_ptr<LuckPointCost> luck_cost() { return _luk_cost; }
	void set_luck_cost(std::shared_ptr<LuckPointCost> luck) { _luk_cost = luck; }

	std::shared_ptr<StatusPoint> status_point() { return _status_point; }
	void set_status_point(std::shared_ptr<StatusPoint> st) { _status_point = st; }

	std::shared_ptr<SkillPoint> skill_point() { return _skill_point; }
	void set_skill_point(std::shared_ptr<SkillPoint> sk) { _skill_point = sk; }

	std::shared_ptr<MaxHP> max_hp() { return _max_hp; }
	void set_max_hp(std::shared_ptr<MaxHP> max_hp) { _max_hp = max_hp; }

	std::shared_ptr<MaxSP> max_sp() { return _max_sp; }
	void set_max_sp(std::shared_ptr<MaxSP> max_sp) { _max_sp = max_sp; }

	std::shared_ptr<CurrentHP> current_hp() { return _current_hp; }
	void set_current_hp(std::shared_ptr<CurrentHP> hp) { _current_hp = hp; }

	std::shared_ptr<CurrentSP> current_sp() { return _current_sp; }
	void set_current_sp(std::shared_ptr<CurrentSP> sp) { _current_sp = sp; }

	std::shared_ptr<BaseLevel> base_level() { return _base_level; }
	void set_base_level(std::shared_ptr<BaseLevel> blvl) { _base_level = blvl; }

	std::shared_ptr<JobLevel> job_level() { return _job_level; }
	void set_job_level(std::shared_ptr<JobLevel> jlvl) { _job_level = jlvl; }

	std::shared_ptr<BaseExperience> base_experience() { return _base_exp; }
	void set_base_experience(std::shared_ptr<BaseExperience> bexp) { _base_exp = bexp; }

	std::shared_ptr<JobExperience> job_experience() { return _job_exp; }
	void set_job_experience(std::shared_ptr<JobExperience> jexp) { _job_exp = jexp; }

	std::shared_ptr<NextBaseExperience> next_base_experience() { return _next_base_exp; }
	void set_next_base_experience(std::shared_ptr<NextBaseExperience> nbexp) { _next_base_exp = nbexp; }

	std::shared_ptr<NextJobExperience> next_job_experience() { return _next_job_exp; }
	void set_next_job_experience(std::shared_ptr<NextJobExperience> njexp) { _next_job_exp = njexp; }

	std::shared_ptr<MovementSpeed> movement_speed() { return _movement_speed; }
	void set_movement_speed(std::shared_ptr<MovementSpeed> speed) { _movement_speed = speed; }

	std::shared_ptr<MaxWeight> max_weight() { return _max_weight; }
	void set_max_weight(std::shared_ptr<MaxWeight> mw) { _max_weight = mw; }

	std::shared_ptr<CurrentWeight> current_weight() { return _current_weight; }
	void set_current_weight(std::shared_ptr<CurrentWeight> cw) { _current_weight = cw; }

	std::shared_ptr<AttackRange> attack_range() { return _attack_range; }
	void set_attack_range(std::shared_ptr<AttackRange> m) { _attack_range = m; }

	/**
	 * SubAttributes
	 */
	std::shared_ptr<StatusATK> status_atk() { return _status_atk; }
	void set_status_atk(std::shared_ptr<StatusATK> satk) { _status_atk = satk; }

	std::shared_ptr<EquipATK> equip_atk() { return _equip_atk; }
	void set_equip_atk(std::shared_ptr<EquipATK> eatk) { _equip_atk = eatk; }

	std::shared_ptr<StatusMATK> status_matk() { return _status_matk; }
	void set_status_matk(std::shared_ptr<StatusMATK> smatk) { _status_matk = smatk; }

	std::shared_ptr<SoftDEF> soft_def() { return _soft_def; }
	void set_soft_def(std::shared_ptr<SoftDEF> sdef) { _soft_def = sdef; }

	std::shared_ptr<SoftMDEF> soft_mdef() { return _soft_mdef; }
	void set_soft_mdef(std::shared_ptr<SoftMDEF> smdef) { _soft_mdef = smdef; }

	std::shared_ptr<HIT> hit() { return _hit; }
	void set_hit(std::shared_ptr<HIT> hit) { _hit = hit; }

	std::shared_ptr<CRIT> crit() { return _crit; }
	void set_crit(std::shared_ptr<CRIT> crit) { _crit = crit; }

	std::shared_ptr<FLEE> flee() { return _flee; }
	void set_flee(std::shared_ptr<FLEE> flee) { _flee = flee; }

	std::shared_ptr<AttackSpeed> attack_speed() { return _aspd; }
	void set_attack_speed(std::shared_ptr<AttackSpeed> aspd) { _aspd = aspd; }

	/**
	 * Appearance
	 */
	std::shared_ptr<BaseAppearance> base_appearance() { return _base_appearance; }
	void set_base_appearance(std::shared_ptr<BaseAppearance> ba) { _base_appearance = ba; }

	std::shared_ptr<HairColor> hair_color() { return _hair_color; }
	void set_hair_color(std::shared_ptr<HairColor> hc) { _hair_color = hc; }

	std::shared_ptr<ClothColor> cloth_color() { return _cloth_color; }
	void set_cloth_color(std::shared_ptr<ClothColor> cc) { _cloth_color = cc; }

	std::shared_ptr<WeaponSprite> weapon_sprite() { return _weapon_sprite; }
	void set_weapon_sprite(std::shared_ptr<WeaponSprite> ws) { _weapon_sprite = ws; }

	std::shared_ptr<ShieldSprite> shield_sprite() { return _shield_sprite; }
	void set_shield_sprite(std::shared_ptr<ShieldSprite> ss) { _shield_sprite = ss; }

	std::shared_ptr<RobeSprite> robe_sprite() { return _robe_sprite; }
	void set_robe_sprite(std::shared_ptr<RobeSprite> rs) { _robe_sprite = rs; }

	std::shared_ptr<HeadTopSprite> head_top_sprite() { return _head_top_sprite; }
	void set_head_top_sprite(std::shared_ptr<HeadTopSprite> hts) { _head_top_sprite = hts; }

	std::shared_ptr<HeadMidSprite> head_mid_sprite() { return _head_mid_sprite; }
	void set_head_mid_sprite(std::shared_ptr<HeadMidSprite> hms) { _head_mid_sprite = hms; }

	std::shared_ptr<HeadBottomSprite> head_bottom_sprite() { return _head_bottom_sprite; }
	void set_head_bottom_sprite(std::shared_ptr<HeadBottomSprite> hbs) { _head_bottom_sprite = hbs; }

	std::shared_ptr<HairStyle> hair_style() { return _hair_style; }
	void set_hair_style(std::shared_ptr<HairStyle> hs) { _hair_style = hs; }

	std::shared_ptr<BodyStyle> body_style() { return _body_style; }
	void set_body_style(std::shared_ptr<BodyStyle> bs) { _body_style = bs; }

	std::shared_ptr<Zeny> zeny() { return _zeny; }
	void set_zeny(std::shared_ptr<Zeny> z) { _zeny = z; }

	std::shared_ptr<Honor> honor() { return _honor; }
	void set_honor(std::shared_ptr<Honor> h) { _honor = h; }

	std::shared_ptr<Manner> manner() { return _manner; }
	void set_manner(std::shared_ptr<Manner> m) { _manner = m; }

	std::shared_ptr<Virtue> virtue() { return _virtue; }
	void set_virtue(std::shared_ptr<Virtue> v) { _virtue = v; }

	/**
	 * Combat Status
	 */
	std::shared_ptr<AttackMotion> attack_motion() { return _attack_motion; }
	void set_attack_motion(std::shared_ptr<AttackMotion> m) { _attack_motion = m; }

	std::shared_ptr<AttackDelay> attack_delay() { return _attack_delay; }
	void set_attack_delay(std::shared_ptr<AttackDelay> a) { _attack_delay = a; }

	std::shared_ptr<DamageMotion> damage_motion() { return _damage_motion; }
	void set_damage_motion(std::shared_ptr<DamageMotion> m) { _damage_motion = m; } 

	std::shared_ptr<BaseAttack> base_attack() { return _base_attack; }
	void set_base_attack(std::shared_ptr<BaseAttack> b) { _base_attack = b; }

	std::shared_ptr<UnitSize> size() { return _size; }
	void set_size(std::shared_ptr<UnitSize> s) { _size = s; }
	
	/* Mob Status */
	std::shared_ptr<MobWeaponAttack> creature_weapon_attack() { return _creature_weapon_attack; }
	void set_creature_weapon_attack(std::shared_ptr<MobWeaponAttack> m) { _creature_weapon_attack = m; }

	std::shared_ptr<MobWeaponAttack> creature_weapon_attack_magic() { return _creature_weapon_attack_magic; }
	void set_creature_weapon_attack_magic(std::shared_ptr<MobWeaponAttack> m) { _creature_weapon_attack_magic = m; }

	std::shared_ptr<MobAttackDamage> creature_attack_damage() { return _creature_attack_damage; }
	void set_creature_attack_damage(std::shared_ptr<MobAttackDamage> m) { _creature_attack_damage = m; }

	std::shared_ptr<MobMagicAttackDamage> creature_magic_attack_damage() { return _creature_magic_attack_damage; }
	void set_creature_magic_attack_damage(std::shared_ptr<MobMagicAttackDamage> m) { _creature_magic_attack_damage = m; }

	std::shared_ptr<MobViewRange> creature_view_range() { return _creature_view_range; }
	void set_creature_view_range(std::shared_ptr<MobViewRange> m) { _creature_view_range = m; }

	std::shared_ptr<MobChaseRange> creature_chase_range() { return _creature_chase_range; }
	void set_creature_chase_range(std::shared_ptr<MobChaseRange> m) { _creature_chase_range = m; }
	
	std::shared_ptr<MobPrimaryRace> creature_primary_race() { return _creature_primary_race; }
	void set_creature_primary_race(std::shared_ptr<MobPrimaryRace> m) { _creature_primary_race = m; }
	
	std::shared_ptr<MobSecondaryRace> creature_secondary_race() { return _creature_secondary_race; }
	void set_creature_secondary_race(std::shared_ptr<MobSecondaryRace> m) { _creature_secondary_race = m; }
	
	std::shared_ptr<MobElement> creature_element() { return _creature_element; }
	void set_creature_element(std::shared_ptr<MobElement> m) { _creature_element = m; }
	
	std::shared_ptr<MobElementLevel> creature_element_level() { return _creature_element_level; }
	void set_creature_element_level(std::shared_ptr<MobElementLevel> m) { _creature_element_level = m; }
	
	std::shared_ptr<MobMode> creature_mode() { return _creature_mode; }
	void set_creature_mode(std::shared_ptr<MobMode> m) { _creature_mode = m; }

	std::shared_ptr<StatusRegistry> status_registry() { return _status_registry; }

	bool is_initialized() { return _is_initialized; }
	void set_initialized(bool b) { _is_initialized = b; }

protected:
	std::shared_ptr<Unit> unit() { return _unit.lock(); }
	unit_type _type{ UNIT_PLAYER };
	bool _is_initialized{ false };

private:
	std::shared_ptr<StatusRegistry> _status_registry{ nullptr };
	std::weak_ptr<Unit> _unit;
	// Attributes
	std::shared_ptr<Strength> _str;
	std::shared_ptr<Agility> _agi;
	std::shared_ptr<Vitality> _vit;
	std::shared_ptr<Intelligence> _int;
	std::shared_ptr<Dexterity> _dex;
	std::shared_ptr<Luck> _luk;
	std::shared_ptr<StrengthPointCost> _str_cost;
	std::shared_ptr<AgilityPointCost> _agi_cost;
	std::shared_ptr<VitalityPointCost> _vit_cost;
	std::shared_ptr<IntelligencePointCost> _int_cost;
	std::shared_ptr<DexterityPointCost> _dex_cost;
	std::shared_ptr<LuckPointCost> _luk_cost;
	std::shared_ptr<StatusPoint> _status_point;
	std::shared_ptr<SkillPoint> _skill_point;
	std::shared_ptr<MaxHP> _max_hp;
	std::shared_ptr<MaxSP> _max_sp;
	std::shared_ptr<CurrentHP> _current_hp;
	std::shared_ptr<CurrentSP> _current_sp;
	std::shared_ptr<BaseLevel> _base_level;
	std::shared_ptr<JobLevel> _job_level;
	std::shared_ptr<BaseExperience> _base_exp;
	std::shared_ptr<JobExperience> _job_exp;
	std::shared_ptr<NextBaseExperience> _next_base_exp;
	std::shared_ptr<NextJobExperience> _next_job_exp;
	std::shared_ptr<MovementSpeed> _movement_speed;
	std::shared_ptr<MaxWeight> _max_weight;
	std::shared_ptr<CurrentWeight> _current_weight;
	std::shared_ptr<AttackRange> _attack_range;
	/* Sub Attributes */
	std::shared_ptr<StatusATK> _status_atk;
	std::shared_ptr<EquipATK> _equip_atk;
	std::shared_ptr<StatusMATK> _status_matk;
	std::shared_ptr<SoftDEF> _soft_def;
	std::shared_ptr<SoftMDEF> _soft_mdef;
	std::shared_ptr<HIT> _hit;
	std::shared_ptr<CRIT> _crit;
	std::shared_ptr<FLEE> _flee;
	std::shared_ptr<AttackSpeed> _aspd;
	/* Appearance */
	std::shared_ptr<BaseAppearance> _base_appearance;
	std::shared_ptr<HairColor> _hair_color;
	std::shared_ptr<ClothColor> _cloth_color;
	std::shared_ptr<WeaponSprite> _weapon_sprite;
	std::shared_ptr<ShieldSprite> _shield_sprite;
	std::shared_ptr<RobeSprite> _robe_sprite;
	std::shared_ptr<HeadTopSprite> _head_top_sprite;
	std::shared_ptr<HeadMidSprite> _head_mid_sprite;
	std::shared_ptr<HeadBottomSprite> _head_bottom_sprite;
	std::shared_ptr<HairStyle> _hair_style;
	std::shared_ptr<BodyStyle> _body_style;
	/* Misc */
	std::shared_ptr<Zeny> _zeny;
	std::shared_ptr<Honor> _honor;
	std::shared_ptr<Manner> _manner;
	std::shared_ptr<Virtue> _virtue;
	/* Combat Status */
	std::shared_ptr<AttackMotion> _attack_motion;
	std::shared_ptr<AttackDelay> _attack_delay;
	std::shared_ptr<DamageMotion> _damage_motion;
	std::shared_ptr<BaseAttack> _base_attack;
	std::shared_ptr<UnitSize> _size;
	/* Mobs */
	std::shared_ptr<MobWeaponAttack> _creature_weapon_attack;
	std::shared_ptr<MobWeaponAttack> _creature_weapon_attack_magic;
	std::shared_ptr<MobAttackDamage> _creature_attack_damage;
	std::shared_ptr<MobMagicAttackDamage> _creature_magic_attack_damage;
	std::shared_ptr<MobViewRange> _creature_view_range;
	std::shared_ptr<MobChaseRange> _creature_chase_range;
	std::shared_ptr<MobPrimaryRace> _creature_primary_race;
	std::shared_ptr<MobSecondaryRace> _creature_secondary_race;
	std::shared_ptr<MobElement> _creature_element;
	std::shared_ptr<MobElementLevel> _creature_element_level;
	std::shared_ptr<MobMode> _creature_mode;
};
}
}
}

#endif /* Status_hpp */
