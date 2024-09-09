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

#ifndef HORIZON_ZONE_GAME_ENTITIES_ITEM_HPP
#define HORIZON_ZONE_GAME_ENTITIES_ITEM_HPP

#include "Server/Zone/Game/GridObject.hpp"
#include "Server/Zone/Game/Map/Grid/GridDefinitions.hpp"
#include "Server/Zone/Game/Units/Unit.hpp"
#include "Server/Zone/Game/StaticDB/ItemDB.hpp"

namespace Horizon
{
namespace Zone
{
class Map;
namespace Units
{
class Item : public Unit, public GridObject<Item>
{
public:
    explicit Item(int64_t uuid, std::shared_ptr<item_entry_data> entry, int32_t amount, std::shared_ptr<Map> map, MapCoords mcoords);
	explicit Item(int64_t uuid, std::shared_ptr<Map> map, MapCoords mcoords, int is_identified, int amount, std::shared_ptr<const item_config_data> item_d);
	~Item();

    void initialize();
    bool finalize() override;

    std::shared_ptr<item_entry_data> entry() { return _entry; }
    std::shared_ptr<const item_config_data> config() const { return _config; }
    
    int is_identified() const { return _is_identified; }
    int amount() const { return _amount; }
    
    int x_area() const { return _x_area; }
    int y_area() const { return _y_area; }

    void set_x_area(int x) { _x_area = x; }
    void set_y_area(int y) { _y_area = y; }

    void stop_movement() override;
    void on_pathfinding_failure() override;
    void on_movement_begin() override;
    void on_movement_step() override;
    void on_movement_end() override;
    
	virtual void on_status_effect_start(std::shared_ptr<status_change_entry> sce) override;
	virtual void on_status_effect_end(std::shared_ptr<status_change_entry> sce) override;
	virtual void on_status_effect_change(std::shared_ptr<status_change_entry> sce) override;
    
private:
    std::shared_ptr<item_entry_data> _entry{ nullptr };
    std::shared_ptr<const item_config_data> _config{ nullptr };
	int _is_identified{ 0 };
    int _amount{ 0 };
    int _x_area{ 0 }, _y_area{ 0 };
};
}
}
}

#endif //HORIZON_ZONE_GAME_ENTITIES_ITEM_HPP