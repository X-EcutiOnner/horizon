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

#include "UnitComponent.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Unit.hpp"
#include "Server/Zone/Game/Units/Battle/Combat.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Definitions/ClientDefinitions.hpp"
#include "Server/Zone/Game/Map/Grid/Notifiers/GridNotifiers.hpp"

#include "UnitDefinitions.hpp"

using namespace Horizon::Zone;

void UnitComponent::sync_definitions(std::shared_ptr<sol::state> state)
{
	state->create_named_table("UnitSize",
		"Small", (int) ESZ_SMALL,
		"Medium", (int) ESZ_MEDIUM,
		"Large", (int) ESZ_LARGE,
		"Max", (int) ESZ_MAX
	);

	state->new_enum("Element",
		"Neutral", (int) ELE_NEUTRAL,
		"Water", (int) ELE_WATER,
		"Earth", (int) ELE_EARTH,
		"Fire", (int) ELE_FIRE,
		"Wind", (int) ELE_WIND,
		"Poison", (int) ELE_POISON,
		"Holy", (int) ELE_HOLY,
		"Dark", (int) ELE_DARK,
		"Ghost", (int) ELE_GHOST,
		"Undead", (int) ELE_UNDEAD,
		"Max", (int) ELE_MAX,
		"Weapon", (int) ELE_WEAPON,
		"Endowed", (int) ELE_ENDOWED,
		"Random", (int) ELE_RANDOM
	);

	state->create_named_table("UnitType",
		"Player", (int) UNIT_MASK_PLAYER,
		"NPC", (int) UNIT_MASK_NPC,
		"Item", (int) UNIT_MASK_ITEM,
		"Skill", (int) UNIT_MASK_SKILL,
		"Monster", (int) UNIT_MASK_MONSTER,
		"Event", (int) UNIT_MASK_EVENT,
		"Pet", (int) UNIT_MASK_PET,
		"Homunculus", (int) UNIT_MASK_HOMUNCULUS,
		"Mercenary", (int) UNIT_MASK_MERCENARY,
		"Elemental", (int) UNIT_MASK_ELEMENTAL
	);

	state->create_named_table("Directions",
		"North", (int) DIR_NORTH,
		"NorthWest", (int) DIR_NORTH_WEST,
		"West", (int) DIR_WEST,
		"SouthWest", (int) DIR_SOUTH_WEST,
		"South", (int) DIR_SOUTH,
		"SouthEast", (int) DIR_SOUTH_EAST,
		"East", (int) DIR_EAST,
		"NorthEast", (int) DIR_NORTH_EAST
	);

	state->create_named_table("Posture",
		"Standing", (int) POSTURE_STANDING,
		"Dead", (int) POSTURE_DEAD,
		"Sitting", (int) POSTURE_SITTING
	);
	state->create_named_table("zc_notify_act_3_action_types",
    	"Damage", (int) ZCNA3_DAMAGE,
    	"PickupItem", (int) ZCNA3_PICKUP_ITEM,
    	"Sit", (int) ZCNA3_SIT,
    	"Stand", (int) ZCNA3_STAND,
    	"EndureDamage", (int) ZCNA3_ENDURE_DAMAGE,
    	"Splash", (int) ZCNA3_SPLASH,
    	"Skill", (int) ZCNA3_SKILL ,
    	"RepeatDamage", (int) ZCNA3_REPEAT_DAMAGE,
    	"MultiHitDamage", (int) ZCNA3_MULTI_HIT_DAMAGE,
    	"MultiHitEndureDamage", (int) ZCNA3_MULTI_HIT_ENDURE_DAMAGE,
    	"CriticalHit ", (int) ZCNA3_CRITICAL_HIT,
    	"LuckyDodge ", (int) ZCNA3_LUCKY_DODGE,
    	"TouchSkill", (int) ZCNA3_TOUCH_SKILL,
    	"MultiHitCritical", (int) ZCNA3_MULTI_HIT_CRITICAL
	);

	state->create_named_table("grid_unit_skill_use_notification_type",
		"GRID_UNIT_SKILL_USE_NOTIFY_CASTTIME", GRID_UNIT_SKILL_USE_NOTIFY_CASTTIME,
		"GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_DAMAGE", GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_DAMAGE,
    	"GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_NO_DAMAGE", GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_NO_DAMAGE
	);

	sol::table tbl = state->create_named_table("Job");

	tbl["Novice"] = (int) JOB_NOVICE;
	tbl["Swordman"] = (int) JOB_SWORDMAN;
	tbl["Magician"] = (int) JOB_MAGE;
	tbl["Archer"] = (int) JOB_ARCHER;
	tbl["Acolyte"] = (int) JOB_ACOLYTE;
	tbl["Merchant"] = (int) JOB_MERCHANT;
	tbl["Thief"] = (int) JOB_THIEF;
	tbl["Knight"] = (int) JOB_KNIGHT;
	tbl["Priest"] = (int) JOB_PRIEST;
	tbl["Wizard"] = (int) JOB_WIZARD;
	tbl["Blacksmith"] = (int) JOB_BLACKSMITH;
	tbl["Hunter"] = (int) JOB_HUNTER;
	tbl["Assassin"] = (int) JOB_ASSASSIN;
	//JOB_KNIGHT2
	tbl["Crusader"] = (int) JOB_CRUSADER;
	tbl["Monk"] = (int) JOB_MONK;
	tbl["Sage"] = (int) JOB_SAGE;
	tbl["Rogue"] = (int) JOB_ROGUE;
	tbl["Alchemist"] = (int) JOB_ALCHEMIST;
	tbl["Bard"] = (int) JOB_BARD;
	tbl["Dancer"] = (int) JOB_DANCER;
	tbl["Wedding"] = (int) JOB_WEDDING;
	tbl["SuperNovice"] = (int) JOB_SUPER_NOVICE;
	tbl["Gunslinger"] = (int) JOB_GUNSLINGER;
	tbl["Ninja"] = (int) JOB_NINJA;
	tbl["Xmas"] = (int) JOB_XMAS;
	tbl["Summer"] = (int) JOB_SUMMER;
	//4000-
	tbl["HighNovice"] = (int) JOB_NOVICE_HIGH;
	tbl["HighSwordman"] = (int) JOB_SWORDMAN_HIGH;
	tbl["HighMagician"] = (int) JOB_MAGE_HIGH;
	tbl["HighArcher"] = (int) JOB_ARCHER_HIGH;
	tbl["HighAcolyte"] = (int) JOB_ACOLYTE_HIGH;
	tbl["HighMerchant"] = (int) JOB_MERCHANT_HIGH;
	tbl["HighThief"] = (int) JOB_THIEF_HIGH;
	tbl["LordKnight"] = (int) JOB_LORD_KNIGHT;
	tbl["HighPriest"] = (int) JOB_HIGH_PRIEST;
	tbl["HighWizard"] = (int) JOB_HIGH_WIZARD;
	tbl["Whitesmith"] = (int) JOB_WHITESMITH;
	tbl["Sniper"] = (int) JOB_SNIPER;
	tbl["AssassinCross"] = (int) JOB_ASSASSIN_CROSS;
	tbl["Paladin"] = (int) JOB_PALADIN;
	tbl["Champion"] = (int) JOB_CHAMPION;
	tbl["Professor"] = (int) JOB_PROFESSOR;
	tbl["Stalker"] = (int) JOB_STALKER;
	tbl["Creator"] = (int) JOB_CREATOR;
	tbl["Clown"] = (int) JOB_CLOWN;
	tbl["Gypsy"] = (int) JOB_GYPSY;
	tbl["BabyNovice"] = (int) JOB_BABY_NOVICE;
	tbl["BabySwordman"] = (int) JOB_BABY_SWORDMAN;
	tbl["BabyMagician"] = (int) JOB_BABY_MAGE;
	tbl["BabyArcher"] = (int) JOB_BABY_ARCHER;
	tbl["BabyAcolyte"] = (int) JOB_BABY_ACOLYTE;
	tbl["BabyMerchant"] = (int) JOB_BABY_MERCHANT;
	tbl["BabyThief"] = (int) JOB_BABY_THIEF;
	tbl["BabyKnight"] = (int) JOB_BABY_KNIGHT;
	tbl["BabyPriest"] = (int) JOB_BABY_PRIEST;
	tbl["BabyWizard"] = (int) JOB_BABY_WIZARD;
	tbl["BabyBlacksmith"] = (int) JOB_BABY_BLACKSMITH;
	tbl["BabyHunter"] = (int) JOB_BABY_HUNTER;
	tbl["BabyAssassin"] = (int) JOB_BABY_ASSASSIN;
	tbl["BabyCrusader"] = (int) JOB_BABY_CRUSADER;
	tbl["BabyMonk"] = (int) JOB_BABY_MONK;
	tbl["BabySage"] = (int) JOB_BABY_SAGE;
	tbl["BabyRogue"] = (int) JOB_BABY_ROGUE;
	tbl["BabyAlchemist"] = (int) JOB_BABY_ALCHEMIST;
	tbl["BabyBard"] = (int) JOB_BABY_BARD;
	tbl["BabyDancer"] = (int) JOB_BABY_DANCER;
	tbl["SuperBaby"] = (int) JOB_SUPER_BABY;
	tbl["Taekwon"] = (int) JOB_TAEKWON;
	tbl["StarGladiator"] = (int) JOB_STAR_GLADIATOR;
	tbl["SoulLinker"] = (int) JOB_SOUL_LINKER;
	tbl["Gangsi"] = (int) JOB_GANGSI;
	tbl["DeathKnight"] = (int) JOB_DEATH_KNIGHT;
	tbl["DarkCollector"] = (int) JOB_DARK_COLLECTOR;
	// 4053-
	tbl["RuneKnight"] = (int) JOB_RUNE_KNIGHT;
	tbl["Warlock"] = (int) JOB_WARLOCK;
	tbl["Ranger"] = (int) JOB_RANGER;
	tbl["Archbishop"] = (int) JOB_ARCH_BISHOP;
	tbl["Mechanic"] = (int) JOB_MECHANIC;
	tbl["GuillotineCross"] = (int) JOB_GUILLOTINE_CROSS;
	tbl["RuneKnightTrans"] = (int) JOB_RUNE_KNIGHT_T;
	tbl["WarlockTrans"] = (int) JOB_WARLOCK_T;
	tbl["RangerTrans"] = (int) JOB_RANGER_T;
	tbl["ArchbishopTrans"] = (int) JOB_ARCH_BISHOP_T;
	tbl["MechanicTrans"] = (int) JOB_MECHANIC_T;
	tbl["GuillotineCrossTrans"] = (int) JOB_GUILLOTINE_CROSS_T;
	tbl["RoyalGuard"] = (int) JOB_ROYAL_GUARD;
	tbl["Sorcerer"] = (int) JOB_SORCERER;
	tbl["Minstrel"] = (int) JOB_MINSTREL;
	tbl["Wanderer"] = (int) JOB_WANDERER;
	tbl["Sura"] = (int) JOB_SURA;
	tbl["Genetic"] = (int) JOB_GENETIC;
	tbl["ShadowChaser"] = (int) JOB_SHADOW_CHASER;
	tbl["RoyalGuardTrans"] = (int) JOB_ROYAL_GUARD_T;
	tbl["SorcererTrans"] = (int) JOB_SORCERER_T;
	tbl["MinstrelTrans"] = (int) JOB_MINSTREL_T;
	tbl["WandererTrans"] = (int) JOB_WANDERER_T;
	tbl["SuraTrans"] = (int) JOB_SURA_T;
	tbl["GeneticTrans"] = (int) JOB_GENETIC_T;
	tbl["ShadowChaserTrans"] = (int) JOB_SHADOW_CHASER_T;
	//4088-4097
	tbl["BabyRuneKnight"] = (int) JOB_BABY_RUNE_KNIGHT;
	tbl["BabyWarlock"] = (int) JOB_BABY_WARLOCK;
	tbl["BabyRanger"] = (int) JOB_BABY_RANGER;
	tbl["BabyArchbishop"] = (int) JOB_BABY_ARCH_BISHOP;
	tbl["BabyMechanic"] = (int) JOB_BABY_MECHANIC;
	tbl["BabyGuillotineCross"] = (int) JOB_BABY_GUILLOTINE_CROSS;
	tbl["BabyRoyalGuard"] = (int) JOB_BABY_ROYAL_GUARD;
	tbl["BabySorcerer"] = (int) JOB_BABY_SORCERER;
	tbl["BabyMinstrel"] = (int) JOB_BABY_MINSTREL;
	tbl["BabyWanderer"] = (int) JOB_BABY_WANDERER;
	tbl["BabySura"] = (int) JOB_BABY_SURA;
	tbl["BabyGenetic"] = (int) JOB_BABY_GENETIC;
	tbl["BabyShadowChaser"] = (int) JOB_BABY_SHADOW_CHASER;
	tbl["SuperNoviceExpanded"] = (int) JOB_SUPER_NOVICE_E;
	tbl["SuperBabyExpanded"] = (int) JOB_SUPER_BABY_E;
	tbl["Kagerou"] = (int) JOB_KAGEROU;
	tbl["Oboro"] = (int) JOB_OBORO;
	tbl["Rebellion"] = (int) JOB_REBELLION;
	tbl["Summoner"] = (int) JOB_SUMMONER;
	tbl["StartBase"] = (int) JOB_BASE_START;
	tbl["EndBase"] = (int) JOB_BASE_END;
	tbl["Start_2_1"] = (int) JOB_2_1_START;
	tbl["End_2_1"] = (int) JOB_2_1_END;
	tbl["Start_2_2"] = (int) JOB_2_2_START;
	tbl["End_2_2"] = (int) JOB_2_2_END;
	tbl["StartTransBase"] = (int) JOB_TRANS_BASE_START;
	tbl["EndTransBase"] = (int) JOB_TRANS_BASE_END;
	tbl["Start_Trans_2_1"] = (int) JOB_TRANS_2_1_START;
	tbl["End_Trans_2_1"] = (int) JOB_TRANS_2_1_END;
	tbl["Start_Trans_2_2"] = (int) JOB_TRANS_2_2_START;
	tbl["End_Trans_2_2"] = (int) JOB_TRANS_2_2_END;
	tbl["Start_Baby_Base"] = (int) JOB_BABY_BASE_START;
	tbl["End_Baby_Base"] = (int) JOB_BABY_BASE_END;
	tbl["Start_Baby_2_1"] = (int) JOB_BABY_2_1_START;
	tbl["End_Baby_2_1"] = (int) JOB_BABY_2_1_END;
	tbl["Start_Baby_2_2"] = (int) JOB_BABY_2_2_START;
	tbl["End_Baby_2_2"] = (int) JOB_BABY_2_2_END;
	tbl["Start_3_1"] = (int) JOB_3_1_START;
	tbl["End_3_1"] = (int) JOB_3_1_END;
	tbl["Start_3_2"] = (int) JOB_3_2_START;
	tbl["End_3_2"] = (int) JOB_3_2_END;
	tbl["Start_Trans_3_1"] = (int) JOB_TRANS_3_1_START;
	tbl["End_Trans_3_1"] = (int) JOB_TRANS_3_1_END;
	tbl["Start_Trans_3_2"] = (int) JOB_TRANS_3_2_START;
	tbl["End_Trans_3_2"] = (int) JOB_TRANS_3_2_END;
	tbl["Start_Baby_3_1"] = (int) JOB_BABY_3_1_START;
	tbl["End_Baby_3_1"] = (int) JOB_BABY_3_1_END;
	tbl["Start_Baby_3_2"] = (int) JOB_BABY_3_2_START;
	tbl["End_Baby_3_2"] = (int) JOB_BABY_3_2_END;

	sol::table job_mask_table = state->create_named_table("JobMask");
	job_mask_table["Invalid"] = (int) JMASK_INVALID;
	job_mask_table["Novice"] = (int) JMASK_NOVICE;
	job_mask_table["Swordman"] = (int) JMASK_SWORDMAN;
	job_mask_table["Magician"] = (int) JMASK_MAGE;
	job_mask_table["Archer"] = (int) JMASK_ARCHER;
	job_mask_table["Acolyte"] = (int) JMASK_ACOLYTE;
	job_mask_table["Merchant"] = (int) JMASK_MERCHANT;
	job_mask_table["Thief"] = (int) JMASK_THIEF;
	job_mask_table["Gangsi"] = (int) JMASK_GANGSI;
	job_mask_table["Summoner"] = (int) JMASK_SUMMONER;
	job_mask_table["Taekwon"] = (int) JMASK_TAEKWON_BASE;
	job_mask_table["Gunslinger"] = (int) JMASK_GUNSLINGER_BASE;
	job_mask_table["Ninja"] = (int) JMASK_NINJA_BASE;
	job_mask_table["BaseClass"] = (int) JMASK_BASE;
	job_mask_table["Wedding"] = (int) JMASK_WEDDING;
	job_mask_table["2_1_Class"] = (int) JMASK_2_1;
	job_mask_table["2_2_Class"] = (int) JMASK_2_2;
	job_mask_table["BabyBaseClass"] = (int) JMASK_BABY_BASE;
	job_mask_table["Baby_1_1_Class"] = (int) JMASK_BABY_1_1;
	job_mask_table["Baby_1_2_Class"] = (int) JMASK_BABY_1_2;
	job_mask_table["Baby_2_1_Class"] = (int) JMASK_BABY_2_1;
	job_mask_table["Baby_2_2_Class"] = (int) JMASK_BABY_2_2;
	job_mask_table["TransBaseClass"] = (int) JMASK_TRANS_BASE;
	job_mask_table["Trans_1_1_Class"] = (int) JMASK_TRANS_1_1;
	job_mask_table["Trans_1_2_Class"] = (int) JMASK_TRANS_1_2;
	job_mask_table["Trans_2_1_Class"] = (int) JMASK_TRANS_2_1;
	job_mask_table["Trans_2_2_Class"] = (int) JMASK_TRANS_2_2;
	job_mask_table["3_1_Class"] = (int) JMASK_3_1;
	job_mask_table["3_2_Class"] = (int) JMASK_3_2;
	job_mask_table["Trans_3_1_Class"] = (int) JMASK_TRANS_3_1;
	job_mask_table["Trans_3_2_Class"] = (int) JMASK_TRANS_3_2;
	job_mask_table["Baby_3_1_Class"] = (int) JMASK_BABY_3_1;
	job_mask_table["Baby_3_3_Class"] = (int) JMASK_BABY_3_2;
	job_mask_table["ExpandedClass"] = (int) JMASK_EXPANDED;
	job_mask_table["ExpandedBabyClass"] = (int) JMASK_EXPANDED_BABY;
	job_mask_table["Expanded_2_1_Class"] = (int) JMASK_EXPANDED_2_1;
	job_mask_table["Expanded_2_2_Class"] = (int) JMASK_EXPANDED_2_2;

	job_mask_table["NormalClass"] = (int) JMASK_NORMAL;
	job_mask_table["BabyClass"] = (int) JMASK_BABY;
	job_mask_table["TransClass"] = (int) JMASK_TRANS;
	job_mask_table["ThirdClass"] = (int) JMASK_3;
	job_mask_table["TransThirdClass"] = (int) JMASK_TRANS_3;
	job_mask_table["BabyThirdClass"] = (int) JMASK_BABY_3;

	// 2-1 Jobs
	job_mask_table["Knight"] = (int) JMASK_KNIGHT;
	job_mask_table["Wizard"] = (int) JMASK_WIZARD;
	job_mask_table["Hunter"] = (int) JMASK_HUNTER;
	job_mask_table["Priest"] = (int) JMASK_PRIEST;
	job_mask_table["Blacksmith"] = (int) JMASK_BLACKSMITH;
	job_mask_table["Assassin"] = (int) JMASK_ASSASSIN;
	job_mask_table["DeathKnight"] = (int) JMASK_DEATH_KNIGHT;

	// 2-2 Jobs
	job_mask_table["Crusader"] = (int) JMASK_CRUSADER;
	job_mask_table["Sage"] = (int) JMASK_SAGE;
	job_mask_table["BardOrDancer"] = (int) JMASK_BARDDANCER;
	job_mask_table["Monk"] = (int) JMASK_MONK;
	job_mask_table["Alchemist"] = (int) JMASK_ALCHEMIST;
	job_mask_table["Rogue"] = (int) JMASK_ROGUE;
	job_mask_table["DarkCollector"] = (int) JMASK_DARK_COLLECTOR;

	//Trans Novice And Trans 1-1 J
	job_mask_table["HighNovice"] = (int) JMASK_NOVICE_HIGH;
	job_mask_table["HighSwordman"] = (int) JMASK_SWORDMAN_HIGH;
	job_mask_table["HighMagician"] = (int) JMASK_MAGE_HIGH;
	job_mask_table["HighArcher"] = (int) JMASK_ARCHER_HIGH;
	job_mask_table["HighAcolyte"] = (int) JMASK_ACOLYTE_HIGH;
	job_mask_table["HighMerchant"] = (int) JMASK_MERCHANT_HIGH;
	job_mask_table["HighThief"] = (int) JMASK_THIEF_HIGH;

	//Trans 2-1 Jobs
	job_mask_table["LordKnight"] = (int) JMASK_LORD_KNIGHT;
	job_mask_table["HighWizard"] = (int) JMASK_HIGH_WIZARD;
	job_mask_table["Sniper"] = (int) JMASK_SNIPER;
	job_mask_table["HighPriest"] = (int) JMASK_HIGH_PRIEST;
	job_mask_table["Whitesmith"] = (int) JMASK_WHITESMITH;
	job_mask_table["AssassinCross"] = (int) JMASK_ASSASSIN_CROSS;

	//Trans 2-2 Jobs
	job_mask_table["Paladin"] = (int) JMASK_PALADIN;
	job_mask_table["Professor"] = (int) JMASK_PROFESSOR;
	job_mask_table["ClownOrGypsy"] = (int) JMASK_CLOWNGYPSY;
	job_mask_table["Champion"] = (int) JMASK_CHAMPION;
	job_mask_table["Creator"] = (int) JMASK_CREATOR;
	job_mask_table["Stalker"] = (int) JMASK_STALKER;

	//Baby Novice And Baby 1-1 Job
	job_mask_table["BabyNovice"] = (int) JMASK_BABY_NOVICE;
	job_mask_table["BabySwordman"] = (int) JMASK_BABY_SWORDMAN;
	job_mask_table["BabyMage"] = (int) JMASK_BABY_MAGE;
	job_mask_table["BabyArcher"] = (int) JMASK_BABY_ARCHER;
	job_mask_table["BabyAcolyte"] = (int) JMASK_BABY_ACOLYTE;
	job_mask_table["BabyMerchant"] = (int) JMASK_BABY_MERCHANT;
	job_mask_table["BabyThief"] = (int) JMASK_BABY_THIEF;

	//Baby 2-1 Jobs
	job_mask_table["BabyKnight"] = (int) JMASK_BABY_KNIGHT;
	job_mask_table["BabyWizard"] = (int) JMASK_BABY_WIZARD;
	job_mask_table["BabyHunter"] = (int) JMASK_BABY_HUNTER;
	job_mask_table["BabyPriest"] = (int) JMASK_BABY_PRIEST;
	job_mask_table["BabyBlacksmith"] = (int) JMASK_BABY_BLACKSMITH;
	job_mask_table["BabyAssassin"] = (int) JMASK_BABY_ASSASSIN;

	//Baby 2-2 Jobs
	job_mask_table["BabyCrusader"] = (int) JMASK_BABY_CRUSADER;
	job_mask_table["BabySage"] = (int) JMASK_BABY_SAGE;
	job_mask_table["BabyBardOrDancer"] = (int) JMASK_BABY_BARDDANCER;
	job_mask_table["BabyMonk"] = (int) JMASK_BABY_MONK;
	job_mask_table["BabyAlchemist"] = (int) JMASK_BABY_ALCHEMIST;
	job_mask_table["BabyRogue"] = (int) JMASK_BABY_ROGUE;

	//3-1 Jobs
	job_mask_table["RuneKnight"] = (int) JMASK_RUNE_KNIGHT;
	job_mask_table["Warlock"] = (int) JMASK_WARLOCK;
	job_mask_table["Ranger"] = (int) JMASK_RANGER;
	job_mask_table["Archbishop"] = (int) JMASK_ARCH_BISHOP;
	job_mask_table["Mechanic"] = (int) JMASK_MECHANIC;
	job_mask_table["GuillotineCross"] = (int) JMASK_GUILLOTINE_CROSS;

	//3-2 Jobs
	job_mask_table["RoyalGuard"] = (int) JMASK_ROYAL_GUARD;
	job_mask_table["Sorcerer"] = (int) JMASK_SORCERER;
	job_mask_table["MinstrelOrWanderer"] = (int) JMASK_MINSTRELWANDERER;
	job_mask_table["Sura"] = (int) JMASK_SURA;
	job_mask_table["Genetic"] = (int) JMASK_GENETIC;
	job_mask_table["ShadowChaser"] = (int) JMASK_SHADOW_CHASER;

	//Trans 3-1 Jobs
	job_mask_table["RuneKnightTrans"] = (int) JMASK_RUNE_KNIGHT_T;
	job_mask_table["WarlockTrans"] = (int) JMASK_WARLOCK_T;
	job_mask_table["RangerTrans"] = (int) JMASK_RANGER_T;
	job_mask_table["ArchbishopTrans"] = (int) JMASK_ARCH_BISHOP_T;
	job_mask_table["MechanicTrans"] = (int) JMASK_MECHANIC_T;
	job_mask_table["GuillotineCrossTrans"] = (int) JMASK_GUILLOTINE_CROSS_T;

	//Trans 3-2 Jobs
	job_mask_table["RoyalGuardTrans"] = (int) JMASK_ROYAL_GUARD_T;
	job_mask_table["SorcererTrans"] = (int) JMASK_SORCERER_T;
	job_mask_table["MinstrelOrWandererTrans"] = (int) JMASK_MINSTRELWANDERER_T;
	job_mask_table["SuraTrans"] = (int) JMASK_SURA_T;
	job_mask_table["GeneticTrans"] = (int) JMASK_GENETIC_T;
	job_mask_table["ShadowChaserTrans"] = (int) JMASK_SHADOW_CHASER_T;

	//Baby 3-1 Jobs
	job_mask_table["BabyRuneKnight"] = (int) JMASK_BABY_RUNE_KNIGHT;
	job_mask_table["BabyWarlock"] = (int) JMASK_BABY_WARLOCK;
	job_mask_table["BabyRanger"] = (int) JMASK_BABY_RANGER;
	job_mask_table["BabyArchbishop"] = (int) JMASK_BABY_ARCH_BISHOP;
	job_mask_table["BabyMechanic"] = (int) JMASK_BABY_MECHANIC;
	job_mask_table["BabyGuillotineCross"] = (int) JMASK_BABY_GUILLOTINE_CROSS;

	//Baby 3-2 Jobs
	job_mask_table["BabyRoyalGuard"] = (int) JMASK_BABY_ROYAL_GUARD;
	job_mask_table["BabySorcerer"] = (int) JMASK_BABY_SORCERER;
	job_mask_table["BabyMinstrelOrWanderer"] = (int) JMASK_BABY_MINSTRELWANDERER;
	job_mask_table["BabySura"] = (int) JMASK_BABY_SURA;
	job_mask_table["BabyGenetic"] = (int) JMASK_BABY_GENETIC;
	job_mask_table["BabyShadowChaser"] = (int) JMASK_BABY_SHADOW_CHASER;

	// Expanded Jobs
	job_mask_table["Taekwon"] = (int) JMASK_TAEKWON;
	job_mask_table["StarGladiator"] = (int) JMASK_STAR_GLADIATOR;
	job_mask_table["SoulLinker"] = (int) JMASK_SOUL_LINKER;
	job_mask_table["SuperNovice"] = (int) JMASK_SUPER_NOVICE;
	job_mask_table["SuperNoviceExpanded"] = (int) JMASK_SUPER_NOVICE_E;
	job_mask_table["SuperBaby"] = (int) JMASK_SUPER_BABY;
	job_mask_table["SuperBabyExpanded"] = (int) JMASK_SUPER_BABY_E;
	job_mask_table["Gunslinger"] = (int) JMASK_GUNSLINGER;
	job_mask_table["Rebellion"] = (int) JMASK_REBELLION;
	job_mask_table["Ninja"] = (int) JMASK_NINJA;
	job_mask_table["KagerouOrOboro"] = (int) JMASK_KAGEROUOBORO;
	job_mask_table["All"] = (int) JMASK_ALL;
}

