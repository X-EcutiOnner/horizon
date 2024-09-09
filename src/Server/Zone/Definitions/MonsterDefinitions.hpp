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

#ifndef HORIZON_ZONE_MONSTER_DEFINITIONS_HPP
#define HORIZON_ZONE_MONSTER_DEFINITIONS_HPP

#include "Server/Zone/Game/Map/Grid/GridDefinitions.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include <cinttypes>
#include <string>
#include <unordered_map>

#define MAX_MOB_DROP 10
#define MAX_MVP_DROP 10
#define MAX_MOB_DB 22000

enum monster_boss_type {
	BOSS_TYPE_NONE = 0,
	BOSS_TYPE_MINI = 1,
	BOSS_TYPE_MVP = 2,
};

enum monster_primary_race_type {
	// Base Races
	MONSTER_RACE_FORMLESS = 0,  ///< Formless
	MONSTER_RACE_UNDEAD,        ///< Undead
	MONSTER_RACE_BRUTE,         ///< Beast/Brute
	MONSTER_RACE_PLANT,         ///< Plant
	MONSTER_RACE_INSECT,        ///< Insect
	MONSTER_RACE_FISH,          ///< Fish
	MONSTER_RACE_DEMON,         ///< Demon
	MONSTER_RACE_DEMIHUMAN,     ///< Demi-Human (not including Player)
	MONSTER_RACE_ANGEL,         ///< Angel
	MONSTER_RACE_DRAGON,        ///< Dragon
	MONSTER_RACE_PLAYER,        ///< Player
	// Boss
	MONSTER_RACE_BOSS,          ///< Boss
	MONSTER_RACE_NONBOSS,       ///< Non-boss

	MONSTER_RACE_MAX,           // Array size delimiter (keep before the combination races)

	// Combination Races
	MONSTER_RACE_NONDEMIHUMAN,   ///< Every race except Demi-Human (including Player)
	MONSTER_RACE_NONPLAYER,      ///< Every non-player race
	MONSTER_RACE_DEMIPLAYER,     ///< Demi-Human (including Player)
	MONSTER_RACE_NONDEMIPLAYER,  ///< Every race except Demi-Human (and except Player)
	MONSTER_RACE_ALL = 0xFF,     ///< Every race (implemented as equivalent to RC_BOSS and RC_NONBOSS)
};

enum monster_secondary_race_type {
	MONSTER_RACE2_NONE       = 0,
	MONSTER_RACE2_GOBLIN     = 1,
	MONSTER_RACE2_KOBOLD     = 2,
	MONSTER_RACE2_ORC        = 3,
	MONSTER_RACE2_GOLEM      = 4,
	MONSTER_RACE2_GUARDIAN   = 5,
	MONSTER_RACE2_NINJA      = 6,
	MONSTER_RACE2_SCARABA    = 7,
	MONSTER_RACE2_TURTLE     = 8,
	MONSTER_RACE2_MAX
};

enum monster_mode_type
{
	MONSTER_MODE_MASK_NONE               = 0x00000000,
	MONSTER_MODE_MASK_CANMOVE            = 0x00000001,
	MONSTER_MODE_MASK_LOOTER             = 0x00000002,
	MONSTER_MODE_MASK_AGGRESSIVE         = 0x00000004,
	MONSTER_MODE_MASK_ASSIST             = 0x00000008,
	MONSTER_MODE_MASK_CASTSENSOR_IDLE    = 0x00000010,
	MONSTER_MODE_MASK_BOSS               = 0x00000020,
	MONSTER_MODE_MASK_PLANT              = 0x00000040,
	MONSTER_MODE_MASK_CANATTACK          = 0x00000080,
	MONSTER_MODE_MASK_DETECTOR           = 0x00000100,
	MONSTER_MODE_MASK_CASTSENSOR_CHASE   = 0x00000200,
	MONSTER_MODE_MASK_CHANGECHASE        = 0x00000400,
	MONSTER_MODE_MASK_ANGRY              = 0x00000800,
	MONSTER_MODE_MASK_CHANGETARGET_MELEE = 0x00001000,
	MONSTER_MODE_MASK_CHANGETARGET_CHASE = 0x00002000,
	MONSTER_MODE_MASK_TARGETWEAK         = 0x00004000,
	MONSTER_MODE_MASK_NOKNOCKBACK        = 0x00008000,
	//MD_RANDOMTARGET     = 0x00010000, // Not implemented
	// Note: This should be kept within INT_MAX, since it's often cast to int.
	MONSTER_MODE_MASK_ALL               = 0x7FFFFFFF,
};

enum monster_skill_state_type {
	MONSTER_SKILL_STATE_ANY = -1,
	MONSTER_SKILL_STATE_IDLE,
	MONSTER_SKILL_STATE_WALK,
	MONSTER_SKILL_STATE_LOOT,
	MONSTER_SKILL_STATE_DEAD,
	MONSTER_SKILL_STATE_BERSERK, //Aggressive mob attacking
	MONSTER_SKILL_STATE_ANGRY,   //Mob retaliating from being attacked.
	MONSTER_SKILL_STATE_RUSH,    //Mob following a player after being attacked.
	MONSTER_SKILL_STATE_FOLLOW,  //Mob following a player without being attacked.
	MONSTER_SKILL_STATE_ANYTARGET,
};

