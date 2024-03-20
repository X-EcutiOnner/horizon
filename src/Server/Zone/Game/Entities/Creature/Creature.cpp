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

#include "Creature.hpp"

#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/Entities/Traits/Status.hpp"
#include "Server/Zone/Game/Entities/Creature/Creature.hpp"
#include "Server/Zone/Definitions/MonsterDefinitions.hpp"

using namespace Horizon::Zone::Entities;

Creature::Creature(uint64_t uuid, entity_type type, entity_type_mask type_mask, std::shared_ptr<Map> map, MapCoords mcoords)
: Entity(uuid, type, type_mask, map, mcoords)
{
	//
}

Creature::~Creature()
{
}

bool Creature::initialize(std::shared_ptr<const monster_config_data> md)
{
	if (Entity::initialize() == false)
		return false;

	if (status()->initialize(shared_from_this()->downcast<Creature>(), md) == false) {
		HLog(error) << "Status::initialize: failed to initialize status for creature with monster id " << md->monster_id << ".";
		return false;
	}

	return true;
}

bool Creature::finalize()
{
	if (!Entity::finalize())
		return false;

	return true;
}

void Creature::on_damage_received(std::shared_ptr<Entity> damage_dealer, int damage)
{
	Entity::on_damage_received(damage_dealer, damage);
}

void Creature::on_killed(std::shared_ptr<Entity> killer, bool with_drops, bool with_exp)
{
	Entity::on_killed(killer, with_drops, with_exp);
}