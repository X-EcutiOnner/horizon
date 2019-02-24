/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2019 Horizon Dev Team.
 *
 * Base Author - Sxyz (sagunxp@gmail.com)
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

#include "Monster.hpp"
#include "Server/Zone/Game/Definitions/EntityDefinitions.hpp"

using namespace Horizon::Zone::Game::Entities;

Monster::Monster(uint32_t guid)
: Creature(guid, ENTITY_MONSTER)
{
	//
}

Monster::Monster(uint32_t guid, MapCoords mcoords)
: Creature(guid, ENTITY_MONSTER, mcoords)
{
	//
}

Monster::Monster(uint32_t guid, MapCoords mcoords, GridCoords gcoords)
: Creature(guid, ENTITY_MONSTER, mcoords, gcoords)
{
	//
}

Monster::~Monster()
{
	//
}
