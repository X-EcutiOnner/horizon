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

#include "Item.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone::Units;

Item::Item(int64_t uuid, std::shared_ptr<item_entry_data> entry, int32_t amount, std::shared_ptr<Map> map, MapCoords mcoords)
    : Unit(uuid, UNIT_ITEM, UNIT_MASK_ITEM, map, mcoords), _entry(entry), _config(entry->config), _amount(amount),
    _is_identified(entry->info.is_identified)
{
    _entry->amount = amount;
}

Item::Item(int64_t uuid, std::shared_ptr<Map> map, MapCoords mcoords, int is_identified, int amount, std::shared_ptr<const item_config_data> item_d)
    : Unit(uuid, UNIT_ITEM, UNIT_MASK_ITEM, map, mcoords), _entry(nullptr), _is_identified(is_identified), _amount(amount), _config(item_d)
{
}

Item::~Item()
{

}

void Item::initialize()
{
    if (!Unit::initialize())
        return;

    map()->ensure_grid_for_unit(this, map_coords());
}

bool Item::finalize()
{
    return true;
}

void Item::stop_movement()
{

}
void Item::on_pathfinding_failure() 
{
    
}
void Item::on_movement_begin() 
{
    
}
void Item::on_movement_step() 
{
    
}
void Item::on_movement_end() 
{
    
}

void Item::on_status_effect_start(std::shared_ptr<status_change_entry> sce) 
{
    
}
void Item::on_status_effect_end(std::shared_ptr<status_change_entry> sce) 
{
    
}
void Item::on_status_effect_change(std::shared_ptr<status_change_entry> sce) 
{
    
}