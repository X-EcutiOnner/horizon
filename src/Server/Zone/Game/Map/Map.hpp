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

#ifndef HORIZON_ZONE_GAME_MAP_HPP
#define HORIZON_ZONE_GAME_MAP_HPP

#include "Path/AStar.hpp"
#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Game/Map/Grid/Cell/Cell.hpp"
#include "Server/Zone/Game/Map/Grid/GridDefinitions.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainerVisitor.hpp"
#include "Server/Zone/Game/Map/Grid/GridHolder.hpp"

namespace Horizon
{
namespace Zone
{
	namespace Units
	{
		class Item;
	}
	class GameLogicProcess;
//! @brief The class Map is the representation of a map in the game. It contains all the cells and the grid holder. It also contains the A* pathfinder. 
//! It is the main class for the map. It is used to get the cells, the grid holder and the pathfinder, and perform a variety of operations on them.
//! @param _container The container that contains this map.
//! @param _name The name of the map.
//! @param _width The width of the map.
//! @param _height The height of the map.
//! @param _cells The cells of the map.
//! @param _gridholder The grid holder of the map.
//! @param _pathfinder The A* pathfinder of the map.
//! @param _obstructions The obstructions of the map.
class Map : public std::enable_shared_from_this<Map>
{
public:
	Map(std::weak_ptr<GameLogicProcess>, std::string const &, uint16_t, uint16_t, std::vector<uint8_t> const &);
	~Map();

	std::shared_ptr<GameLogicProcess> container() { return _container.expired() == false ? _container.lock() : nullptr; }

	std::string const &get_name() { return _name; }

	int get_area() { return _width * _height; }

	uint16_t get_width() { return _width; }
	uint16_t get_height() { return _height; }

	map_cell_types get_cell_type(MapCoords coords) { return _cells[coords.x()][coords.y()].get_type(); }

	GridHolderType &getGridHolder() { return _gridholder; }

	template <class T>
	bool ensure_grid_for_unit(T *unit, MapCoords coords);

	template<class T, class CONTAINER>
	void visit(int grid_x, int grid_y, GridReferenceContainerVisitor<T, CONTAINER> &visitor);

	template<class T, class CONTAINER>
	void visit(GridCoords const &lower_bound, GridCoords const &upper_bound, GridReferenceContainerVisitor<T, CONTAINER> &visitor);

	template<class T, class CONTAINER>
	void visit_in_range(MapCoords const &map_coords, GridReferenceContainerVisitor<T, CONTAINER> &visitor, uint16_t range = MAX_VIEW_RANGE);

	AStar::Generator &get_pathfinder() { return _pathfinder; }

	bool has_obstruction_at(int16_t x, int16_t y);

	MapCoords get_random_accessible_coordinates()
	{
		int16_t x = 0;
		int16_t y = 0;

		do {
			x = rand() % _width;
			y = rand() % _height;
		} while (has_obstruction_at(x, y));

		return { x, y };
	}

	MapCoords get_random_coordinates_in_walkable_range(uint16_t x, uint16_t y, int16_t min, int16_t max)
	{
		int a = 0, b = 0;
		int r = std::rand();
		int d = r % (max - min + 1) + min;
		int i = 0;

		do {
			a = r % (d * 2 + 1) - d;
			b = r / (d * 2 + 1) % (d * 2 + 1) - d;

			a += x;
			b += y;
			i++;
		} while(has_obstruction_at(a, b) && i < 20);

		if (i == 20) return MapCoords(0, 0);

		return MapCoords(a, b);
	}

