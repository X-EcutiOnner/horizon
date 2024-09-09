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

#include "PlayerComponent.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Definitions/PlayerDefinitions.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Player/Assets/Inventory.hpp"
#include "Server/Zone/Game/Units/Player/Assets/Storage.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

void PlayerComponent::sync_definitions(std::shared_ptr<sol::state> state)
{
    state->create_named_table("PlayerActionType",
        "Attack", PLAYER_ACT_ATTACK,
        "ItemPickup", PLAYER_ACT_ITEM_PICKUP,
        "Sit", PLAYER_ACT_SIT,
        "Stand", PLAYER_ACT_STAND,
        "AttackNoMotion", PLAYER_ACT_ATTACK_NOMOTION,
        "Splash", PLAYER_ACT_SPLASH,
        "Skill", PLAYER_ACT_SKILL,
        "AttackRepeat", PLAYER_ACT_ATTACK_REPEAT,
        "AttackMultiple", PLAYER_ACT_ATTACK_MULTIPLE,
        "AttackMultipleNoMotion", PLAYER_ACT_ATTACK_MULTIPLE_NOMOTION,
        "AttackCritical", PLAYER_ACT_ATTACK_CRITICAL,
        "AttackLucky", PLAYER_ACT_ATTACK_LUCKY,
        "TouchSkill", PLAYER_ACT_TOUCHSKILL
    );
}

void PlayerComponent::sync_data_types(std::shared_ptr<sol::state> state)
{   
    state->new_usertype<Assets::Inventory>("Inventory",
        "add_item", sol::resolve<enum Horizon::Zone::Assets::inventory_addition_result_type(uint32_t, uint16_t, bool)>(&Assets::Inventory::add_item)
    );
    
    state->new_usertype<Assets::Storage>("Storage",
        "notify_all", &Assets::Storage::notify_all
    );

    sol::usertype<Horizon::Zone::Units::Player> a = state->new_usertype<Horizon::Zone::Units::Player>("Player");

	a["unit"] = [](std::shared_ptr<Horizon::Zone::Units::Player> player) { return player->shared_from_this(); };
	
    a["send_npc_dialog"] = &Horizon::Zone::Units::Player::send_npc_dialog;
    a["send_npc_next_dialog"] = &Horizon::Zone::Units::Player::send_npc_next_dialog;
    a["send_npc_close_dialog"] = &Horizon::Zone::Units::Player::send_npc_close_dialog;
    a["send_npc_menu_list"] = &Horizon::Zone::Units::Player::send_npc_menu_list;
    a["move_to_map"] = &Horizon::Zone::Units::Player::move_to_map;
    a["inventory"] = &Horizon::Zone::Units::Player::inventory;
    a["storage"] = &Horizon::Zone::Units::Player::get_storage;
    a["message"] = [] (std::shared_ptr<Horizon::Zone::Units::Player> player, std::string const &message)
	{
		player->get_session()->clif()->notify_chat(message);
	};
	a["job_change"] = &Horizon::Zone::Units::Player::job_change;
    a["perform_action"] = &Horizon::Zone::Units::Player::perform_action;
    a["get_learnt_skill"] = &Horizon::Zone::Units::Player::get_learnt_skill;
    a["perform_skill"] = &Horizon::Zone::Units::Player::perform_skill;
    a["on_skill_failure"] = &Horizon::Zone::Units::Player::on_skill_failure;
}

void PlayerComponent::sync_functions(std::shared_ptr<sol::state> state)
{
    state->set_function("cast_unit_to_player",
                    [] (std::shared_ptr<Unit> e)
                    {
                        return e->template downcast<Horizon::Zone::Units::Player>();
                    });
}

void PlayerComponent::perform_command_from_player(std::shared_ptr<Horizon::Zone::Units::Player> player, std::string const &cmd)
{
    try {
        std::string script_root_path = sZone->config().get_script_root_path().string();
        sol::load_result fx = player->lua_state()->load_file(script_root_path + "internal/at_command_main.lua");
        sol::protected_function_result result = fx(player, cmd);
        if (!result.valid()) {
            sol::error err = result;
            HLog(error) << "PlayerComponent::perform_command_from_player: " << err.what();
        }
    } catch (sol::error &e) {
        HLog(error) << "PlayerComponent::perform_command_from_player: " << e.what();
    }
}
