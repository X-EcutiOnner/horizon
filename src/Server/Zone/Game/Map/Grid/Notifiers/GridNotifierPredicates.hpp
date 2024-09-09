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

#ifndef HORIZON_ZONE_GAME_MAP_GRIDNOTIFIERPREDICATES_HPP
#define HORIZON_ZONE_GAME_MAP_GRIDNOTIFIERPREDICATES_HPP

#include "Server/Zone/Game/Units/Unit.hpp"
#include "Server/Zone/Game/Map/Grid/GridDefinitions.hpp"

class GUIDCheckPredicate
{
public:
	GUIDCheckPredicate(uint32_t guid) : _guid(guid) { }

	bool operator()(std::weak_ptr<Horizon::Zone::Unit> unit)
	{
		return !unit.expired() && ((unit.lock())->guid() == _guid);
	}

private:
	uint32_t _guid;
};

class RangeCheckPredicate
{
public:
	RangeCheckPredicate(std::weak_ptr<Horizon::Zone::Unit> source)
	: _source(source){ }

	bool operator()(std::weak_ptr<Horizon::Zone::Unit> target, uint16_t range = MAX_VIEW_RANGE)
	{
		return !_source.expired() && !target.expired() && _source.lock()->is_in_range_of(target.lock(), range);
	}

private:
	std::weak_ptr<Horizon::Zone::Unit> _source;
};

class AOETargetTypePredicate
{
public:
	AOETargetTypePredicate(int aoe_target_mask)
	: _aoe_target_mask(aoe_target_mask){ }

	bool operator()(std::weak_ptr<Horizon::Zone::Unit> target)
	{
		return !target.expired() && (target.lock())->is_of_type(_aoe_target_mask);
	}

private:
	int _aoe_target_mask{ 0 };
};

class CellCheckPredicate
{
public:
	CellCheckPredicate(MapCoords cell)
	: _cell(cell){ }

	bool operator()(MapCoords cell)
	{
		return cell == _cell;
	}

private:
	MapCoords _cell{ 0, 0 };
};

#endif /* GRIDNOTIFIERPREDICATES_HPP */