	MapCoords get_random_coordinates_in_walkable_area(uint16_t x, uint16_t y, int16_t xs, int16_t ys)
	{
		std::vector<MapCoords> available_cells;

		assert(xs >= 0);
		assert(ys >= 0);

		for (int i = std::max(x - xs, 0); i < std::min(x + xs, (int) _width); i++) {
			for (int j = std::max(y - ys, 0); j < std::min(y + ys,(int) _height); j++) {
				if (i == x && j == y)
					continue;
				if (!has_obstruction_at(i, j))
					available_cells.push_back(MapCoords(i, j));
			}
		}

		if (available_cells.size() == 0)
			return MapCoords(0, 0);

		int rnd = rand() % available_cells.size();

		return available_cells.at(rnd);
	}
	
	void set_user_count(int32_t count) { _user_count = count; }
	int32_t get_user_count() { return _user_count; }

	void add_user_count();
	void sub_user_count();

	void add_item_drop(int item_id, MapCoords map_coords, int amount, int identified);
	void add_item_drop(std::shared_ptr<item_entry_data> entry, int32_t amount, MapCoords map_coords);
private:
	std::weak_ptr<GameLogicProcess> _container;
	std::string _name{""};
	uint16_t _width{0}, _height{0};
	GridCoords _max_grids;
	Cell _cells[MAX_CELLS_PER_MAP][MAX_CELLS_PER_MAP]{{0}};
	GridHolderType _gridholder;
	AStar::Generator _pathfinder;
	int32_t _user_count{ 0 };
};
}
}

template <class T>
bool Horizon::Zone::Map::ensure_grid_for_unit(T *unit, MapCoords mcoords)
{
	std::string const &new_map_name = unit->map()->get_name();
	GridCoords new_gcoords = mcoords.scale<MAX_CELLS_PER_GRID, MAX_GRIDS_PER_MAP>();

	if (new_map_name.compare(get_name()) == 0 && unit->grid_coords() == new_gcoords)
		return false;

	if (unit->has_valid_grid_reference())
		unit->remove_grid_reference();

	unit->set_grid_coords(new_gcoords);

	_gridholder.get_grid(new_gcoords.x(), new_gcoords.y()).add_object(unit);

	return true;
}

template<class T, class CONTAINER>
inline void Horizon::Zone::Map::visit(int grid_x, int grid_y, GridReferenceContainerVisitor<T, CONTAINER> &visitor)
{
	Grid<AllUnitTypes> &g = _gridholder.get_grid(grid_x, grid_y);
	g.visit(visitor);
}

template<class T, class CONTAINER>
inline void Horizon::Zone::Map::visit_in_range(MapCoords const &map_coords, GridReferenceContainerVisitor<T, CONTAINER> &visitor, uint16_t range)
{
	MapCoords lower_bounds = map_coords.at_range<MAX_CELLS_PER_MAP>(-range);
	MapCoords upper_bounds = map_coords.at_range<MAX_CELLS_PER_MAP>(range);

	visit(lower_bounds.scale<MAX_CELLS_PER_GRID, MAX_GRIDS_PER_MAP>(), upper_bounds.scale<MAX_CELLS_PER_GRID, MAX_GRIDS_PER_MAP>(), visitor);
}

template<class T, class CONTAINER>
inline void Horizon::Zone::Map::visit(GridCoords const &lower_bound, GridCoords const &upper_bound, GridReferenceContainerVisitor<T, CONTAINER> &visitor)
{
	// Visit all grids in the map, from top to bottom, left to right
	// This is done to ensure that the visitor is called in the same order as the grids are stored in the map
	// This is important for the GridReferenceContainerVisitor, which will add the grid references to a container
	// The container will then be iterated over in the same order as the grids were visited
	// This ensures that the entities are visited in the container in the same order as they are stored in the map
	// We visit all ranges - 0,0 to maximum grids in a map 
	// (the upper range as well, i.e. if the map is 10x10 grids, we visit 0,0 to 10,10 and not 9x9)
	for (int y = upper_bound.y(); y >= lower_bound.y(); y--) {
		for (int x = lower_bound.x(); x <= upper_bound.x(); x++) {
			visit(x, y, visitor);
		}
	}
}

#endif /* HORIZON_ZONE_GAME_MAP_HPP */
