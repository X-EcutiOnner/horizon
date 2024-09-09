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

#include "Zone.hpp"

#include "Server/Zone/SocketMgr/ClientSocketMgr.hpp"
#include <boost/bind/bind.hpp>

#include <signal.h>

using namespace std;
using namespace Horizon::Zone;

ZoneKernel::ZoneKernel(s_zone_server_configuration &config) 
: Server(), _config(config)
{

}

ZoneKernel::~ZoneKernel()
{
	if (!get_io_context().stopped())
		get_io_context().stop();
}

void ZoneKernel::initialize()
{
	_task_scheduler.Schedule(Seconds(60), [this] (TaskContext context) {
		verify_connected_sessions();
		context.Repeat();
	});

	Server::initialize();
}

void ZoneKernel::finalize()
{
	_task_scheduler.CancelAll();
	
	Server::finalize();
}

void ZoneKernel::verify_connected_sessions()
{	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("DELETE FROM `session_data` WHERE `current_server` = ? AND `last_update` < ?");
		auto b1 = stmt.bind("Z", std::time(nullptr) - config().session_max_timeout());
		boost::mysql::results results;
		conn->execute(b1, results);

		stmt = conn->prepare_statement("SELECT COUNT(`game_account_id`) FROM `session_data` WHERE `current_server` = ?");
		auto b2 = stmt.bind("Z");
		conn->execute(b2, results);

		int32_t count = results.rows()[0][0].as_int64();

		HLog(info) << count << " connected session(s).";
	} catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "Error while verifying connected sessions: " << err.what();
		return;
	}
}

/**
 * Zone Main server constructor.
 */
ZoneServer::ZoneServer()
: ZoneKernel(_zone_server_config), _update_timer(get_io_context())
{
}

/**
 * Zone Main server destructor.
 */
ZoneServer::~ZoneServer()
{
}

#define config_error(setting_name, default) \
	HLog(error) << "No setting for '" << setting_name << "' in configuration file, defaulting to '" << default << "'.";

/**
 * Read /config/zone-server.yaml
 * @return false on failure, true on success.
 */
bool ZoneServer::read_config()
{
	using namespace std::chrono;

	sol::state lua;
	std::string file_path = general_conf().get_config_file_path().string();

	// Read the file. If there is an error, report it and exit.
	try {
		lua.script_file(file_path);
	} catch(const std::exception &e) {
		HLog(error) << "ZoneMain::ReadConfig: " << e.what() << ".";
		return false;
	}

	sol::table tbl = lua["horizon_config"];

	config().set_static_db_path(tbl.get_or("static_db_path", std::string("db/")));
	HLog(info) << "Static database path set to " << config().get_static_db_path() << "";

	config().set_mapcache_path(tbl.get_or("map_cache_file_path", std::string("db/maps.dat")));
	HLog(info) << "Mapcache file name is set to " << config().get_mapcache_path() << ", it will be read while initializing maps.";

	config().set_script_root_path(tbl.get_or("script_root_path", std::string("scripts/")));
	HLog(info) << "Script root path is set to " << config().get_script_root_path() << ", it will be used for all scripting references.";

	config().set_session_max_timeout(tbl.get_or("session_max_timeout", 60));
	HLog(info) << "Session maximum timeout set to '" << config().session_max_timeout() << "'.";

	config().set_max_network_threads(tbl.get_or("max_network_threads", 1));
	HLog(info) << "Maximum network threads set to '" << config().max_network_threads() << "'.";

	config().set_max_game_logic_threads(tbl.get_or("max_game_logic_threads", 1));
	HLog(info) << "Maximum game logic threads set to '" << config().max_game_logic_threads() << "'.";

	config().set_max_persistence_threads(tbl.get_or("max_persistence_threads", 1));
	HLog(info) << "Maximum persistence threads set to '" << config().max_persistence_threads() << "'.";

	config().set_max_script_vm_threads(tbl.get_or("max_script_vm_threads", 1));
	HLog(info) << "Maximum script VM threads set to '" << config().max_script_vm_threads() << "'.";

	/**
	 * Process Configuration that is common between servers.
	 */
	if (!parse_common_configs(tbl))
		return false;

	HLog(info) << "Done reading server configurations from '" << file_path << "'.";

	return true;
}

uint64_t ZoneServer::to_uuid(uint8_t type, uint32_t uid, uint16_t uid2, uint8_t uid3)
{
	return ((uint64_t)uid3 << 56 | ((uint64_t)uid2 << 8) | ((uint64_t)uid << 40) | (uint64_t) type);
}

void ZoneServer::from_uuid(uint64_t unit_uuid, uint8_t& type, uint32_t& uid, uint16_t& uid2, uint8_t& uid3)
{
    type = (uint8_t) unit_uuid;
    uid = (uint32_t) (unit_uuid >> 40);
    uid2 = (uint16_t) (unit_uuid >> 8);
    uid3 = (uint8_t) (unit_uuid >> 56);
}

/**
 * Signal Handler for the Zone-Server main thread.
 * @param error
 */
void SignalHandler(int signal_num)
{
	if (sZone->get_signal_interrupt_command_line_loop() == true) {
		sZone->set_signal_interrupt_command_line_loop(false);
		// Install a signal handler
		signal(signal_num, SignalHandler);
	} else {
		set_shutdown_stage(SHUTDOWN_INITIATED);
		set_shutdown_signal(signal_num);
	}
}

