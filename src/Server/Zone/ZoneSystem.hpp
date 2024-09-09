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

#ifndef HORIZON_ZONE_ZONESYSTEM_HPP
#define HORIZON_ZONE_ZONESYSTEM_HPP

#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Definitions/MonsterDefinitions.hpp"

#include "Server/Common/System.hpp"
#include "Server/Common/Server.hpp"

#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/static_results.hpp>
#include <boost/mysql/tcp_ssl.hpp>
#include <boost/describe/class.hpp>

class DatabaseProcess;

struct s_character_info 
{
	int32_t character_id{0};
	int16_t slot{0};
	int8_t online{0};
	std::string gender{""};
	int32_t max_inventory_size{MAX_INVENTORY_SIZE};
	int64_t last_unique_id{0};
	int8_t font{0};
	int32_t unban_time{0};
	int32_t rename_count{0};
	int16_t hotkey_row_index{0};
	int32_t change_slot_count{0};
	int8_t show_equip{0}; 
	int8_t allow_party{0};
	int32_t partner_aid{0};
	int32_t father_aid{0}, mother_aid{0}, child_aid{0};
	int32_t party_id{0}, guild_id{0};
	int32_t homun_id{0}, pet_id{0}, elemental_id{0};
	std::string saved_map{""};
	int32_t saved_x{0}, saved_y{0};
	std::string current_map{""};
	int32_t current_x{0}, current_y{0};
};

BOOST_DESCRIBE_STRUCT(s_character_info, (), (character_id, slot, online,
	gender, max_inventory_size, last_unique_id, font, unban_time, rename_count,
	hotkey_row_index, change_slot_count, show_equip, allow_party, partner_aid,
	father_aid, mother_aid, child_aid, party_id, guild_id,
	homun_id, pet_id, elemental_id, saved_map, saved_x, saved_y,
	current_map, current_x, current_y))

struct s_login_info_query_result
{
	int64_t id;
	int64_t account_id;
	std::string current_map;
	int32_t current_x;
	int32_t current_y;
};

BOOST_DESCRIBE_STRUCT(s_login_info_query_result, (), (id, account_id, current_map, current_x, current_y))

namespace Horizon
{
namespace Zone
{

class ZoneSession;

struct s_scenario_login_request
{
	uint32_t account_id;
	uint32_t char_id;
	uint32_t auth_code;
	uint32_t client_time;
	uint8_t gender;
};

class ActiveRuntimeScenario : public Horizon::System::RuntimeRoutineContext
{
public:
	ActiveRuntimeScenario(Horizon::System::SystemRoutineManager &srm, Horizon::System::runtime_synchronization_method sync_t) 
	: Horizon::System::RuntimeRoutineContext(srm, sync_t) { }

	std::shared_ptr<ZoneSession> get_session() { return _session; }
	void set_session(std::shared_ptr<ZoneSession> session) { _session = session; }

	std::shared_ptr<ZoneSession> _session;
};

class PassiveRuntimeScenario : public Horizon::System::RuntimeRoutineContext
{
public:
	PassiveRuntimeScenario(std::shared_ptr<KernelComponent> component, Horizon::System::runtime_module_type module_t, Horizon::System::runtime_synchronization_method sync_t) 
	: Horizon::System::RuntimeRoutineContext(component->get_system_routine_manager(), sync_t), _component(component), _module_type(module_t) { }

	std::shared_ptr<KernelComponent> get_component() { return _component.expired() == false ? _component.lock() : nullptr; }
	
	Horizon::System::runtime_module_type get_module_type() { return _module_type; }

protected:
	std::weak_ptr<KernelComponent> _component;
	Horizon::System::runtime_module_type _module_type{Horizon::System::RUNTIME_RUNTIME};
};

class SCENARIO_LOGIN : public ActiveRuntimeScenario
{
public:
	SCENARIO_LOGIN(Horizon::System::SystemRoutineManager &srm) 
	: ActiveRuntimeScenario(srm, Horizon::System::RUNTIME_SYNC_WAIT_CHECK_STATE) { }

	class Login : public Horizon::System::RuntimeRoutineContext::Work
	{
	public:
		Login(std::shared_ptr<SCENARIO_LOGIN> parent_context) 
		: Horizon::System::RuntimeRoutineContext::Work(parent_context) { }

		void set_request(s_scenario_login_request req) { _request = req; }
		s_scenario_login_request get_request() { return _request; }

		bool execute();

		s_scenario_login_request _request;
	};

	Horizon::System::Result<s_login_info_query_result> get_result() { return _context_result; }
	void set_result(Horizon::System::Result<s_login_info_query_result> result) { _context_result = result; }

	Horizon::System::Result<s_login_info_query_result> _context_result;
};

struct s_player_loaded_data
{
	int32_t account_id;
	int32_t char_id;
	std::string map;
	int32_t current_x, current_y;
};

class SCENARIO_CREATE_PLAYER : public ActiveRuntimeScenario
{
public:
	SCENARIO_CREATE_PLAYER(Horizon::System::SystemRoutineManager &srm) 
	: ActiveRuntimeScenario(srm, Horizon::System::RUNTIME_SYNC_WAIT_CHECK_STATE),
	 _previous_context_result(s_login_info_query_result{})
	 { }

	class CreatePlayer : public Horizon::System::RuntimeRoutineContext::Work
	{
	public:
		CreatePlayer(std::shared_ptr<SCENARIO_CREATE_PLAYER> parent_context) 
		: Horizon::System::RuntimeRoutineContext::Work(parent_context)
		{ }
		
		void set_request(s_player_loaded_data req) { }
		s_player_loaded_data get_request() { return _request; }

