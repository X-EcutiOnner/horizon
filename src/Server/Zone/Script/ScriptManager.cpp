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

#include "ScriptManager.hpp"

#include "Server/Zone/Game/Units/NPC/NPC.hpp"
#include "Server/Common/System.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Interface/ZoneClientInterface.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Zone.hpp"

#if WIN32
	#include <windows.h>
#elif __linux__
	#include <sched.h>
#endif
using namespace Horizon::Zone;
using namespace Horizon::Zone::Units;

ScriptManager::ScriptManager()
: KernelComponent(sZone, Horizon::System::RUNTIME_SCRIPTVM),
_lua_state(std::make_shared<sol::state>()),
_player_component(std::make_shared<PlayerComponent>()),
_npc_component(std::make_shared<NPCComponent>()),
_monster_component(std::make_shared<MonsterComponent>()),
_map_component(std::make_shared<MapComponent>()),
_item_component(std::make_shared<ItemComponent>()),
_unit_component(std::make_shared<UnitComponent>()),
_skill_component(std::make_shared<SkillComponent>()),
_status_effect_component(std::make_shared<StatusEffectComponent>()),
_combat_component(std::make_shared<CombatComponent>()),
_resource_manager(PrimaryResource(RESOURCE_PRIORITY_PRIMARY, std::make_shared<s_segment_storage<uint64_t, std::shared_ptr<Units::NPC>>>()))
{
}

ScriptManager::~ScriptManager()
{
	if (_lua_state != nullptr)
		_lua_state.reset();
}

void ScriptManager::initialize(int segment_number)
{
	set_segment_number(segment_number);
	_thread = std::thread(&ScriptManager::start, this);
	
	for (int i = 0; i < sZone->get_registered_component_count_of_type(Horizon::System::RUNTIME_GAMELOGIC); i++)
		while (sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, i + 1)->is_initialized() == false)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			
	initialize_basic_state(_lua_state);
	initialize_monster_state(_lua_state);
	initialize_npc_state(_lua_state);
	initialize_player_state(_lua_state);

	load_constants();
	load_scripts();

	_is_initialized.exchange(true);

}

void ScriptManager::finalize()
{
	if (_thread.joinable()) {
		try {
			_thread.join();
		} catch(std::exception &e) {
			HLog(error) << "ScriptManager::finalize: " << e.what();
		}
	}

	_lua_state->collect_garbage();
	
	_script_files.clear();

	_is_finalized.exchange(true);
}

void ScriptManager::start()
{

	while (!sZone->general_conf().is_test_run_minimal() && get_shutdown_stage() == SHUTDOWN_NOT_STARTED) {
		update(std::time(nullptr));
		std::this_thread::sleep_for(std::chrono::microseconds(MAX_CORE_UPDATE_INTERVAL));
	}
}

void ScriptManager::update(uint64_t diff)
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	getScheduler().Update();

	get_system_routine_manager().process_queue();
	
#if WIN32
	DWORD cpu = GetCurrentProcessorNumber();
	if (get_thread_cpu_id() != (int) cpu) 
		set_thread_cpu_id(cpu);
#elif __linux__
	int cpu = sched_getcpu();
	if (get_thread_cpu_id() != cpu)
		set_thread_cpu_id(cpu);
#endif
	calculate_and_set_cpu_load();
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	set_total_execution_time(time_span.count());
}

