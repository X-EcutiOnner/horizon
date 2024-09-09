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

#ifndef HORIZON_ZONE_COMBAT_DEFINITIONS_HPP
#define HORIZON_ZONE_COMBAT_DEFINITIONS_HPP

#include "UnitDefinitions.hpp"
#include <sol/sol.hpp>

enum combat_damage_type_mask {
	// Flag of the final calculation
	CBT_DMGMASK_NONE       = 0x0000,
	CBT_DMGMASK_WEAPON     = 0x0001,
	CBT_DMGMASK_MAGIC      = 0x0002,
	CBT_DMGMASK_MISC       = 0x0004,
	CBT_DMGMASK_SHORT      = 0x0010,
	CBT_DMGMASK_LONG       = 0x0040,
	CBT_DMGMASK_SKILL      = 0x0100,
	CBT_DMGMASK_NORMAL     = 0x0200,
	CBT_DMGMASK_WEAPONMASK = 0x000f,
	CBT_DMGMASK_RANGEMASK  = 0x00f0,
	CBT_DMGMASK_SKILLMASK  = 0x0f00,
};

// state of a single attack attempt; used in flee/def penalty calculations when mobbed
enum combat_retaliate_type {
	CBT_RET_NONE,    // not an attack
	CBT_RET_LUCKY,   // attack was lucky-dodged
	CBT_RET_FLEE,    // attack was dodged
	CBT_RET_MISS,    // attack missed because of element/race modifier.
	CBT_RET_BLOCK,   // attack was blocked by some skills.
	CBT_RET_DEF      // attack connected
};

enum combat_target_mask { //New definitions [Skotlex]
	CBT_TMASK_NOONE       =   0x000000,
	CBT_TMASK_SELF        =   0x010000,
	CBT_TMASK_ENEMY       =   0x020000,
	CBT_TMASK_PARTY       =   0x040000,
	CBT_TMASK_GUILDALLY   =   0x080000, ///< Only allies, NOT guildmates
	CBT_TMASK_NEUTRAL     =   0x100000,
	CBT_TMASK_SAMEGUILD   =   0x200000, ///< No Guild Allies

	CBT_TMASK_GUILD       =   0x280000, ///< Guild AND allies (BCT_SAMEGUILD|BCT_GUILDALLY)

	CBT_TMASK_NOGUILD     =   0x170000, ///< This must be (~BCT_GUILD&BCT_ALL)
	CBT_TMASK_NOPARTY     =   0x3b0000, ///< This must be (~BCT_PARTY&BCT_ALL)
	CBT_TMASK_NOENEMY     =   0x3d0000, ///< This must be (~BCT_ENEMY&BCT_ALL)

	CBT_TMASK_ALL         =   0x3f0000, ///< Sum of BCT_NOONE to BCT_SAMEGUILD
};

/**
 * Values used by (struct Damage).type, as well as clif->damage(type) and clif->skill_damage(type)
 *
 * Note: some values may not apply in some contexts.
 */
enum combat_damage_hit_type {
	CBT_DMG_HIT_NORMAL      = 0,  // Normal attack
	//CBT_DMG_HIT_PICKUP    = 1,  // Pick up item
	//CBT_DMG_HIT_SITDOWN   = 2,  // Sit down
	//CBT_DMG_HIT_STANDUP   = 3,  // Stand up
	CBT_DMG_HIT_ENDURE      = 4,  // Damage (endure)
	CBT_DMG_HIT_SPLASH      = 5,  // Splash
	CBT_DMG_HIT_SKILL       = 6,  // Skill
	//CBT_DMG_HIT_REPEAT    = 7,  // (repeat damage?)
	CBT_DMG_HIT_MULTIHIT    = 8,  // Multi-hit damage
	CBT_DMG_HIT_MULTIENDURE = 9,  // Multi-hit damage (endure)
	CBT_DMG_HIT_CRIT        = 10, // Critical hit
	CBT_DMG_HIT_PDODGE      = 11, // Lucky dodge
	//CBT_DMG_HIT_TOUCH     = 12, // (touch skill?)
};

struct combat_structures
{
	int16_t skill_id{ 0 };
	int16_t skill_lv{ 0 };
	sol::table skill_cast_data;
};

struct combat_damage : public combat_structures {
    int64_t right_damage{0}, left_damage{0};
    combat_damage_hit_type type{CBT_DMG_HIT_NORMAL};
    int number_of_hits{0};
    int amotion{0}, dmotion{0};
    int blewcount{0};
    combat_damage_type_mask damage_type{CBT_DMGMASK_NONE};
    combat_retaliate_type ret_type{CBT_RET_NONE};
    element_type ele_type{ELE_NEUTRAL};
    int8_t element_level{1};
};

struct combat_healing : public combat_structures  {
	int64_t heal_amount{ 0 };
	bool success{ 0 };
};

#endif /* HORIZON_ZONE_COMBAT_DEFINITIONS_HPP */