		bool execute();

		s_player_loaded_data _request;
	};

	void set_previous_context_result(Horizon::System::Result<s_login_info_query_result> result)
	{
		_previous_context_result = result;
	}
	
	Horizon::System::Result<s_login_info_query_result> get_previous_context_result()
	{
		return _previous_context_result;
	}

	bool run() override
	{
		
		// use result in work.
		if (RuntimeRoutineContext::run()) {
			return true;
		}

		return false;
	}

	Horizon::System::Result<s_login_info_query_result> _previous_context_result;
};

class SCENARIO_LOGIN_RESPONSE : public ActiveRuntimeScenario
{
public:
	SCENARIO_LOGIN_RESPONSE(Horizon::System::SystemRoutineManager &srm) 
	: ActiveRuntimeScenario(srm, Horizon::System::RUNTIME_SYNC_WAIT_CHECK_STATE) { }

	struct s_scenario_login_response_request
	{
		uint32_t account_id;
		uint32_t current_x, current_y;
		int8_t font;
	};

	class LoginResponse : public Horizon::System::RuntimeRoutineContext::Work
	{
	public:
		LoginResponse(std::shared_ptr<SCENARIO_LOGIN_RESPONSE> parent_context) 
		: Horizon::System::RuntimeRoutineContext::Work(parent_context) { }

		bool execute();

		void set_request(s_scenario_login_response_request req) { _request = req; }
		s_scenario_login_response_request get_request() { return _request; }

		s_scenario_login_response_request _request;
	};
};

class SCENARIO_GENERIC_TASK : public ActiveRuntimeScenario
{
public:
	SCENARIO_GENERIC_TASK(Horizon::System::SystemRoutineManager &srm) 
	: ActiveRuntimeScenario(srm, Horizon::System::RUNTIME_SYNC_WAIT_CHECK_STATE) { }

	class GenericTask : public Horizon::System::RuntimeRoutineContext::Work
	{
	public:
		GenericTask(std::shared_ptr<SCENARIO_GENERIC_TASK> parent_context) 
		: Horizon::System::RuntimeRoutineContext::Work(parent_context) { }

		bool execute();

		void set_task(std::function<void(std::shared_ptr<GenericTask> generic_task)> task) { _generic_task = task; }
		std::function<void(std::shared_ptr<GenericTask> send_packet)> get_task() { return _generic_task; }

		std::function<void(std::shared_ptr<GenericTask> send_packet)> _generic_task;
	};
};

class SCENARIO_REGISTER_MONSTER_SPAWN : public PassiveRuntimeScenario
{
public:
	SCENARIO_REGISTER_MONSTER_SPAWN(std::shared_ptr<KernelComponent> component)
	: PassiveRuntimeScenario(component, Horizon::System::RUNTIME_GAMELOGIC, Horizon::System::RUNTIME_SYNC_WAIT_CHECK_STATE) { }

	struct s_register_monster_spawn_request
	{
		monster_spawn_data data;
	};

	class RegisterMonsterSpawn : public Horizon::System::RuntimeRoutineContext::Work
	{
	public:
		RegisterMonsterSpawn(std::shared_ptr<SCENARIO_REGISTER_MONSTER_SPAWN> parent_context) 
		: Horizon::System::RuntimeRoutineContext::Work(parent_context) { }

		bool execute();

		void set_request(s_register_monster_spawn_request request) { _request = request; }
		s_register_monster_spawn_request get_request() { return _request; }

		s_register_monster_spawn_request _request;
	};
};

class SCENARIO_SPAWN_MONSTERS_IN_MAP : public PassiveRuntimeScenario
{
public:
	SCENARIO_SPAWN_MONSTERS_IN_MAP(std::shared_ptr<KernelComponent> component) 
	: PassiveRuntimeScenario(component, Horizon::System::RUNTIME_GAMELOGIC, Horizon::System::RUNTIME_SYNC_WAIT_CHECK_STATE) { }

	struct s_spawn_monster_request
	{
		std::string map_name;
	};

	class SpawnMonsters : public Horizon::System::RuntimeRoutineContext::Work
	{
	public:
		SpawnMonsters(std::shared_ptr<SCENARIO_SPAWN_MONSTERS_IN_MAP> parent_context) 
		: Horizon::System::RuntimeRoutineContext::Work(parent_context) { }

		bool execute();

		void set_request(s_spawn_monster_request request) { _request = request; }
		s_spawn_monster_request get_request() { return _request; }

		s_spawn_monster_request _request;
	};
};

class SCENARIO_REMOVE_MONSTERS_IN_MAP : public PassiveRuntimeScenario
{
public:
	SCENARIO_REMOVE_MONSTERS_IN_MAP(std::shared_ptr<KernelComponent> component) 
	: PassiveRuntimeScenario(component, Horizon::System::RUNTIME_GAMELOGIC, Horizon::System::RUNTIME_SYNC_WAIT_CHECK_STATE) { }

	struct s_remove_monster_request
	{
		std::string map_name;
	};

	class RemoveMonsters : public Horizon::System::RuntimeRoutineContext::Work
	{
	public:
		RemoveMonsters(std::shared_ptr<SCENARIO_REMOVE_MONSTERS_IN_MAP> parent_context) 
		: Horizon::System::RuntimeRoutineContext::Work(parent_context) { }

		bool execute();

		void set_request(s_remove_monster_request request) { _request = request; }
		s_remove_monster_request get_request() { return _request; }

		s_remove_monster_request _request;
	};
};
}
}
#endif // HORIZON_ZONE_ZONESYSTEM_HPP
