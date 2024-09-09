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

#include "MapComponent.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/Map/Coordinates.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

void MapComponent::sync_definitions(std::shared_ptr<sol::state> state)
{
}

void MapComponent::sync_data_types(std::shared_ptr<sol::state> state)
{
	sol::usertype<GridCoords> config_1 = state->new_usertype<GridCoords>("GridCoords", sol::constructors<GridCoords(uint16_t, uint16_t)>());
	config_1["x"] = &GridCoords::x;
	config_1["y"] = &GridCoords::y;
	config_1["move_cost"] = &GridCoords::move_cost;
	config_1["set_move_cost"] = &GridCoords::set_move_cost;
	

	sol::usertype<MapCoords> config_2 = state->new_usertype<MapCoords>("MapCoords",
		sol::constructors<MapCoords(uint16_t, uint16_t)>());
	config_2["x"] = &MapCoords::x;
	config_2["y"] = &MapCoords::y;
	config_2["move_cost"] = &MapCoords::move_cost;
	config_2["set_move_cost"] = &MapCoords::set_move_cost;

	sol::usertype<Map> config_3 = state->new_usertype<Map>("Map");
	config_3["name"] = sol::readonly_property(&Map::get_name);
	config_3["width"] = sol::readonly_property(&Map::get_width);
	config_3["height"] = sol::readonly_property(&Map::get_height);
	config_3["has_obstruction_at"] = &Map::has_obstruction_at;
	config_3["get_random_coordinates_in_walkable_range"] = &Map::get_random_coordinates_in_walkable_range;
	config_3["get_random_coordinates_in_walkable_area"] = &Map::get_random_coordinates_in_walkable_area;
	config_3["add_new_item_drop"] = sol::resolve<void(int, MapCoords, int, int)>(&Map::add_item_drop);
	config_3["add_unique_item_drop"] = sol::resolve<void(std::shared_ptr<item_entry_data>, int32_t, MapCoords)>(&Map::add_item_drop);
}

void MapComponent::sync_functions(std::shared_ptr<sol::state> state)
{
	// @TODO
	state->set_function("get_map_by_name",
		[] (std::string const &name)
		{
			int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, name, nullptr);

			if (segment_number == 0)
				return std::shared_ptr<Map>(nullptr);
			
			auto resource_manager = sZone->get_component_of_type<GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number)->get_resource_manager();
			return resource_manager.template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(name, nullptr);
		});
}