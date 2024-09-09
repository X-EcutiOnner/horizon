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

#include "ZoneSystem.hpp"
#include "Server/Common/System.hpp"
#include "Server/Common/Server.hpp"
#include "Server/Zone/Packets/TransmittedPackets.hpp"
#include "Server/Zone/Zone.hpp"

bool Horizon::Zone::SCENARIO_LOGIN::Login::execute()
{
	std::lock_guard<std::mutex> lock(get_runtime_context()->get_runtime_synchronization_mutex());
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();

	boost::mysql::statement stmt = conn->prepare_statement("SELECT `current_server` FROM `session_data` WHERE `game_account_id` = ? AND `auth_code` = ?");
	auto b1 = stmt.bind(_request.account_id, _request.auth_code);
	boost::mysql::results results;
	conn->execute(b1, results);

	if (results.rows().empty()) {
		get_message_agent().set_error_message("Login error! Session data for game account " + std::to_string(_request.account_id) + " and authentication code " + std::to_string(_request.auth_code) + " does not exist.");
		return false;
	}

	auto r = results.rows()[0];

	std::string current_server = r[0].as_string();
	if (current_server.compare("Z") == 0) { // Already on Zone.
		ZC_REFUSE_ENTER pkt(std::dynamic_pointer_cast<ActiveRuntimeScenario>(get_runtime_context())->get_session());
		pkt.deliver(ZONE_SERV_ERROR_REJECT);
		get_message_agent().set_error_message("Login of game account " + std::to_string(_request.account_id) + " is refused, already logged in.");
		return false;
	}

	stmt = conn->prepare_statement("UPDATE `session_data` SET `current_server` = ? WHERE `game_account_id` = ? AND `auth_code` = ?");
	auto b2 = stmt.bind("Z", _request.account_id, _request.auth_code);
	conn->execute(b2, results);
	
	stmt = conn->prepare_statement("SELECT `id`, `account_id`, `current_map`, `current_x`, `current_y` FROM `characters` WHERE id = ?");
	auto b3 = stmt.bind(_request.char_id);
    boost::mysql::static_results<s_login_info_query_result> s_results;
	conn->execute(b3, s_results);

	if (s_results.rows().empty()) {
		get_message_agent().set_error_message("Error loading player, character with ID " + std::to_string(_request.char_id) + " does not exist.");
		return false;
	}

	s_login_info_query_result login = s_results.rows()[0];
	std::dynamic_pointer_cast<SCENARIO_LOGIN>(get_runtime_context())->set_result(Horizon::System::Result<s_login_info_query_result>(login));

	auto resource_manager = sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_resource_manager();
	auto active_scenario = std::dynamic_pointer_cast<ActiveRuntimeScenario>(get_runtime_context());
	resource_manager.add<RESOURCE_PRIORITY_PRIMARY>(active_scenario->get_session()->get_session_id(), active_scenario->get_session());
	get_message_agent().set_status_message("Login of game account " + std::to_string(_request.account_id) + " is successful.");
	return true;
}

bool Horizon::Zone::SCENARIO_CREATE_PLAYER::CreatePlayer::execute()
{
	std::lock_guard<std::mutex> lock(get_runtime_context()->get_runtime_synchronization_mutex());

	s_login_info_query_result login_info = std::dynamic_pointer_cast<SCENARIO_CREATE_PLAYER>(get_runtime_context())->get_previous_context_result().get_one();
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();

	boost::mysql::statement stmt = conn->prepare_statement("SELECT `gender`, `group_id` FROM `game_accounts` WHERE `id` = ?");
	boost::mysql::results results;
	auto b2 = stmt.bind(login_info.account_id);
	conn->execute(b2, results);
	
	if (results.rows().empty()) {
		get_message_agent().set_error_message("Login error! Game account with id " + std::to_string(login_info.account_id) + " does not exist.");
		return false;
	}
	
	auto r2 = results.rows()[0];

	uint64_t uuid = sZone->to_uuid((int8_t) UNIT_PLAYER, login_info.account_id, login_info.id, 0);
	std::shared_ptr<Horizon::Zone::Units::Player> pl = std::make_shared<Horizon::Zone::Units::Player>(std::dynamic_pointer_cast<ActiveRuntimeScenario>(get_runtime_context())->get_session(), uuid);
	pl->create(login_info.id, r2[0].as_string(), r2[1].as_int64());
	std::dynamic_pointer_cast<ActiveRuntimeScenario>(get_runtime_context())->get_session()->set_player(pl);

	int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, login_info.current_map, nullptr);
	
	if (segment_number == 0)
	{
		get_message_agent().set_error_message("Error creating player, map " + login_info.current_map + " does not exist.");
		return false;
	}
	auto resource_manager = sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number)->get_resource_manager();
	resource_manager.add<RESOURCE_PRIORITY_SECONDARY>(pl->uuid(), pl);
	resource_manager.add<RESOURCE_PRIORITY_TERTIARY>(pl->uuid(), pl->shared_from_this());
	
	get_message_agent().set_status_message("Player (char_id: " + std::to_string(login_info.id) + ") created successfully.");
	return true;
}

