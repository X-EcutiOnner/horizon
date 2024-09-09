/***************************************************
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

#include "Char.hpp"

#include "Server/Char/SocketMgr/ClientSocketMgr.hpp"

#include <boost/asio.hpp>
#include <boost/make_shared.hpp>
#include <iostream>

#include <sol.hpp>

using namespace Horizon::Char;

using namespace std;

/**
 * Char Constructor
 */
CharServer::CharServer()
: Server(), _update_timer(get_io_context())
{
	//
}

/**
 * Char Destructor
 */
CharServer::~CharServer()
{
	//
}

#define char_config_error(s, d) \
	HLog(error) << "No setting for '" << s << "' in configuration file, defaulting to '" << d << "'.";

/**
 * Read /config/char-server.yaml
 * @return true on success false on failure.
 */
bool CharServer::read_config()
{
	sol::state lua;
	std::string file_path = general_conf().get_config_file_path().string();

	// Read the file. If there is an error, report it and exit.
	try {
		lua.script_file(file_path);
	} catch(const std::exception &e) {
		HLog(error) << "Char::ReadConfig: " << e.what();
		return false;
	}

	sol::table tbl = lua["horizon_config"];

	try {
		sol::table chtbl = tbl.get<sol::table>("new_character");
		config().set_start_map(chtbl.get_or("start_map", std::string("new_1-1")));
		config().set_start_x(chtbl.get_or("start_x", 53));
		config().set_start_y(chtbl.get_or("start_y", 111));
		config().set_start_zeny(chtbl.get_or("start_zeny", 0));

		sol::optional<sol::table> maybe_chitbl = chtbl.get<sol::optional<sol::table>>("start_items");

		if (maybe_chitbl) {
			sol::table chitbl = maybe_chitbl.value();
			chitbl.for_each([this](sol::object const &key, sol::object const &value) {
				config().add_start_item(std::make_pair(key.as<int>(), value.as<int>()));
			});
			if (chitbl.size() < 1) {
				config().add_start_item(std::make_pair(1201, 1));
				config().add_start_item(std::make_pair(2301, 1));
			}
		}
	} catch (sol::error &err) {
		HLog(error) << "Error for setting in new_character:" << err.what();
	}

	try {
		int char_deletion_time = tbl.get_or("character_deletion_time", 86400);
		config().set_character_deletion_time(char_deletion_time);
		HLog(info) << "Character deletion wait period : '" << char_deletion_time << "'.";
	} catch (sol::error &err) {
		HLog(error) << "Error for setting character_deletion_time:" << err.what();
	}

	try {
		bool char_hard_delete = tbl.get_or("character_hard_delete", false);
		config().set_char_hard_delete(char_hard_delete);
		HLog(info) << "Character hard delete (permanently deleted) : '" << (char_hard_delete ? "True" : "False") << "'.";
	} catch (sol::error &err) {
		HLog(error) << "Error for setting character_hard_delete:" << err.what();
	}

	try {
		sol::table zone_tbl = tbl.get<sol::table>("zone_server");
		
		std::string ip_addr = zone_tbl.get_or("ip_address", std::string("127.0.0.1"));
		int32_t port = zone_tbl.get_or("port", 5121);

		config().set_zone_server_ip(ip_addr);
		config().set_zone_server_port(port);
		HLog(info) << "Zone Server set to '" << config().zone_server_ip() << "' port: " << config().zone_server_port() << ".";
	} catch (sol::error &err) {
		HLog(error) << "Error for setting zone_server:" << err.what();
	}

	try {
		int32_t pincode_expiry = tbl.get<int32_t>("pincode_expiry");
		config().set_pincode_expiry(pincode_expiry);
		HLog(info) << "Pincode expiry to '" << config().pincode_expiry() << "'.";
	} catch (sol::error &err) {
		HLog(error) << "Error for setting pincode_expiry:" << err.what();
	}

	try {
		int32_t pincode_max_retry = tbl.get<uint32_t>("pincode_max_retry");
		config().set_pincode_max_retry(pincode_max_retry);
		HLog(info) << "Pincode max retry to '" << config().pincode_max_retry() << "'.";
	} catch (sol::error &err) {
		HLog(error) << "Error for setting pincode_max_retry:" << err.what();
	}

	try {
		int32_t session_max_timeout = tbl.get_or("session_max_timeout", 60);
		config().set_session_max_timeout(session_max_timeout);
		HLog(info) << "Session maximum timeout set to '" << config().session_max_timeout() << "',"
			<< " sessions surpassing this time limit will be disconnected.";
	} catch (sol::error &err) {
		HLog(error) << "Error for setting session_max_timeout:" << err.what();
	}

	// Max Network Threads
	try {
		int32_t max_network_threads = tbl.get_or("max_network_threads", 1);
		config().set_max_network_threads(max_network_threads);
		HLog(info) << "Max Network Threads set to '" << config().max_network_threads() << "'.";
	} catch (sol::error &err) {
		HLog(error) << "Error for setting max_network_threads:" << err.what();
	}

	/**
	 * Process Configuration that is common between servers.
	 */
	if (!parse_common_configs(tbl))
		return false;

	HLog(info) << "Done reading server configurations from '" << file_path << "'.";

	return true;
}