void ScriptManager::initialize_basic_state(std::shared_ptr<sol::state> state)
{
	sol::protected_function_result res{};

	state->open_libraries(sol::lib::base);
	state->open_libraries(sol::lib::string);
	state->open_libraries(sol::lib::math);
	state->open_libraries(sol::lib::coroutine);
	state->open_libraries(sol::lib::jit);
	state->open_libraries(sol::lib::table);
	state->open_libraries(sol::lib::package);
	state->open_libraries(sol::lib::os);

	// Load timer function
	
	state->set_function("get_time", get_sys_time);
	// @TODO Schedule Function
	state->set_function("schedule", [this] (uint32_t time, sol::function fn) {
		getScheduler().Schedule(
		  Milliseconds(time),
		  [fn] (TaskContext context) {
		    sol::protected_function_result result = fn();
		    if (!result.valid()) {
					sol::error err = result;
					HLog(error) << "ScriptManager::initialize_basic_state: Error on scheduled function: " << err.what();
				}
		  }
		);
	});

	_map_component->sync_definitions(state);
	_map_component->sync_data_types(state);
	_map_component->sync_functions(state);

	_item_component->sync_definitions(state);
	_item_component->sync_data_types(state);
	_item_component->sync_functions(state);

	_skill_component->sync_definitions(state);
	_skill_component->sync_data_types(state);
	_skill_component->sync_functions(state);

	_status_effect_component->sync_definitions(state);
	_status_effect_component->sync_data_types(state);
	_status_effect_component->sync_functions(state);

	_combat_component->sync_definitions(state);
	_combat_component->sync_data_types(state);
	_combat_component->sync_functions(state);

	_unit_component->sync_definitions(state);
	_unit_component->sync_data_types(state);
	_unit_component->sync_functions(state);

	std::string script_root_path = sZone->config().get_script_root_path().string();
	std::string static_db_root_path = sZone->config().get_static_db_path().string();
	std::vector<std::string> _loadable_files = {
		script_root_path + "utils/strutils.lua",
		static_db_root_path + "definitions/constants.lua"
	};

	for (auto &file : _loadable_files) {
		try {
			res = state->script_file(file);

			if (!res.valid()) {
				sol::error error = res;
				HLog(error) << "ScriptManager::initialize_state: " << error.what();
			}
		} catch (sol::error &error) {
			HLog(error) << "ScriptManager::initialize_state: " << error.what();
		}
	}

	// Macro to constants
	(*state)["MAX_LEVEL"]            = MAX_LEVEL;
	(*state)["MAX_STATUS_POINTS"]    = MAX_STATUS_POINTS;
	(*state)["MAX_CHARACTER_SLOTS"]  = MAX_CHARACTER_SLOTS;
	(*state)["MAX_VIEW_RANGE"]       = MAX_VIEW_RANGE;
	(*state)["MIN_INVENTORY_SIZE"]   = MIN_INVENTORY_SIZE;
	(*state)["MAX_INVENTORY_SIZE"]   = MAX_INVENTORY_SIZE;
	(*state)["MIN_STORAGE_SIZE"]     = MIN_STORAGE_SIZE;
	(*state)["MAX_STORAGE_SIZE"]     = MAX_STORAGE_SIZE;
	(*state)["MAX_INVENTORY_STACK_LIMIT"]  = MAX_INVENTORY_STACK_LIMIT;
	(*state)["MAX_CART_STACK_LIMIT"]  = MAX_CART_STACK_LIMIT;
	(*state)["MAX_STORAGE_STACK_LIMIT"]  = MAX_STORAGE_STACK_LIMIT;
	(*state)["MAX_GSTORAGE_STACK_LIMIT"]  = MAX_GSTORAGE_STACK_LIMIT;

#ifdef RENEWAL
	(*state)["RENEWAL"]              = true;
#else
	(*state)["RENEWAL"]              = false;
#endif

	(*state)["basic_component"] = true;
}

void ScriptManager::initialize_player_state(std::shared_ptr<sol::state> state)
{
	if ((*state)["basic_component"] == sol::lua_nil)
		initialize_basic_state(state);

	_player_component->sync_definitions(state);
	_player_component->sync_data_types(state);
	_player_component->sync_functions(state);

	(*state)["player_component"] = true;
}

void ScriptManager::initialize_npc_state(std::shared_ptr<sol::state> state)
{
	if ((*state)["basic_component"] == sol::lua_nil)
		initialize_basic_state(state);

	_npc_component->sync_definitions(state);
	_npc_component->sync_data_types(state);
	// @TODO NPC Component Functions
	_npc_component->sync_functions(state);

	(*state)["npc_component"] = true;
}

void ScriptManager::initialize_monster_state(std::shared_ptr<sol::state> state)
{
	if ((*state)["basic_component"] == sol::lua_nil)
		initialize_basic_state(state);

	_monster_component->sync_definitions(state);
	_monster_component->sync_data_types(state);
	// @TODO Monster component functions
	_monster_component->sync_functions(state);


	(*state)["monster_component"] = true;
}

static std::atomic<bool> _script_loader{false};
void ScriptManager::load_scripts()
{
	std::string script_root_path = sZone->config().get_script_root_path().string();

	// Script loader acquired, the other threads do not need to load the scripts as one has already done it.
	if (_script_loader.exchange(true) == true)
		return;

	try {
		_lua_state->script_file(script_root_path + "include.lua");

		sol::table scripts = (*_lua_state)["scripts"];

		int count = 0;
		scripts.for_each([this, &count, &script_root_path](sol::object const &/*key*/, sol::object const& value) {
			std::string script_file = value.as<std::string>();
			sol::protected_function fn = _lua_state->load_file(script_root_path + script_file);
			sol::protected_function_result result = fn();
			if (!result.valid()) {
				sol::error error = result;
				HLog(warning) << "Failed to load file '" << script_file << "' from '" << script_root_path << "', reason: " << error.what();
				return;
			}
			count++;
		});
		HLog(info) << "Read " << count << " NPC scripts from '" << script_root_path << "'.";
	} catch (sol::error &e) {
		HLog(warning) << "Failed to load included script files from '" << script_root_path << "', reason: " << e.what();
	}
}

static std::atomic<bool> _constants_loader{false};
void ScriptManager::load_constants()
{
	std::string file_path = sZone->config().get_static_db_path().string();

	// Constants loader acquired, the other threads do not need to load the constants as one has already done it.
	if (_constants_loader.exchange(true) == true)
		return;

	try {
		_lua_state->script_file(file_path + "definitions/constants.lua");
		sol::table const_table = _lua_state->get<sol::table>("constants");
		HLog(info) << "Read constants from '" << file_path << "'.";
	} catch (sol::error &e) {
		HLog(error) << "Failed to read constants from '" << file_path << "', reason: " << e.what();
	}
}