enum monster_skill_target_type {
	MONSTER_SKILL_TARGET_CURRENT = 0,
	MONSTER_SKILL_TARGET_RANDOM, //Random Target!
	MONSTER_SKILL_TARGET_SELF,
	MONSTER_SKILL_TARGET_FRIEND,
	MONSTER_SKILL_TARGET_MASTER,
	MONSTER_SKILL_TARGET_AROUND5,
	MONSTER_SKILL_TARGET_AROUND6,
	MONSTER_SKILL_TARGET_AROUND7,
	MONSTER_SKILL_TARGET_AROUND8,
	MONSTER_SKILL_TARGET_AROUND1,
	MONSTER_SKILL_TARGET_AROUND2,
	MONSTER_SKILL_TARGET_AROUND3,
	MONSTER_SKILL_TARGET_AROUND4,
	MONSTER_SKILL_TARGET_AROUND = MONSTER_SKILL_TARGET_AROUND4,
};

enum monster_skill_cast_condition_type {
	MONSTER_SKILL_CC_ALWAYS = 0x0000,
	MONSTER_SKILL_CC_MYHPLTMAXRATE,
	MONSTER_SKILL_CC_MYHPINRATE,
	MONSTER_SKILL_CC_FRIENDHPLTMAXRATE,
	MONSTER_SKILL_CC_FRIENDHPINRATE,
	MONSTER_SKILL_CC_MYSTATUSON,
	MONSTER_SKILL_CC_MYSTATUSOFF,
	MONSTER_SKILL_CC_FRIENDSTATUSON,
	MONSTER_SKILL_CC_FRIENDSTATUSOFF,
	MONSTER_SKILL_CC_ATTACKPCGT,
	MONSTER_SKILL_CC_ATTACKPCGE,
	MONSTER_SKILL_CC_SLAVELT,
	MONSTER_SKILL_CC_SLAVELE,
	MONSTER_SKILL_CC_CLOSERANGEATTACKED,
	MONSTER_SKILL_CC_LONGRANGEATTACKED,
	MONSTER_SKILL_CC_AFTERSKILL,
	MONSTER_SKILL_CC_SKILLUSED,
	MONSTER_SKILL_CC_CASTTARGETED,
	MONSTER_SKILL_CC_RUDEATTACKED,
	MONSTER_SKILL_CC_MASTERHPLTMAXRATE,
	MONSTER_SKILL_CC_MASTERATTACKED,
	MONSTER_SKILL_CC_ALCHEMIST,
	MONSTER_SKILL_CC_SPAWN,
	MONSTER_SKILL_CC_MAGICATTACKED,
};

struct monster_spawn_data {
	int spawn_dataset_id{ 0 };
	int monster_id{0};
	std::string map_name{""};
	MapCoords coords{0,0};
	int16_t x{0}, y{0}, x_area{0}, y_area{0};
	std::string mob_name{""};
	int16_t amount{0};
	int32_t spawn_delay_base{0}, spawn_delay_variance{0};
	int16_t dead_amount{ 0 };
	struct s_monster_spawn_time_cache {
		int64_t dead_time{ 0 };
		int64_t spawn_time{ 0 };
	};
	std::unordered_map<int64_t, s_monster_spawn_time_cache> dead_spawn_time_list;
};

struct monster_skill_config_data {
	uint16_t skill_id{0};
	uint16_t skill_level{0};
	int16_t skill_invoke_rate{0};
	int cast_time{0};
	int delay{0};
	bool cancelable{0};
	int16_t cast_condition{0};
	int32_t condition_data{0};
	int val[5]{0};
	int16_t emotion_id{0};
	uint16_t msg_id{0};
	enum monster_skill_target_type target{MONSTER_SKILL_TARGET_CURRENT};
	enum monster_skill_state_type state{MONSTER_SKILL_STATE_ANY};
};

struct monster_config_data
{
	uint16_t monster_id{0};
	char sprite_name[MAX_UNIT_NAME_LENGTH]{0};
	char name[MAX_UNIT_NAME_LENGTH]{0};
	char alt_name[MAX_UNIT_NAME_LENGTH]{0};

	uint16_t level{1};
	int32_t hp{0};
	int32_t sp{0};
	int32_t attack_range{0};
	int32_t attack_damage[2]{0};
	int32_t defense{0};
	int32_t magic_defense{0};

	struct {
		int16_t str{0};
		int16_t agi{0};
		int16_t vit{0};
		int16_t int_{0};
		int16_t dex{0};
		int16_t luk{0};
	} stats;

	short view_range{1};
	short chase_range{1};
	unit_size_type size{ESZ_MEDIUM};
	monster_primary_race_type primary_race{MONSTER_RACE_FORMLESS};
	monster_secondary_race_type secondary_race{ MONSTER_RACE2_NONE };
	element_type element{ELE_NEUTRAL};
	int8_t element_level{0};
	int32_t mode{0};
	int32_t move_speed{0};
	int32_t attack_delay{0};
	int32_t attack_motion{0};
	int32_t damage_motion{0};
	int32_t damage_taken_rate{100};

	struct monster_view_data {
		int32_t sprite_id{0};
		int32_t weapon_id{0};
		int32_t shield_id{0};
		int32_t robe_id{0};
		int32_t headgear_top_id{0};
		int32_t headgear_middle_id{0};
		int32_t headgear_bottom_id{0};
		int32_t hair_style_id{0};
		int32_t body_style_id{0};
		int32_t hair_color_id{0};
		int32_t body_color_id{0};
		unit_gender_types gender{UNIT_GENDER_FEMALE};
	} view{0};
	//struct view_data vd;
	unsigned int option;
	//struct mob_skill skill[MAX_MOBSKILL];
	//struct spawn_info spawn[10];
	struct rewards
	{
		struct drops { int item_id{0}, chance{0}; };

		unsigned int base_exp{0}, job_exp{0};
		unsigned int mvp_exp{0};
		struct drops items[MAX_MOB_DROP]{0};
		struct drops items_mvp[MAX_MVP_DROP]{0};
	} rewards;
};

#endif /* HORIZON_ZONE_MONSTER_DEFINITIONS_HPP */