#undef char_config_error

/* Initialize Char-Server CLI Commands */
void CharServer::initialize_cli_commands()
{
}

/**
 * Signal handler for the Char-Server main thread.
 * @param[in|out] error   boost system error code.
 * @param[in]     signal  interrupt signal code
 */
void SignalHandler(int signal_num)
{
	if (sChar->get_signal_interrupt_command_line_loop() == true) {
		sChar->set_signal_interrupt_command_line_loop(false);
		// re-set the signal handler
		signal(signal_num, SignalHandler);
	} else {
		set_shutdown_stage(SHUTDOWN_INITIATED);
		set_shutdown_signal(signal_num);
	}
}

void CharServer::verify_connected_sessions()
{	
	try {
		std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
		boost::mysql::statement stmt = conn->prepare_statement("DELETE FROM `session_data` WHERE `current_server` = ? AND `last_update` < ?");
		auto b = stmt.bind("C", std::time(nullptr) - config().session_max_timeout());
		boost::mysql::results results;
		conn->execute(b, results);

		stmt = conn->prepare_statement("SELECT COUNT(`game_account_id`) FROM `session_data` WHERE `current_server` = ?");
		auto b2 = stmt.bind("C");
		conn->execute(b2, results);

		if (results.rows().empty()) {
			HLog(info) << "There are no connected session(s).";
			return;
		}

		int32_t count = results.rows()[0][0].as_int64();

		HLog(info) << count << " connected session(s).";

	}
	catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << error.what();
	}
	catch (std::exception& error) {
		HLog(error) << error.what();
	}
}

void CharServer::update(uint64_t time)
{
	getScheduler().Update();

	sClientSocketMgr->manage_sockets(time);
	sClientSocketMgr->update_sessions(time);
	
	if (get_shutdown_stage() == SHUTDOWN_NOT_STARTED && !general_conf().is_test_run_minimal()) {
		_update_timer.expires_from_now(boost::posix_time::microseconds(MAX_CORE_UPDATE_INTERVAL));
		_update_timer.async_wait(std::bind(&CharServer::update, this, std::time(nullptr)));
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

void CharServer::initialize()
{
	/* Core Signal Handler  */
	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);
#ifndef WIN32
	signal(SIGQUIT, SignalHandler);
#endif

	/* Start Character Network */
	sClientSocketMgr->start(get_io_context(),
						  general_conf().get_listen_ip(),
						  general_conf().get_listen_port(),
						  config().max_network_threads(),
						  general_conf().is_test_run() && general_conf().is_test_run_with_network() == false);

	Server::initialize();

	getScheduler().Schedule(Seconds(60), [this] (TaskContext context) {
		verify_connected_sessions();
		context.Repeat();
	});

	_update_timer.expires_from_now(boost::posix_time::seconds(0));
	_update_timer.async_wait(std::bind(&CharServer::update, this, std::time(nullptr)));
	
	Server::post_initialize();
	
	// Run the io_context until stop is called from the internal, finalizing thread.
	// After stopping, execution will continue through the next line onwards.
	// We actually finalize on this thread and not in any of io_context's internal threads.
	get_io_context().run();

	/* Core Cleanup */
	HLog(info) << "Server shutting down...";
	
	HLogShutdown;

	set_shutdown_stage(SHUTDOWN_CLEANUP_COMPLETE);
}

void CharServer::finalize()
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