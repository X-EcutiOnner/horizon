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

#ifndef HORIZON_ZONE_NPC_LUA_COMPONENT_HPP
#define HORIZON_ZONE_NPC_LUA_COMPONENT_HPP

#include "Core/Multithreading/LockedLookupTable.hpp"
#include "Server/Zone/Script/Components/LUAComponent.hpp"
#include "Server/Zone/Definitions/NPCDefinitions.hpp"

namespace Horizon
{
namespace Zone
{
class GameLogicProcess;
class NPCComponent : public LUAComponent
{
public:
    NPCComponent() { }
    NPCComponent(std::shared_ptr<GameLogicProcess> container) : LUAComponent(container) { }
    ~NPCComponent() {}
    
    void sync_definitions(std::shared_ptr<sol::state> state);
    void sync_data_types(std::shared_ptr<sol::state> state);
    void sync_functions(std::shared_ptr<sol::state> state);

    void add_npc_to_db(uint32_t guid, std::shared_ptr<npc_db_data> const &data) { _npc_db.insert(guid, data); }
    std::shared_ptr<npc_db_data> get_npc_from_db(uint32_t guid) { return _npc_db.at(guid); }

    void contact_npc_for_player(std::shared_ptr<Units::Player> player, uint32_t npc_guid);
    void continue_npc_script_for_player(std::shared_ptr<Units::Player> player, uint32_t npc_guid, uint32_t select_idx = 0);

    // void send_dialog_to_player(std::shared_ptr<Units::Player> player, uint32_t npc_guid, std::string const &HLog);
    // void send_next_dialog_to_player(std::shared_ptr<Units::Player> player, uint32_t npc_guid);
    // void send_close_dialog_to_player(std::shared_ptr<Units::Player> player, uint32_t npc_guid);

    LockedLookupTable<uint32_t, std::shared_ptr<npc_db_data>> _npc_db;
};
}
}

#endif /* HORIZON_ZONE_NPC_LUA_COMPONENT_HPP */
