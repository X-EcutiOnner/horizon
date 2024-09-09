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
#include "Auth.hpp"

#include "Server/Auth/SocketMgr/ClientSocketMgr.hpp"
#include "Server/Common/Server.hpp"
#include "Server/Common/Configuration/Database.hpp"

using boost::asio::ip::udp;
using namespace std::chrono_literals;
using namespace Horizon::Auth;

/**
 * Horizon Constructor.
 */
AuthServer::AuthServer()
: Server(), _update_timer(get_io_context())
{
}

/**
 * Horizon Destructor.
 */
AuthServer::~AuthServer()
{
}

#define horizon_config_error(setting_name, default) \
	HLog(error) << "No setting for '" << setting_name << "' in configuration file, defaulting to " << default;

/**
 * Read /config/horizon-server.yaml
 * @return true on success, false on failure.
 */
bool AuthServer::read_config()
{
	sol::state lua;

	std::string file_path = general_conf().get_config_file_path().string();
	std::string tmp_string;
	sol::table tbl;

    if (!boost::filesystem::exists(file_path)) {
        HLog(error) << "Configuration file does not exist at " << file_path << "." << std::endl;
        return false;
    }
    
	// Read the file. If there is an error, report it and exit.
	try {
		lua.script_file(file_path);
		tbl = lua["horizon_config"];
	} catch (const std::exception &error) {
		HLog(error) << "Horizon::ReadConfig: '" << error.what() << "'.";
		return false;
	}
	
	try {
		sol::table char_servers = tbl.get<sol::table>("character_servers");
		char_servers.for_each([&](sol::object const &key, sol::object const &value) {
			auth_config_type::char_server c;
			sol::table cfg = value.as<sol::table>();
			
			c._name = cfg.get<std::string>("Name");
			c._host = cfg.get<std::string>("Host");
			c._port = cfg.get<uint16_t>("Port");
			c._type = cfg.get<uint16_t>("Type");
			c._is_new = cfg.get<uint16_t>("IsNew");
			
			get_auth_config().add_char_server(c);
			
			HLog(info) << "Character server '" << c._name << "' is configured.";
		});
	} catch (std::exception &error) {
		HLog(error) << "Character server configuration error:" << error.what() << ".";
		return false;
	}
	
	int char_server_count = get_auth_config().get_char_servers().size();
	HLog(info) << char_server_count << " character servers were configured.";
	
	// Max network threads
	if (tbl.get<sol::object>("max_network_threads") != sol::lua_nil) {
		get_auth_config().set_max_network_threads(tbl.get<int>("max_network_threads"));
	} else {
		horizon_config_error("max_network_threads", 1);
	}

	HLog(info) << "Max Network Threads set to '" << get_auth_config().max_network_threads() << "'.";

	/**
	 * Process Configuration that is common between servers.
	 */
	if (!parse_common_configs(tbl))
		return false;

	HLog(info) << "Done reading server configuration from '" << file_path << "'.";

	return true;
}

#undef horizon_config_error

/**
 * CLI Command: Reload Configuration
 * @return boolean value from AuthServer->ReadConfig()
 */
bool AuthServer::clicmd_reload_config(std::string /*cmd*/)
{
	HLog(info) << "Reloading configuration from '" << general_conf().get_config_file_path() << "'";

	return read_config();
}

