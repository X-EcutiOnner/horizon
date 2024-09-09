/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
       \_| |_/\___/|_|  |_/___\___/|_| |_|        *
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

#include "Mob.hpp"

#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Game/Units/Mob/Mob.hpp"
#include "Server/Zone/Definitions/MonsterDefinitions.hpp"

using namespace Horizon::Zone::Units;

Mob::Mob(uint64_t uuid, unit_type type, unit_type_mask type_mask, std::shared_ptr<Map> map, MapCoords mcoords)
: Unit(uuid, type, type_mask, map, mcoords)
{
	//
}

Mob::~Mob()
{
}

bool Mob::initialize(std::shared_ptr<const monster_config_data> md)
{
	if (Unit::initialize() == false)
		return false;

	if (status()->initialize(shared_from_this()->downcast<Mob>(), md) == false) {
		HLog(error) << "Status::initialize: failed to initialize status for creature with monster id " << md->monster_id << ".";
		return false;
	}

	return true;
}

bool Mob::finalize()
{
	if (!Unit::finalize())
		return false;

	return true;
}

void Mob::on_damage_received(std::shared_ptr<Unit> damage_dealer, int damage)
{
	Unit::on_damage_received(damage_dealer, damage);
}

void Mob::on_killed(std::shared_ptr<Unit> killer, bool with_drops, bool with_exp)
{
	Unit::on_killed(killer, with_drops, with_exp);
}