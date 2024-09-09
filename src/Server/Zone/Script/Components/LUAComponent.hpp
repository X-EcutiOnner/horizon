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

#ifndef HORIZON_ZONE_LUA_COMPONENT_HPP
#define HORIZON_ZONE_LUA_COMPONENT_HPP

#include <sol/sol.hpp>

namespace Horizon
{
namespace Zone
{
class GameLogicProcess;
class LUAComponent
{
public:
    LUAComponent() { }
    LUAComponent(std::shared_ptr<GameLogicProcess> container) : _map_container_thread(container) { }
    ~LUAComponent() { }

    virtual void sync_definitions(std::shared_ptr<sol::state> state) = 0;
    virtual void sync_data_types(std::shared_ptr<sol::state> state) = 0;
    virtual void sync_functions(std::shared_ptr<sol::state> state) = 0;

    std::shared_ptr<GameLogicProcess> get_container() { return _map_container_thread; }
private:
    std::shared_ptr<GameLogicProcess> _map_container_thread;
};
}
}

#endif /* HORIZON_ZONE_LUA_COMPONENT_HPP */