bool AuthServer::clicmd_create_new_account(std::string cmd)
{
	std::vector<std::string> separated_args;
	std::string help_str = "Usage: create-account <username> <password> <email> <birthdate: YYYY-MM-DD> <pincode> {"
		"{<account_gender = 'M' or 'F' or 'C' (Character Dependent)>}, {<group_id>}, {<character_slots>}}";
	
	boost::algorithm::split(separated_args, cmd, boost::algorithm::is_any_of(" "));

	if (separated_args.size() < 6) {
		HLog(error) << "create-account command requires at least 5 arguments.";
		HLog(error) << help_str;
		return false;
	}

	std::string username = separated_args[1];
	std::string password = separated_args[2];
	std::string email = separated_args[3];
	std::string birthdate = separated_args[4];
	std::string pincode = separated_args[5];

	game_account_gender_type gender = separated_args.size() >= 7 ? ((separated_args[6] == "M" ? ACCOUNT_GENDER_MALE : (separated_args[6] == "F" ? ACCOUNT_GENDER_FEMALE : ACCOUNT_GENDER_NONE))) : ACCOUNT_GENDER_NONE;
	int group_id = separated_args.size() >= 8 ? std::stoi(separated_args[7]) : 0;
	int character_slots = separated_args.size() >= 9 ? std::stoi(separated_args[8]) : 3;
	
	try {
		auto b1 = get_database_connection()->prepare_statement("SELECT `id` FROM game_accounts WHERE `username` = ?").bind(username);
		boost::mysql::results results;
		get_database_connection()->execute(b1, results);

		if (!results.rows().empty()) {
			HLog(error) << "Account with username '" << username << "' already exists.";
			return false;
		}

		std::vector<unsigned char> salt;
		std::vector<unsigned char> hash;

		sAuth->generate_salt(salt);
		sAuth->hash_password(password, salt, hash);
		boost::mysql::statement stmt = get_database_connection()->prepare_statement("INSERT INTO `game_accounts` (`username`, `hash`, `salt`, `gender`, `email`, `birth_date`, `character_slots`, `pincode`, `group_id`, `state`) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
		auto b2 = stmt.bind(username, hash, salt, gender == ACCOUNT_GENDER_MALE ? "M" : (gender == ACCOUNT_GENDER_FEMALE ? "F" : "NA"), 
				email, birthdate, character_slots, pincode, group_id, (int)ACCOUNT_STATE_NONE);
		get_database_connection()->execute(b2, results);
	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << error.what();
		return false;
	}
	
	HLog(info) << "Account '" << username << "' has been created successfully.";

	return true;
}

bool AuthServer::clicmd_reset_password(std::string cmd)
{
	std::vector<std::string> separated_args;
	std::string help_str = "Usage: reset-password <username> <new_password>";

	boost::algorithm::split(separated_args, cmd, boost::algorithm::is_any_of(" "));

	if (separated_args.size() < 3) {
		HLog(error) << "reset-password command requires 2 arguments.";
		HLog(error) << help_str;
		return false;
	}

	std::string username = separated_args[1];
	std::string new_password = separated_args[2];

	try {
		auto b1 = get_database_connection()->prepare_statement("SELECT `id` FROM game_accounts WHERE `username` = ?").bind(username);
		boost::mysql::results results;
		get_database_connection()->execute(b1, results);

		if (results.rows().empty()) {
			HLog(error) << "Account with username '" << username << "' does not exist.";
			return false;
		}

		std::vector<unsigned char> salt;

		sAuth->generate_salt(salt);

		std::vector<unsigned char> hash;
		sAuth->hash_password(new_password, salt, hash);

		boost::mysql::statement stmt = get_database_connection()->prepare_statement("UPDATE `game_accounts` SET `hash` = ?, `salt` = ? WHERE `username` = ?");
		auto b2 = stmt.bind(hash, salt, username);
		get_database_connection()->execute(b2, results);

		HLog(info) << "Password for account '" << username << "' has been reset successfully.";
	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << error.what();
		return false;
	}
	return true;
}

/**
 * Initialize CLI Comamnds
 */
void AuthServer::initialize_cli_commands()
{
	if (general_conf().is_test_run() && general_conf().is_test_run_minimal())
		return;

	get_component_of_type<CommandLineProcess>(Horizon::System::RUNTIME_COMMANDLINE)->add_function("reloadconf", std::bind(&AuthServer::clicmd_reload_config, this, std::placeholders::_1));
	get_component_of_type<CommandLineProcess>(Horizon::System::RUNTIME_COMMANDLINE)->add_function("create-account", std::bind(&AuthServer::clicmd_create_new_account, this, std::placeholders::_1));
	get_component_of_type<CommandLineProcess>(Horizon::System::RUNTIME_COMMANDLINE)->add_function("reset-password", std::bind(&AuthServer::clicmd_reset_password, this, std::placeholders::_1));
}

/**
 * Signal Handler from the main thread.
 * @param ioServiceRef
 * @param error
 */
void SignalHandler(int signal_num)
{
	if (sAuth->get_signal_interrupt_command_line_loop() == true) {
		sAuth->set_signal_interrupt_command_line_loop(false);
		// re-set the signal handler
		signal(signal_num, SignalHandler);
	} else {
		set_shutdown_stage(SHUTDOWN_INITIATED);
		set_shutdown_signal(SIGTERM);
	}
}

void AuthServer::update(uint64_t time)
{
	getScheduler().Update();
	
	sClientSocketMgr->manage_sockets(time);
	sClientSocketMgr->update_sessions(time);
	
	if (get_shutdown_stage() == SHUTDOWN_NOT_STARTED && !general_conf().is_test_run_minimal()) {
		_update_timer.expires_from_now(boost::posix_time::microseconds(MAX_CORE_UPDATE_INTERVAL));
		_update_timer.async_wait(std::bind(&AuthServer::update, this, std::time(nullptr)));
	} else {
	
		for (auto i = _components.begin(); i != _components.end(); i++) {
			HLog(info) << "Kernel component '" << i->second.ptr->get_type_string()  << " (" << i->second.segment_number << ")': " << (i->second.ptr->is_finalized() == true ? "Offline" : "Online (Shutting Down)") << " { CPU: " << i->second.ptr->get_thread_cpu_id() << ", uuid: " << i->first << " }";
		}

		/**
		 * Cancel all pending tasks.
		 */
		getScheduler().CancelAll();

		/**
		 * Stop all networks
		 */
		sClientSocketMgr->stop();

		finalize();
	}
}

void AuthServer::initialize()
{
	/**
	 * Core Signal Handler
	 */
	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);
#ifndef WIN32
	signal(SIGQUIT, SignalHandler);
#endif

	// Start Horizon Network
	sClientSocketMgr->start(get_io_context(), 
		general_conf().get_listen_ip(), 
		general_conf().get_listen_port(), 
		get_auth_config().max_network_threads(),
		general_conf().is_test_run() && general_conf().is_test_run_with_network() == false);
	
	// Initialize core.
	Server::initialize();
	
	Server::post_initialize();

	initialize_cli_commands();
		
	_update_timer.expires_from_now(boost::posix_time::microseconds(MAX_CORE_UPDATE_INTERVAL));
	_update_timer.async_wait(std::bind(&AuthServer::update, this, MAX_CORE_UPDATE_INTERVAL));

	// Run the io_context until stop is called from the internal, finalizing thread.
	// After stopping, execution will continue through the next line onwards.
	// We actually finalize on this thread and not in any of io_context's internal threads.
	get_io_context().run();

	/*
	 * Core Cleanup
	 */
	HLog(info) << "Server shutting down...";

	HLogShutdown;

	set_shutdown_stage(SHUTDOWN_CLEANUP_COMPLETE);
}

void AuthServer::finalize()
{
	HLog(info) << "Shutdown process initiated...";

	if (!get_io_context().stopped())
		get_io_context().stop();

	/**
	 * Server shutdown routine begins here...
	 */
	Server::finalize();
	
	Server::post_finalize();
}