void ZoneServer::update(int64_t diff)
{
	/**
	 * Process Packets.
	 */
	sZone->get_client_socket_mgr().manage_sockets(diff);
    sZone->get_client_socket_mgr().update_sessions(diff);

	// Process Horizon System Routine Queue.
	sZone->system_routine_process_queue();

	if (get_shutdown_stage() == SHUTDOWN_NOT_STARTED && !general_conf().is_test_run_minimal()) {
		_update_timer.expires_from_now(boost::posix_time::microseconds(MAX_CORE_UPDATE_INTERVAL));
		_update_timer.async_wait(boost::bind(&ZoneServer::update, this, std::time(nullptr)));
	} else {	
		for (auto i = _components.begin(); i != _components.end(); i++) {
			HLog(info) << "Kernel component '" << i->second.ptr->get_type_string()  << " (" << i->second.segment_number << ")': " << (i->second.ptr->is_finalized() == true ? "Offline" : "Online (Shutting Down)") << " { CPU: " << i->second.ptr->get_thread_cpu_id() << " , uuid: " << i->first << " }";
		}
		// Stop the client socket manager here because the io_context will be stopped later.
		// If this is stopped before the io_context, it will cause a dangling pointer.
		get_client_socket_mgr().stop();

		finalize();
	}
}

void ZoneServer::initialize()
{
	// Install a signal handler
	signal(SIGINT, SignalHandler);
#ifndef WIN32
	signal(SIGQUIT, SignalHandler);
#endif
	signal(SIGTERM, SignalHandler);
	
	// We declare this here since the responsibility of starting the network is of 
	// the Runtime level. It must remain lower than the kernel, because
	// we're using the kernel's io_context and it will be destroyed after the destruction of
	// the ClientSocketMgr class. This fixes the issue of dangling pointers and memory access violations.
	sZone->get_client_socket_mgr().start(get_io_context(),
						  sZone->general_conf().get_listen_ip(),
						  general_conf().get_listen_port(),
						  config().max_network_threads(),
						  general_conf().is_test_run() && general_conf().is_test_run_with_network() == false);
	
	// Initialize Runtime
	register_component<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME, std::make_shared<Horizon::Zone::ZoneRuntime>());
	get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->initialize();
	
	for (int i = 0; i < config().max_game_logic_threads(); i++) {
		register_component(Horizon::System::RUNTIME_GAMELOGIC, std::make_shared<GameLogicProcess>());
		get_component_of_type<GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, i + 1)->initialize(i + 1);
	}

	for (int i = 0; i < config().max_script_vm_threads(); i++) {
		register_component(Horizon::System::RUNTIME_SCRIPTVM, std::make_shared<ScriptManager>());
		get_component_of_type<ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM, i + 1)->initialize(i + 1);
	}

	for (int i = 0; i < config().max_persistence_threads(); i++) {
		register_component(Horizon::System::RUNTIME_PERSISTENCE, std::make_shared<PersistenceManager>());
		get_component_of_type<PersistenceManager>(Horizon::System::RUNTIME_PERSISTENCE, i + 1)->initialize(i + 1);
	}

	_update_timer.expires_from_now(boost::posix_time::microseconds(MAX_CORE_UPDATE_INTERVAL));
	_update_timer.async_wait(boost::bind(&ZoneServer::update, this, std::time(nullptr)));
						  
	ZoneKernel::initialize();

	Server::post_initialize();

	// Run the io_context until stop is called from the internal, finalizing thread.
	// After stopping, execution will continue through the next line onwards.
	// We actually finalize on this thread and not in any of io_context's internal threads.
	get_io_context().run();
	/*
	 * Core Cleanup
	 */
	HLog(info) << "Server shutting down...";

	HLogShutdown;
}

void ZoneServer::finalize()
{
	HLog(info) << "Server shutdown initiated ...";
	
	if (!get_io_context().stopped())
		get_io_context().stop();

	get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->finalize();
	deregister_component(Horizon::System::RUNTIME_RUNTIME);

	for (int i = 0; i < config().max_game_logic_threads(); i++) {
		get_component_of_type<GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, i + 1)->finalize();
		deregister_component(Horizon::System::RUNTIME_GAMELOGIC, i + 1);
	}
	for (int i = 0; i < config().max_persistence_threads(); i++) {
		get_component_of_type<PersistenceManager>(Horizon::System::RUNTIME_PERSISTENCE, i + 1)->finalize();
		deregister_component(Horizon::System::RUNTIME_PERSISTENCE, i + 1);
	}
	for (int i = 0; i < config().max_script_vm_threads(); i++) {
		get_component_of_type<ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM, i + 1)->finalize();
		deregister_component(Horizon::System::RUNTIME_SCRIPTVM, i + 1);
	}

	ZoneKernel::finalize();
	
	Server::post_finalize();
}

ZoneRuntime::ZoneRuntime()
: KernelComponent(sZone, Horizon::System::RUNTIME_RUNTIME),
_resource_manager(PrimaryResource(RESOURCE_PRIORITY_PRIMARY, std::make_shared<s_segment_storage<uint64_t, std::shared_ptr<ZoneSession>>>()))
{
}

void ZoneRuntime::start()
{
	while (!sZone->general_conf().is_test_run_minimal() && get_shutdown_stage() == SHUTDOWN_NOT_STARTED) {
		update(std::time(nullptr));
		std::this_thread::sleep_for(std::chrono::microseconds(MAX_CORE_UPDATE_INTERVAL));
	};
}

void ZoneRuntime::update(int64_t diff)
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	get_system_routine_manager().process_queue();

	calculate_and_set_cpu_load();
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	set_total_execution_time(time_span.count());
}