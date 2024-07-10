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

#include "Server/Zone/Game/Map/MapManager.hpp"

using namespace std;
using namespace Horizon::Zone;

ZoneMainframe::ZoneMainframe(s_zone_server_configuration &config) 
: Server(), _config(config), _runtime(get_io_service(), general_conf())
{

}

ZoneMainframe::~ZoneMainframe()
{

}

void ZoneMainframe::initialize()
{
	_task_scheduler.Schedule(Seconds(60), [this] (TaskContext context) {
		verify_connected_sessions();
		context.Repeat();
	});

	// Start Network
	register_component(Horizon::System::RUNTIME_NETWORKING, std::make_shared<ClientSocketMgr>());
	get_component_of_type<ClientSocketMgr>(Horizon::System::RUNTIME_NETWORKING)->start(get_io_service(),
						  general_conf().get_listen_ip(),
						  general_conf().get_listen_port(),
						  MAX_NETWORK_THREADS);
	
	for (int i = 0; i < MAX_GAME_LOGIC_THREADS; i++) {
		register_component(Horizon::System::RUNTIME_GAMELOGIC, std::make_shared<GameLogicProcess>());
		get_component_of_type<GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, i + 1)->initialize(i + 1);
	}

	for (int i = 0; i < MAX_PERSISTENCE_THREADS; i++) {
		register_component(Horizon::System::RUNTIME_PERSISTENCE, std::make_shared<PersistenceManager>());
		get_component_of_type<PersistenceManager>(Horizon::System::RUNTIME_PERSISTENCE, i + 1)->initialize(i + 1);
	}

	for (int i = 0; i < MAX_SCRIPT_VM_THREADS; i++) {
		register_component(Horizon::System::RUNTIME_SCRIPTVM, std::make_shared<ScriptManager>());
		get_component_of_type<ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM, i + 1)->initialize(i + 1);
	}

	Server::initialize();

	// Initialize mainframe runtime. This is where the mainframe begins its routines of working during its lifetime.
	get_runtime().initialize();

	get_io_service().run();
}

void ZoneMainframe::finalize()
{
	HLog(info) << "Server shutdown initiated ...";

	get_io_service().stop();

	for (int i = 0; i < MAX_GAME_LOGIC_THREADS; i++)
		get_component_of_type<GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, i + 1)->finalize(i + 1);
	for (int i = 0; i < MAX_PERSISTENCE_THREADS; i++)
		get_component_of_type<PersistenceManager>(Horizon::System::RUNTIME_PERSISTENCE, i + 1)->finalize(i + 1);
	for (int i = 0; i < MAX_SCRIPT_VM_THREADS; i++)
		get_component_of_type<ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM, i + 1)->finalize(i + 1);

	get_component_of_type<ClientSocketMgr>(Horizon::System::RUNTIME_NETWORKING)->finalize();

	/**
	 * Server shutdown routine begins here...
	 */
	_task_scheduler.CancelAll();
	
	Server::finalize();

}

void ZoneMainframe::verify_connected_sessions()
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
: ZoneMainframe(_zone_server_config)
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
//	config().set_unit_save_interval(tbl.get_or("unit_save_interval", 180000));
//	HLog(info) << "Unit data will be saved to the database every " << duration_cast<minutes>(std::chrono::milliseconds(config().get_unit_save_interval())).count() << " minutes and " << duration_cast<seconds>(std::chrono::milliseconds(config().get_unit_save_interval())).count() << " seconds.";
	
	config().set_session_max_timeout(tbl.get_or("session_max_timeout", 60));

	HLog(info) << "Session maximum timeout set to '" << config().session_max_timeout() << "'.";

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
void SignalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM
#ifndef WIN32
		|| signal == SIGQUIT
#endif
		) {
		set_shutdown_stage(SHUTDOWN_INITIATED);
		set_shutdown_signal(signal);
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

	ZoneMainframe::initialize();
}

/**
 * Zone Server Main runtime entrypoint.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char * argv[])
{
	std::srand(std::time(nullptr));
	
	if (argc > 1)
		sZone->parse_exec_args(argv, argc);

	/*
	 * Read Configuration Settings for
	 * the Zoneacter Server.
	 */
	if (!sZone->read_config())
		exit(1); // Stop process if the file can't be read.

	/**
	 * Initialize the Common Core
	 */
	sZone->initialize();

	/*
	 * Core Cleanup
	 */
	HLog(info) << "Server shutting down...";

	return 0;
}
