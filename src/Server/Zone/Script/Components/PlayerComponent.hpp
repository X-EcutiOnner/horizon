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

#ifndef HORIZON_ZONE_PLAYER_LUA_COMPONENT_HPP
#define HORIZON_ZONE_PLAYER_LUA_COMPONENT_HPP


#include "Server/Zone/Script/Components/LUAComponent.hpp"

namespace Horizon
{
namespace Zone
{
    namespace Units
    {
        class Player;
    }
class GameLogicProcess;
class PlayerComponent : public LUAComponent
{
public:
    PlayerComponent() { }
    PlayerComponent(std::shared_ptr<GameLogicProcess> container) : LUAComponent(container) { }
    ~PlayerComponent() { }
    void sync_definitions(std::shared_ptr<sol::state> state);
    void sync_data_types(std::shared_ptr<sol::state> state);
    void sync_functions(std::shared_ptr<sol::state> state);

    void perform_command_from_player(std::shared_ptr<Units::Player> player, std::string const &cmd);
};
}
}

#endif /* HORIZON_ZONE_PLAYER_LUA_COMPONENT_HPP */
