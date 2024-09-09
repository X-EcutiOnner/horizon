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

#ifndef HORIZON_ZONE_GAME_GRIDHOLDER_HPP
#define HORIZON_ZONE_GAME_GRIDHOLDER_HPP

#include "GridDefinitions.hpp"
#include "Server/Zone/Game/Map/Coordinates.hpp"
#include "Server/Zone/Game/Map/Grid/Grid.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainerVisitor.hpp"

//! @brief GridHolder is a container for Grids, handles the visiting of grids.
//! @tparam ZONE_OBJECT_TYPES TypeList of all the objects that can be stored in the grid.
//! @param _bounds The bounds of the GridHolder.
//! @param _grids The Grids contained in the GridHolder.
template<class ZONE_OBJECT_TYPES>
class GridHolder
{
public:
	typedef Grid<ZONE_OBJECT_TYPES> GridType;

	GridHolder(int width, int height)
	: _height(height), _width(width)
	{
	}

	~GridHolder()
	{
	}

	GridType &get_grid(int x, int y)
	{
		assert(x <= _width);
		assert(y <= _height);
		assert(x >= 0 && y >= 0);
		
		return _grids[x][y];
	}

	uint16_t height() { return _height; }
	uint16_t width() { return _width; }

	template<class VISITOR, class CONTAINER_TYPE>
	void visit_all(GridReferenceContainerVisitor<VISITOR, GridReferenceContainer<CONTAINER_TYPE>> &visitor)
	{
		for (uint16_t x = 0; x < _width; ++x) {
			for (uint16_t y = 0; y < _height; ++y) {
				get_grid(x, y).visit(visitor);
			}
		}
	}

	template<class VISITOR, class CONTAINER_TYPE>
	void visit(int x, int y, GridReferenceContainerVisitor<VISITOR, GridReferenceContainer<CONTAINER_TYPE>> &visitor)
	{
		get_grid(x, y).Visit(visitor);
	}

private:
	int _width{ 0 }, _height{ 0 };
	GridType _grids[MAX_GRIDS_PER_MAP][MAX_GRIDS_PER_MAP];
};

#endif /* HORIZON_ZONE_GAME_GRIDHOLDER_HPP */
