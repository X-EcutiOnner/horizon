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

#ifndef HORIZON_ZONE_GAME_ENTITIES_MOB_HPP
#define HORIZON_ZONE_GAME_ENTITIES_MOB_HPP

#include "Server/Zone/Game/Units/Unit.hpp"

struct monster_config_data;

namespace Horizon
{
namespace Zone
{
class Map;
namespace Units
{
class Mob : public Unit
{
public:
	Mob(uint64_t uuid, unit_type type, unit_type_mask type_mask, std::shared_ptr<Map> map, MapCoords mcoords);
	~Mob() override;

	bool initialize(std::shared_ptr<const monster_config_data> md);
	virtual bool finalize() override;

	virtual void on_damage_received(std::shared_ptr<Unit> damage_dealer, int damage) override;
	virtual void on_killed(std::shared_ptr<Unit> killer, bool with_drops = false, bool with_exp = false) override;
};
}
}
}

#endif /* HORIZON_ZONE_GAME_ENTITIES_MOB_HPP */