bool Horizon::Zone::SCENARIO_LOGIN_RESPONSE::LoginResponse::execute()
{	
	std::lock_guard<std::mutex> lock(get_runtime_context()->get_runtime_synchronization_mutex());
	ZC_AID zc_aid(std::dynamic_pointer_cast<ActiveRuntimeScenario>(get_runtime_context())->get_session());
	zc_aid.deliver(_request.account_id);
	
	ZC_ACCEPT_ENTER2 zc_ae2(std::dynamic_pointer_cast<ActiveRuntimeScenario>(get_runtime_context())->get_session());
	zc_ae2.deliver(_request.current_x, _request.current_y, DIR_SOUTH, _request.font); // edit third argument to saved font.

	get_message_agent().set_status_message("Login response for game account " + std::to_string(_request.account_id) + " is successful.");
	return true;
}

bool Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask::execute()
{
	std::lock_guard<std::mutex> lock(get_runtime_context()->get_runtime_synchronization_mutex());
	get_task()(std::dynamic_pointer_cast<GenericTask>(shared_from_this()));
	return true;
}

bool Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN::RegisterMonsterSpawn::execute()
{
	std::lock_guard<std::mutex> lock(get_runtime_context()->get_runtime_synchronization_mutex());
	std::shared_ptr<Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN> scenario = std::dynamic_pointer_cast<Horizon::Zone::SCENARIO_REGISTER_MONSTER_SPAWN>(get_runtime_context());
	std::shared_ptr<Horizon::Zone::GameLogicProcess> game_logic = std::static_pointer_cast<Horizon::Zone::GameLogicProcess>(scenario->get_component());
	auto map = game_logic->get_resource_manager().template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(_request.data.map_name, nullptr);
	
	if (map == nullptr) {
		get_message_agent().set_error_message("Error registering monster spawn in map " + _request.data.map_name + ", map does not exist.");
		return false;
	}

	game_logic->get_monster_spawn_agent().register_monster_spawn_info(_request.data.spawn_dataset_id, std::make_shared<monster_spawn_data>(_request.data));

	HLog(debug) << "Monster spawn registered in map " << _request.data.map_name << " with spawn dataset ID " << _request.data.spawn_dataset_id << ".";
	return true;
}

bool Horizon::Zone::SCENARIO_SPAWN_MONSTERS_IN_MAP::SpawnMonsters::execute()
{
	std::lock_guard<std::mutex> lock(get_runtime_context()->get_runtime_synchronization_mutex());
	std::shared_ptr<Horizon::Zone::SCENARIO_SPAWN_MONSTERS_IN_MAP> scenario = std::dynamic_pointer_cast<Horizon::Zone::SCENARIO_SPAWN_MONSTERS_IN_MAP>(get_runtime_context());
	std::shared_ptr<Horizon::Zone::GameLogicProcess> game_logic = std::static_pointer_cast<Horizon::Zone::GameLogicProcess>(scenario->get_component());
	auto resource_manager = game_logic->get_resource_manager();
	auto map = resource_manager.template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(_request.map_name, nullptr);
	
	if (map == nullptr) {
		get_message_agent().set_error_message("Error spawning monsters in map " + _request.map_name + ", map does not exist.");
		return false;
	}

	game_logic->get_monster_spawn_agent().spawn_monsters(_request.map_name);
	get_message_agent().set_status_message("Monsters spawned in map " + _request.map_name + ".");
	return true;
}

bool Horizon::Zone::SCENARIO_REMOVE_MONSTERS_IN_MAP::RemoveMonsters::execute()
{
	std::lock_guard<std::mutex> lock(get_runtime_context()->get_runtime_synchronization_mutex());
	std::shared_ptr<Horizon::Zone::SCENARIO_REMOVE_MONSTERS_IN_MAP> scenario = std::dynamic_pointer_cast<Horizon::Zone::SCENARIO_REMOVE_MONSTERS_IN_MAP>(get_runtime_context());
	std::shared_ptr<Horizon::Zone::GameLogicProcess> game_logic = std::static_pointer_cast<Horizon::Zone::GameLogicProcess>(scenario->get_component());
	auto resource_manager = game_logic->get_resource_manager();
	auto map = resource_manager.template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(_request.map_name, nullptr);
	
	if (map == nullptr) {
		get_message_agent().set_error_message("Error removing monsters in map " + _request.map_name + ", map does not exist.");
		return false;
	}

	game_logic->get_monster_spawn_agent().despawn_monsters(_request.map_name);
	get_message_agent().set_status_message("Monsters removed in map " + _request.map_name + ".");
	return true;
}