void UnitComponent::sync_data_types(std::shared_ptr<sol::state> state)
{
	sol::usertype<Horizon::Zone::Traits::Attribute> config_3 = state->new_usertype<Horizon::Zone::Traits::Attribute>("Attribute");
	config_3["add_base"] = &Horizon::Zone::Traits::Attribute::add_base;
	config_3["sub_base"] = &Horizon::Zone::Traits::Attribute::sub_base;
	config_3["add_equip"] = &Horizon::Zone::Traits::Attribute::add_equip;
	config_3["sub_equip"] = &Horizon::Zone::Traits::Attribute::sub_equip;
	config_3["add_status"] = &Horizon::Zone::Traits::Attribute::add_status;
	config_3["sub_status"] = &Horizon::Zone::Traits::Attribute::sub_status;
	config_3["total"] = &Horizon::Zone::Traits::Attribute::total;

    sol::usertype<Horizon::Zone::Traits::Status> config_4 = state->new_usertype<Horizon::Zone::Traits::Status>("Status");
    config_4["strength"] = &Horizon::Zone::Traits::Status::strength;
    config_4["agility"] = &Horizon::Zone::Traits::Status::agility;
    config_4["vitality"] = &Horizon::Zone::Traits::Status::vitality;
    config_4["dexterity"] = &Horizon::Zone::Traits::Status::dexterity;
    config_4["intelligence"] = &Horizon::Zone::Traits::Status::intelligence;
    config_4["luck"] = &Horizon::Zone::Traits::Status::luck;
    config_4["max_hp"] = &Horizon::Zone::Traits::Status::max_hp;
    config_4["max_sp"] = &Horizon::Zone::Traits::Status::max_sp;
    config_4["current_hp"] = &Horizon::Zone::Traits::Status::current_hp;
    config_4["current_sp"] = &Horizon::Zone::Traits::Status::current_sp;
    config_4["base_level"] = &Horizon::Zone::Traits::Status::base_level;
    config_4["job_level"] = &Horizon::Zone::Traits::Status::job_level;
    config_4["base_experience"] = &Horizon::Zone::Traits::Status::base_experience;
    config_4["job_experience"] = &Horizon::Zone::Traits::Status::job_experience;
    config_4["next_base_experience"] = &Horizon::Zone::Traits::Status::next_base_experience;
    config_4["next_job_experience"] = &Horizon::Zone::Traits::Status::next_job_experience;
    config_4["movement_speed"] = &Horizon::Zone::Traits::Status::movement_speed;
    config_4["max_weight"] = &Horizon::Zone::Traits::Status::max_weight;
    config_4["current_weight"] = &Horizon::Zone::Traits::Status::current_weight;
    config_4["hair_color"] = &Horizon::Zone::Traits::Status::hair_color;
    config_4["cloth_color"] = &Horizon::Zone::Traits::Status::cloth_color;
    config_4["weapon_sprite"] = &Horizon::Zone::Traits::Status::weapon_sprite;
    config_4["shield_sprite"] = &Horizon::Zone::Traits::Status::shield_sprite;
    config_4["robe_sprite"] = &Horizon::Zone::Traits::Status::robe_sprite;
    config_4["head_top_sprite"] = &Horizon::Zone::Traits::Status::head_top_sprite;
    config_4["head_mid_sprite"] = &Horizon::Zone::Traits::Status::head_mid_sprite;
    config_4["head_bottom_sprite"] = &Horizon::Zone::Traits::Status::head_bottom_sprite;
    config_4["hair_style"] = &Horizon::Zone::Traits::Status::hair_style;
    config_4["body_style"] = &Horizon::Zone::Traits::Status::body_style;
    config_4["status_point"] = &Horizon::Zone::Traits::Status::status_point;
    config_4["skill_point"] = &Horizon::Zone::Traits::Status::skill_point;
	config_4["zeny"] = &Horizon::Zone::Traits::Status::zeny;

	sol::usertype<Combat> config_30 = state->new_usertype<Combat>("Combat",
		sol::constructors<Combat(std::shared_ptr<Unit>, std::shared_ptr<Unit>)>());
	config_30["source"] = &Combat::unit;
	config_30["target"] = &Combat::target;

    sol::usertype<Horizon::Zone::Traits::BaseLevel> config_5 = state->new_usertype<Horizon::Zone::Traits::BaseLevel>("BaseLevel");
    config_5["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::BaseLevel> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_5["add"] = &Horizon::Zone::Traits::BaseLevel::add_base;
    config_5["sub"] = &Horizon::Zone::Traits::BaseLevel::sub_base;
    config_5["get"] = &Horizon::Zone::Traits::BaseLevel::get_base;
    config_5["set"] = &Horizon::Zone::Traits::BaseLevel::set_base;
    sol::usertype<Horizon::Zone::Traits::JobLevel> config_6 = state->new_usertype<Horizon::Zone::Traits::JobLevel>("JobLevel");
    config_6["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::JobLevel> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_6["add"] = &Horizon::Zone::Traits::JobLevel::add_base;
    config_6["sub"] = &Horizon::Zone::Traits::JobLevel::sub_base;
    config_6["get"] = &Horizon::Zone::Traits::JobLevel::get_base;
    config_6["set"] = &Horizon::Zone::Traits::JobLevel::set_base;
    sol::usertype<Horizon::Zone::Traits::MaxHP> config_7 = state->new_usertype<Horizon::Zone::Traits::MaxHP>("MaxHP");
    config_7["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::MaxHP> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_7["add"] = &Horizon::Zone::Traits::MaxHP::add_base;
    config_7["sub"] = &Horizon::Zone::Traits::MaxHP::sub_base;
    config_7["get"] = &Horizon::Zone::Traits::MaxHP::get_base;
    config_7["set"] = &Horizon::Zone::Traits::MaxHP::set_base;
    sol::usertype<Horizon::Zone::Traits::MaxSP> config_8 = state->new_usertype<Horizon::Zone::Traits::MaxSP>("MaxSP");
    config_8["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::MaxSP> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_8["add"] = &Horizon::Zone::Traits::MaxSP::add_base;
    config_8["sub"] = &Horizon::Zone::Traits::MaxSP::sub_base;
    config_8["get"] = &Horizon::Zone::Traits::MaxSP::get_base;
    config_8["set"] = &Horizon::Zone::Traits::MaxSP::set_base;
    sol::usertype<Horizon::Zone::Traits::CurrentHP> config_9 = state->new_usertype<Horizon::Zone::Traits::CurrentHP>("CurrentHP");
    config_9["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::CurrentHP> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_9["add"] = &Horizon::Zone::Traits::CurrentHP::add_base;
    config_9["sub"] = &Horizon::Zone::Traits::CurrentHP::sub_base;
    config_9["get"] = &Horizon::Zone::Traits::CurrentHP::get_base;
    config_9["set"] = &Horizon::Zone::Traits::CurrentHP::set_base;
    sol::usertype<Horizon::Zone::Traits::CurrentSP> config_10 = state->new_usertype<Horizon::Zone::Traits::CurrentSP>("CurrentSP");
    config_10["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::CurrentHP> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_10["add"] = &Horizon::Zone::Traits::CurrentSP::add_base;
    config_10["sub"] = &Horizon::Zone::Traits::CurrentSP::sub_base;
    config_10["get"] = &Horizon::Zone::Traits::CurrentSP::get_base;
    config_10["set"] = &Horizon::Zone::Traits::CurrentSP::set_base;
    sol::usertype<Horizon::Zone::Traits::MovementSpeed> config_11 = state->new_usertype<Horizon::Zone::Traits::MovementSpeed>("MovementSpeed");
    config_11["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::MovementSpeed> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_11["add"] = &Horizon::Zone::Traits::MovementSpeed::add_base;
    config_11["sub"] = &Horizon::Zone::Traits::MovementSpeed::sub_base;
    config_11["get"] = &Horizon::Zone::Traits::MovementSpeed::get_base;
    config_11["set"] = &Horizon::Zone::Traits::MovementSpeed::set_base;
    sol::usertype<Horizon::Zone::Traits::MaxWeight> config_12 = state->new_usertype<Horizon::Zone::Traits::MaxWeight>("MaxWeight");
    config_12["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::MaxWeight> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_12["add"] = &Horizon::Zone::Traits::MaxWeight::add_base;
    config_12["sub"] = &Horizon::Zone::Traits::MaxWeight::sub_base;
    config_12["get"] = &Horizon::Zone::Traits::MaxWeight::get_base;
    config_12["set"] = &Horizon::Zone::Traits::MaxWeight::set_base;
    sol::usertype<Horizon::Zone::Traits::CurrentWeight> config_13 = state->new_usertype<Horizon::Zone::Traits::CurrentWeight>("CurrentWeight");
    config_13["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::CurrentWeight> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_13["add"] = &Horizon::Zone::Traits::CurrentWeight::add_base;
    config_13["sub"] = &Horizon::Zone::Traits::CurrentWeight::sub_base;
    config_13["get"] = &Horizon::Zone::Traits::CurrentWeight::get_base;
    config_13["set"] = &Horizon::Zone::Traits::CurrentWeight::set_base;
    sol::usertype<Horizon::Zone::Traits::Strength> config_14 = state->new_usertype<Horizon::Zone::Traits::Strength>("Strength");
    config_14["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Strength> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_14["add"] = &Horizon::Zone::Traits::Strength::add_base;
    config_14["sub"] = &Horizon::Zone::Traits::Strength::sub_base;
    config_14["get"] = &Horizon::Zone::Traits::Strength::get_base;
    config_14["set"] = &Horizon::Zone::Traits::Strength::set_base;
    sol::usertype<Horizon::Zone::Traits::Agility> config_15 = state->new_usertype<Horizon::Zone::Traits::Agility>("Agility");
    config_15["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Agility> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_15["add"] = &Horizon::Zone::Traits::Agility::add_base;
    config_15["sub"] = &Horizon::Zone::Traits::Agility::sub_base;
    config_15["get"] = &Horizon::Zone::Traits::Agility::get_base;
    config_15["set"] = &Horizon::Zone::Traits::Agility::set_base;
    sol::usertype<Horizon::Zone::Traits::Vitality> config_16 = state->new_usertype<Horizon::Zone::Traits::Vitality>("Vitality");
    config_16["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Vitality> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_16["add"] = &Horizon::Zone::Traits::Vitality::add_base;
    config_16["sub"] = &Horizon::Zone::Traits::Vitality::sub_base;
    config_16["get"] = &Horizon::Zone::Traits::Vitality::get_base;
    config_16["set"] = &Horizon::Zone::Traits::Vitality::set_base;
    sol::usertype<Horizon::Zone::Traits::Intelligence> config_17 = state->new_usertype<Horizon::Zone::Traits::Intelligence>("Intelligence");
    config_17["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Intelligence> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_17["add"] = &Horizon::Zone::Traits::Intelligence::add_base;
    config_17["sub"] = &Horizon::Zone::Traits::Intelligence::sub_base;
    config_17["get"] = &Horizon::Zone::Traits::Intelligence::get_base;
    config_17["set"] = &Horizon::Zone::Traits::Intelligence::set_base;
    sol::usertype<Horizon::Zone::Traits::Dexterity> config_18 = state->new_usertype<Horizon::Zone::Traits::Dexterity>("Dexterity");
    config_18["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Dexterity> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_18["add"] = &Horizon::Zone::Traits::Dexterity::add_base;
    config_18["sub"] = &Horizon::Zone::Traits::Dexterity::sub_base;
    config_18["get"] = &Horizon::Zone::Traits::Dexterity::get_base;
    config_18["set"] = &Horizon::Zone::Traits::Dexterity::set_base;
    sol::usertype<Horizon::Zone::Traits::Luck> config_19 = state->new_usertype<Horizon::Zone::Traits::Luck>("Luck");
    config_19["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Luck> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_19["add"] = &Horizon::Zone::Traits::Luck::add_base;
    config_19["sub"] = &Horizon::Zone::Traits::Luck::sub_base;
    config_19["get"] = &Horizon::Zone::Traits::Luck::get_base;
    config_19["set"] = &Horizon::Zone::Traits::Luck::set_base;
    sol::usertype<Horizon::Zone::Traits::BaseExperience> config_20 = state->new_usertype<Horizon::Zone::Traits::BaseExperience>("BaseExperience");
    config_20["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::BaseExperience> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_20["add"] = &Horizon::Zone::Traits::BaseExperience::add_base;
    config_20["sub"] = &Horizon::Zone::Traits::BaseExperience::sub_base;
    config_20["get"] = &Horizon::Zone::Traits::BaseExperience::get_base;
    config_20["set"] = &Horizon::Zone::Traits::BaseExperience::set_base;
    sol::usertype<Horizon::Zone::Traits::JobExperience> config_21 = state->new_usertype<Horizon::Zone::Traits::JobExperience>("JobExperience");
    config_21["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::JobExperience> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_21["add"] = &Horizon::Zone::Traits::JobExperience::add_base;
    config_21["sub"] = &Horizon::Zone::Traits::JobExperience::sub_base;
    config_21["get"] = &Horizon::Zone::Traits::JobExperience::get_base;
    config_21["set"] = &Horizon::Zone::Traits::JobExperience::set_base;
    sol::usertype<Horizon::Zone::Traits::NextBaseExperience> config_22 = state->new_usertype<Horizon::Zone::Traits::NextBaseExperience>("NextBaseExperience");
    config_22["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::NextBaseExperience> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_22["add"] = &Horizon::Zone::Traits::NextBaseExperience::add_base;
    config_22["sub"] = &Horizon::Zone::Traits::NextBaseExperience::sub_base;
    config_22["get"] = &Horizon::Zone::Traits::NextBaseExperience::get_base;
    config_22["set"] = &Horizon::Zone::Traits::NextBaseExperience::set_base;
    sol::usertype<Horizon::Zone::Traits::NextJobExperience> config_23 = state->new_usertype<Horizon::Zone::Traits::NextJobExperience>("NextJobExperience");
    config_23["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::NextJobExperience> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_23["add"] = &Horizon::Zone::Traits::NextJobExperience::add_base;
    config_23["sub"] = &Horizon::Zone::Traits::NextJobExperience::sub_base;
    config_23["get"] = &Horizon::Zone::Traits::NextJobExperience::get_base;
    config_23["set"] = &Horizon::Zone::Traits::NextJobExperience::set_base;
    sol::usertype<Horizon::Zone::Traits::StatusPoint> config_24 = state->new_usertype<Horizon::Zone::Traits::StatusPoint>("StatusPoint");
    config_24["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::StatusPoint> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_24["add"] = &Horizon::Zone::Traits::StatusPoint::add_base;
    config_24["sub"] = &Horizon::Zone::Traits::StatusPoint::sub_base;
    config_24["get"] = &Horizon::Zone::Traits::StatusPoint::get_base;
    config_24["set"] = &Horizon::Zone::Traits::StatusPoint::set_base;
    sol::usertype<Horizon::Zone::Traits::SkillPoint> config_25 = state->new_usertype<Horizon::Zone::Traits::SkillPoint>("SkillPoint");
    config_25["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::SkillPoint> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_25["add"] = &Horizon::Zone::Traits::SkillPoint::add_base;
    config_25["sub"] = &Horizon::Zone::Traits::SkillPoint::sub_base;
    config_25["get"] = &Horizon::Zone::Traits::SkillPoint::get_base;
    config_25["set"] = &Horizon::Zone::Traits::SkillPoint::set_base;
    sol::usertype<Horizon::Zone::Traits::Zeny> config_26 = state->new_usertype<Horizon::Zone::Traits::Zeny>("Zeny");
    config_26["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Zeny> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_26["add"] = &Horizon::Zone::Traits::Zeny::add_base;
    config_26["sub"] = &Horizon::Zone::Traits::Zeny::sub_base;
    config_26["get"] = &Horizon::Zone::Traits::Zeny::get_base;
    config_26["set"] = &Horizon::Zone::Traits::Zeny::set_base;
    sol::usertype<Horizon::Zone::Traits::Honor> config_27 = state->new_usertype<Horizon::Zone::Traits::Honor>("Honor");
    config_27["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Honor> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_27["add"] = &Horizon::Zone::Traits::Honor::add_base;
    config_27["sub"] = &Horizon::Zone::Traits::Honor::sub_base;
    config_27["get"] = &Horizon::Zone::Traits::Honor::get_base;
    config_27["set"] = &Horizon::Zone::Traits::Honor::set_base;
    sol::usertype<Horizon::Zone::Traits::Manner> config_28 = state->new_usertype<Horizon::Zone::Traits::Manner>("Manner");
    config_28["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Manner> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_28["add"] = &Horizon::Zone::Traits::Manner::add_base;
    config_28["sub"] = &Horizon::Zone::Traits::Manner::sub_base;
    config_28["get"] = &Horizon::Zone::Traits::Manner::get_base;
    config_28["set"] = &Horizon::Zone::Traits::Manner::set_base;
    sol::usertype<Horizon::Zone::Traits::Virtue> config_29 = state->new_usertype<Horizon::Zone::Traits::Virtue>("Virtue");
    config_29["attribute"] = [](std::shared_ptr<Horizon::Zone::Traits::Virtue> t) { return std::static_pointer_cast<Horizon::Zone::Traits::Attribute>(t); };
    config_29["add"] = &Horizon::Zone::Traits::Virtue::add_base;
    config_29["sub"] = &Horizon::Zone::Traits::Virtue::sub_base;
    config_29["get"] = &Horizon::Zone::Traits::Virtue::get_base;
    config_29["set"] = &Horizon::Zone::Traits::Virtue::set_base;
	sol::usertype<unit_uuid> uuid = state->new_usertype<unit_uuid>("UnitUUID");
	uuid["type"] = &unit_uuid::type;
	uuid["guid"] = &unit_uuid::guid;
	uuid["uid2"] = &unit_uuid::uid2;
	uuid["uid3"] = &unit_uuid::uid3;

	sol::usertype<Unit> config_1 = state->new_usertype<Unit>("Unit");
	config_1["guid"] = &Unit::guid;
	config_1["uuid"] = &Unit::uuid;
	config_1["s_uuid"] = &Unit::s_uuid;
	config_1["set_uuid"] = &Unit::set_uuid;
	config_1["dest_coords"] = &Unit::dest_coords;
	config_1["walk_to_coordinates"] = &Unit::walk_to_coordinates;
	config_1["is_walking"] = &Unit::is_walking;
	config_1["stop_movement"] = &Unit::stop_movement;
	config_1["job_id"] = &Unit::job_id;
	config_1["posture"] = &Unit::posture;
	config_1["set_posture"] = &Unit::set_posture;
	config_1["name"] = &Unit::name;
	config_1["direction"] = &Unit::direction;
	config_1["status"] = &Unit::status;
	config_1["force_movement_stop_internal"] = &Unit::force_movement_stop_internal;
	config_1["map"] = &Unit::map;
	config_1["type"] = &Unit::type;
	config_1["map_coords"] = &Unit::map_coords;
	config_1["grid_coords"] = &Unit::grid_coords;
	config_1["is_in_range_of"] = &Unit::is_in_range_of;
	config_1["notify_nearby_players_of_existence"] = &Unit::notify_nearby_players_of_existence;
	config_1["notify_nearby_players_of_movement"] = &Unit::notify_nearby_players_of_movement;
	config_1["notify_nearby_players_of_spawn"] = &Unit::notify_nearby_players_of_spawn;
	config_1["notify_nearby_players_of_skill_use"] = &Unit::notify_nearby_players_of_skill_use;
	config_1["notify_nearby_players_of_item_drop"] = &Unit::notify_nearby_players_of_item_drop;
	config_1["get_nearby_unit"] = &Unit::get_nearby_unit;
	config_1["status_effect_start"] = &Unit::status_effect_start;
	config_1["status_effect_end"] = &Unit::status_effect_end;
	config_1["get_walk_path"] = &Unit::get_walk_path;
	config_1["is_dead"] = &Unit::is_dead;
	config_1["combat"] = &Unit::combat;
	config_1["combat_registry"] = &Unit::combat_registry;
	config_1["apply_status_change_in_area"] = &Unit::apply_status_change_in_area;
	config_1["remove_status_change_in_area"] = &Unit::remove_status_change_in_area;
	config_1["execute_skill_in_area"] = &Unit::execute_skill_in_area;
	config_1["execute_skill_in_cell"] = &Unit::execute_skill_in_cell;

	sol::usertype<s_unit_skill_use_notifier_config> config_2 = state->new_usertype<s_unit_skill_use_notifier_config>("s_unit_skill_use_notifier_config");
	config_2["source_guid"] = &s_unit_skill_use_notifier_config::source_guid;
	config_2["target_guid"] = &s_unit_skill_use_notifier_config::target_guid;
	config_2["skill_id"] = &s_unit_skill_use_notifier_config::skill_id;
	config_2["skill_lv"] = &s_unit_skill_use_notifier_config::skill_lv;
	config_2["damage_value"] = &s_unit_skill_use_notifier_config::damage_value;
	config_2["display_value"] = &s_unit_skill_use_notifier_config::display_value;
	config_2["start_time"] = &s_unit_skill_use_notifier_config::start_time;
	config_2["attack_motion"] = &s_unit_skill_use_notifier_config::attack_motion;
	config_2["delay_motion"] = &s_unit_skill_use_notifier_config::delay_motion;
	config_2["number_of_hits"] = &s_unit_skill_use_notifier_config::number_of_hits;
	config_2["action_type"] = &s_unit_skill_use_notifier_config::action_type;
	config_2["item_id"] = &s_unit_skill_use_notifier_config::item_id;
	config_2["target_x"] = &s_unit_skill_use_notifier_config::target_x;
	config_2["target_y"] = &s_unit_skill_use_notifier_config::target_y;
	config_2["element"] = &s_unit_skill_use_notifier_config::element;
	config_2["cast_time"] = &s_unit_skill_use_notifier_config::cast_time;
}

void UnitComponent::sync_functions(std::shared_ptr<sol::state> state)
{
}