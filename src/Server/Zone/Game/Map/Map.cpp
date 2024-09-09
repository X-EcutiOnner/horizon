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
 * Base Author - Sxyz <sagunxp@gmail.com>
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

#include "Map.hpp"
#include "Utility/Utility.hpp"
#include "Core/Multithreading/WorkerThreadPool.hpp"
#include "Server/Zone/Game/Map/Grid/Notifiers/GridNotifiers.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainer.hpp"
#include "Server/Zone/Game/Map/Grid/Container/GridReferenceContainerVisitor.hpp"
#include "Server/Zone/Script/Components/MonsterComponent.hpp"
#include "Server/Zone/Game/Units/Item/Item.hpp"
#include "Server/Zone/Game/Map/Grid/Grid.hpp"
#include "Server/Zone/Game/StaticDB/ItemDB.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

Map::Map(std::weak_ptr<GameLogicProcess> container, std::string const &name, uint16_t width, uint16_t height, std::vector<uint8_t> const &cells)
: _container(container), _name(name), _width(width), _height(height),
  _max_grids((width / MAX_CELLS_PER_GRID), (height / MAX_CELLS_PER_GRID)),
  _gridholder((width / MAX_CELLS_PER_GRID) + 1, (height / MAX_CELLS_PER_GRID) + 1), // add 1 because 0,0 is included.
  _pathfinder(AStar::Generator(MapCoords(width, height), std::bind(&Map::has_obstruction_at, this, std::placeholders::_1, std::placeholders::_2), true, &AStar::Heuristic::manhattan))
{
	for (int y = height - 1; y >= 0; --y) {
		for (int x = 0; x < width; ++x) {
			_cells[x][y] = Cell(cells.at(y * width + x));
		}
	}
}

Map::~Map()
{
	for (int y = _height - 1; y >= 0; --y) {
		for (int x = 0; x < _width; ++x) {
			_cells[x][y].~Cell();
		}
	}
}

bool Map::has_obstruction_at(int16_t x, int16_t y)
{
	if (x < 0 || y < 0 || x > _width || y > _height)
		return true;

	Cell c = _cells[x][y];
	
	if (!c.isWalkable())
		return true;

	return false;
}

void Map::add_user_count() {
	_user_count++; 
	if (get_user_count() == 1)
		container()->get_monster_spawn_agent().spawn_monsters(get_name());
	HLog(debug) << "Map::add_user_count: " << get_user_count();
}

void Map::sub_user_count() { 
	_user_count--; 
	if (get_user_count() == 0)
		container()->get_monster_spawn_agent().despawn_monsters(get_name());
	HLog(debug) << "Map::sub_user_count: " << get_user_count();
}


void Map::add_item_drop(int item_id, MapCoords map_coords, int amount, int identified)
{
	std::shared_ptr<const item_config_data> item_d = ItemDB->get_item_by_id(item_id);

	if (item_d == nullptr) {
		HLog(warning) << "Map::add_item_drop: Tried to drop non-existent item with ID: " << item_id << ".";
		return;
	}

	int r = std::rand();
	
	int64_t uuid = sZone->to_uuid(UNIT_ITEM, ++_last_np_unit_guid, 0, 0);
	std::shared_ptr<Horizon::Zone::Units::Item> item = std::make_shared<Horizon::Zone::Units::Item>(uuid, shared_from_this(), map_coords, identified, amount, item_d);
	item->initialize();

	s_grid_notify_item_drop_entry entry;
	entry.guid = item->guid();
	entry.item_id = item->config()->item_id;
	entry.type = item->config()->type;
	entry.is_identified = identified;
	entry.x = item->map_coords().x();
	entry.y = item->map_coords().y();
	entry.amount = amount;
	item->set_x_area((r & 3) * 3 + 3);
	item->set_y_area(((r >> 2) & 3) * 3 + 3);
	entry.x_area = item->x_area();
	entry.y_area = item->y_area();
	entry.show_drop_effect = item->config()->config.show_drop_effect;
	entry.drop_effect_mode = item->config()->drop_effect_mode;
	item->notify_nearby_players_of_item_drop(entry);

	this->container()->get_resource_manager().add<RESOURCE_PRIORITY_TERTIARY>(item->uuid(), item);
}

void Map::add_item_drop(std::shared_ptr<item_entry_data> entry, int32_t amount, MapCoords map_coords)
{	
	int64_t uuid = sZone->to_uuid(UNIT_ITEM, ++_last_np_unit_guid, 0, 0);
	std::shared_ptr<Horizon::Zone::Units::Item> floor_item = std::make_shared<Horizon::Zone::Units::Item>(uuid, entry, amount, shared_from_this(), map_coords);
	floor_item->initialize();

	s_grid_notify_item_drop_entry notif_config;

	int r = std::rand();

	notif_config.guid = floor_item->guid();
	notif_config.item_id = floor_item->config()->item_id;
	notif_config.type = floor_item->config()->type;
	notif_config.is_identified = floor_item->is_identified();
	notif_config.x = floor_item->map_coords().x();
	notif_config.y = floor_item->map_coords().y();
	notif_config.amount = amount;
	floor_item->set_x_area((r & 3) * 3 + 3);
	floor_item->set_y_area(((r >> 2) & 3) * 3 + 3);
	notif_config.x_area = floor_item->x_area();
	notif_config.y_area = floor_item->y_area();
	notif_config.show_drop_effect = floor_item->config()->config.show_drop_effect;
	notif_config.drop_effect_mode = floor_item->config()->drop_effect_mode;

	floor_item->notify_nearby_players_of_item_drop(notif_config);

	this->container()->get_resource_manager().add<RESOURCE_PRIORITY_TERTIARY>(floor_item->uuid(), floor_item);
}