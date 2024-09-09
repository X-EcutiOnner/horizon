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

#include "Server.hpp"

#include "Core/Logging/Logger.hpp"
#include "Server/Common/Configuration/Horizon.hpp"
#include "Libraries/Networking/Buffer/ByteBuffer.hpp"
#include "version.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <iomanip>
#include <signal.h>

#if WIN32
#include <windows.h>
#elif __linux__
#include <ctime>
#include <thread>
#endif

std::atomic<shutdown_stages> _shutdown_stage = SHUTDOWN_NOT_STARTED;
std::atomic<int> _shutdown_signal = 0;

Kernel::Kernel(general_server_configuration &conf) : _config(conf), _hsr_manager(Horizon::System::RUNTIME_MAIN) { }
Kernel::~Kernel() 
{
	for (auto it = _components.begin(); it != _components.end();) {
		it->second.ptr.reset();
		it = _components.erase(it);
	}
}

void Kernel::system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContext> context) { _hsr_manager.push(context); }
void Kernel::system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContextChain> context) { _hsr_manager.push(context); }
void Kernel::system_routine_process_queue() { _hsr_manager.process_queue(); }
void Kernel::system_routine_register(Horizon::System::runtime_module_type module_t, Horizon::System::runtime_synchronization_method sync_t, std::shared_ptr<Horizon::System::RuntimeContext> context)
{
	_hsr_manager.register_(module_t, sync_t, context);
}

boost::asio::io_context &Kernel::get_io_context()
{
	return _io_context_global;
}

void KernelComponent::system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContext> context) { _hsr_manager.push(context); }
void KernelComponent::system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContextChain> context) { _hsr_manager.push(context); }
void KernelComponent::system_routine_process_queue() { _hsr_manager.process_queue(); }
void KernelComponent::system_routine_register(Horizon::System::runtime_module_type module_t, Horizon::System::runtime_synchronization_method sync_t, std::shared_ptr<Horizon::System::RuntimeContext> context)
{
	_hsr_manager.register_(module_t, sync_t, context);
}

void Kernel::post_initialize()
{

}

void Kernel::post_finalize()
{

}

bool CommandLineProcess::clicmd_shutdown(std::string /*cmd*/)
{
	set_shutdown_stage(SHUTDOWN_INITIATED);
	set_shutdown_signal(SIGTERM);
	return true;
}

bool CommandLineProcess::clicmd_kernel_info(std::string /*cmd*/)
{
    std::string red = "\033[31m";
    std::string green = "\033[32m";
    std::string reset = "\033[0m";

	get_kernel()->set_signal_interrupt_command_line_loop(true);
	HLog(info) << "Horizon Kernel (C) 2024 Initialized with " << get_kernel()->get_components().size() << " components.";

	while (get_kernel()->get_signal_interrupt_command_line_loop() == true) {
		int total_execution_time = 0.0;

       	std::cout << "[Status]"
			<< std::setw(15) << "Component (Segment #)"
			<< std::setw(15) << "CPU #"
			<< std::setw(15) << "Execution Time"
			<< std::setw(15) << "Update Rate"
			<< std::flush << std::endl;
		for (auto i = get_kernel()->get_components().begin(); i != get_kernel()->get_components().end(); i++) {
			std::string online = std::string(red + "Offline" + reset);
			if (i->second.ptr->is_initialized() == true)
				online = std::string(green + "Online" + reset);
			std::cout << "\r\033[K" << std::flush;
			
			double limited_update_rate = std::round(i->second.ptr->get_thread_update_rate() * 100.0) / 100.0;

			total_execution_time += i->second.ptr->get_total_execution_time();
       		std::cout << "[" << online << "]"
				<< std::setw(15) << i->second.ptr->get_type_string() << " (" << i->second.segment_number << ")"
				<< std::setw(15) << i->second.ptr->get_thread_cpu_id()
				<< std::setw(15) << i->second.ptr->get_total_execution_time() << "ns"
				<< std::setw(15) << limited_update_rate << "/s." 
				<< std::flush << std::endl;
		}

		std::cout << "Total Execution Time: " << total_execution_time << "ns" << std::endl;
		
        std::cout << "\033[" << get_kernel()->get_components().size() + 2 << "A";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	std::cout << "\033[ " << get_kernel()->get_components().size() + 2 << "B";
	return true;
}

void CommandLineProcess::initialize(int segment_number)
{

	_cli_thread = std::thread(std::bind(&CommandLineProcess::cli_thread_start, this));

	add_function("shutdown", std::bind(&CommandLineProcess::clicmd_shutdown, this, std::placeholders::_1));
	add_function("kernel-info", std::bind(&CommandLineProcess::clicmd_kernel_info, this, std::placeholders::_1));
	set_segment_number(segment_number);

	_is_initialized.exchange(true);
}

void CommandLineProcess::finalize()
{
	if (_cli_thread.joinable())
		_cli_thread.detach();

	_is_finalized.exchange(true);
}

void CommandLineProcess::process()
{
	while (_cli_cmd_queue.size()) {
		/**
		 * Error on this line, need to fix it. Particularly with VS Code debugger. It is probably only in the debugger because it acts wierd with line inputs.
		 * We will disable the command line on test runs for now.
		 * Exception thrown at 0x00007FFA60FAF39C in ZoneSystemTest.exe: Microsoft C++ exception: std::bad_alloc at memory location 0x000000851FAFE830.
		 * Failed to process command line input: bad allocation
		 */
		try {
			CLICommand command = _cli_cmd_queue.front();
			_cli_cmd_queue.pop();

			bool ret = false;
			std::vector<std::string> separated_args;
			boost::algorithm::split(separated_args, command.m_command, boost::algorithm::is_any_of(" "));
		
			std::function<bool(std::string)> cmd_func = find(separated_args[0]);
		
			if (cmd_func) {
				ret = cmd_func(command.m_command);
				_is_running_command.exchange(false);
			}
		
			if (command.m_finish_func != nullptr)
				command.m_finish_func(command, ret);
		} catch(std::exception &e) {
			HLog(error) << "Failed to process command line input: " << e.what();
			return;
		}
	}
}

void CommandLineProcess::cli_thread_start()
{
	while (get_shutdown_stage() == SHUTDOWN_NOT_STARTED)
	{
		std::string command;

		process();
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

		try {
			while(_is_running_command.load() == true)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
			};
			std::cout << TERMINAL_STR;
			std::getline(std::cin, command);
			std::flush(std::cout);
			if (command.size()) {
				std::vector<std::string> separated_args;
				boost::algorithm::split(separated_args, command, boost::algorithm::is_any_of(" "));

				if (find(separated_args[0]) == nullptr)
					HLog(error) << "Command '" << separated_args[0] << "' not found.";
				else
					_is_running_command.exchange(true);
			}
		} catch(std::bad_alloc &e) {
			HLog(error) << "Failed to allocate memory for command line input.";
			set_shutdown_stage(SHUTDOWN_INITIATED);
			set_shutdown_signal(SIGTERM);
			break;
		} catch(std::length_error &e) {
			HLog(error) << "Command line input too long.";
			set_shutdown_stage(SHUTDOWN_INITIATED);
			set_shutdown_signal(SIGTERM);
			break;
		}

		if (command.size()) {
			queue(CLICommand((char *)command.c_str(), std::bind(&CommandLineProcess::command_complete, this, std::placeholders::_1, std::placeholders::_2)));	
			std::this_thread::sleep_for(std::chrono::microseconds(MAX_CORE_UPDATE_INTERVAL * 1)); // Sleep until core has updated.
		} else if (feof(stdin)) {
			set_shutdown_stage(SHUTDOWN_INITIATED);
			set_shutdown_signal(SIGTERM);
		}
	}
}

void DatabaseProcess::initialize(boost::asio::io_context &io_context, int segment_number, std::string host, int port, std::string user, std::string pass, std::string database)
{
	set_segment_number(segment_number);
	
	try {
		_ssl_ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tls_client);
		_connection = std::make_shared<boost::mysql::tcp_ssl_connection>(io_context.get_executor(), *_ssl_ctx);
		boost::asio::ip::tcp::resolver resolver(io_context.get_executor());
		auto endpoints = resolver.resolve(host, std::to_string(port));
		boost::mysql::handshake_params params(user, pass, database);
		_connection->connect(*endpoints.begin(), params);
		_is_initialized.exchange(true);
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
	} catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << error.what();
	}
}

/* Public */
Server::Server() : Kernel(general_conf())
{   
	auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

	HLog(info) << "Copyright (c) https://github.com/horizonxyz/horizon";
	HLog(info) << "Date: " << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
	HLog(info) << "Compile CXX Flags: " << _CMAKE_CXX_FLAGS;
	HLog(info) << "Version: " << VER_PRODUCTVERSION_STR;
	HLog(info) << "Last Update: " << _DATE;
	HLog(info) << "Branch: " << _BRANCH;
	HLog(info) << "Copyright: " << VER_LEGALCOPYRIGHT_STR;
	HLog(info) << "Boost Version: v" << (BOOST_VERSION / 100000) << "." << (BOOST_VERSION / 100 % 1000) << "." << (BOOST_VERSION % 100);
    HLog(info) << "Client Information: " << CLIENT_TYPE << " " << PACKET_VERSION;
}

Server::~Server()
{
}

void Server::print_help()
{
	HLog(error) << "usage: <server> [--version] [--help] [--with-config=<file_with_path>]";
}

void Server::parse_exec_args(const char *argv[], int argc)
{
	std::string started_with_args;
    for (int i = 1; i < argc; ++i) {
		started_with_args.append(argv[i]).append(" ");

        std::string arg(argv[i]);
        std::vector<std::string> separated_args;
        boost::algorithm::split(separated_args, arg, boost::algorithm::is_any_of("="));
        
		if (separated_args.size() == 1) {
            std::string arg = separated_args.at(0);
            if (arg.compare("--test-run") == 0) {
                HLog(info) << "Horizon test mode (without command-line, network) intiated.";
                general_conf().set_test_run(TEST_RUN_MINIMAL);
            } else if (arg.compare("--test-run-with-network") == 0) {
                HLog(info) << "Horizon test mode (with network) intiated.";
                general_conf().set_test_run(TEST_RUN_WITH_NETWORK);
            } else if (arg.compare("--help") == 0) {
				print_help();
			} else if (arg.compare("--version") == 0) {
				HLog(info) << "Version: " << VER_PRODUCTVERSION_STR;
			}
        } else if (separated_args.size() == 0) { 
			HLog(error) << "Horizon started with no command line arguments.";
			print_help();
		} else {
            std::string arg = separated_args.at(0);
            std::string val = separated_args.at(1);
            
            if (arg.compare("--with-config") == 0) {
                HLog(info) << "Loading configurations from '" << val << "'.";
                general_conf().set_config_file_path(val);
                if (!exists(general_conf().get_config_file_path())) {
                    HLog(error) << "Configuration file path '" << val << "' does not exist!";
                    continue;
                }
            }
        }
    }

	HLog(info) << "Started with args:" << started_with_args;
}

#define core_config_error(setting_name, default) \
HLog(error) << "No setting for " << setting_name << " in configuration file, defaulting to " << default;

bool Server::parse_common_configs(sol::table &tbl)
{
	std::string tmp_string{""};

	general_conf().set_listen_ip(tbl.get_or("bind_ip", std::string("127.0.0.1")));
	general_conf().set_listen_port(tbl.get_or("bind_port", 0));
	
	if (general_conf().get_listen_port() == 0) {
		HLog(error) << "Invalid or non-existent configuration for 'bind_port', Halting...";
		return false;
	}
	
	sol::table db_tbl = tbl.get<sol::table>("database_config");
	
	try {
		general_conf().set_db_host(db_tbl.get_or<std::string>("host", "127.0.0.1"));
		general_conf().set_db_user(db_tbl.get_or<std::string>("user", "horizon"));
		general_conf().set_db_database(db_tbl.get_or<std::string>("db", "horizon"));
		general_conf().set_db_pass(db_tbl.get_or<std::string>("pass", "horizon"));
		general_conf().set_db_port(db_tbl.get_or<uint16_t>("port", 33060));
		
		register_component(Horizon::System::RUNTIME_DATABASE, std::make_shared<DatabaseProcess>(this));
		
		get_component_of_type<DatabaseProcess>(Horizon::System::RUNTIME_DATABASE)->initialize(
			get_io_context(),
			1,
			general_conf().get_db_host(), 
			general_conf().get_db_port(), 
			general_conf().get_db_user(), 
			general_conf().get_db_pass(), 
			general_conf().get_db_database());
		
		HLog(info) << "Database tcp://" << general_conf().get_db_user()
			<< ":" << general_conf().get_db_pass()
			<< "@" << general_conf().get_db_host()
			<< ":" << general_conf().get_db_port()
			<< "/" << general_conf().get_db_database()
			<< (test_database_connection() ? " (connected)" : "(not connected)");
	}
	catch (const boost::mysql::error_with_diagnostics &error) {
		HLog(error) << error.what() << ".";
		return false;
	}
	catch (const std::exception &error) {
		HLog(error) << error.what() << ".";
		return false;
	}

	return true;
}

#undef core_config_error

void Server::initialize()
{
	/**
	 * Initialize Commandline Interface
	 */
	if (general_conf().is_test_run() && general_conf().is_test_run_minimal()) {
		HLog(info) << "Command line not supported during test-runs... skipping.";
	} else {
		HLog(info) << "Horizon Command-Line initializing...";
		register_component(Horizon::System::RUNTIME_COMMANDLINE, std::make_shared<CommandLineProcess>(this));
		get_component_of_type<CommandLineProcess>(Horizon::System::RUNTIME_COMMANDLINE)->initialize();
	}
}

void Server::finalize()
{
	if (!general_conf().is_test_run() && !general_conf().is_test_run_minimal()) {
		get_component_of_type<CommandLineProcess>(Horizon::System::RUNTIME_COMMANDLINE)->finalize();
		deregister_component(Horizon::System::RUNTIME_COMMANDLINE);
	}
	
	get_component_of_type<DatabaseProcess>(Horizon::System::RUNTIME_DATABASE)->finalize();
	deregister_component(Horizon::System::RUNTIME_DATABASE);
}

void Server::post_initialize()
{
	Kernel::post_initialize();
	
	for (auto i = _components.begin(); i != _components.end(); i++) {
		HLog(info) << "Kernel component '" << i->second.ptr->get_type_string()  << " (" << i->second.segment_number << ")': " << (i->second.ptr->is_initialized() == true ? "Online" : "Offline (Starting)") << " { CPU: " << i->second.ptr->get_thread_cpu_id() << ", uuid: " << i->first << " }";
	}
}

void Server::post_finalize()
{
	Kernel::post_finalize();
}

bool Server::test_database_connection()
{
	try {
		const char *sql = "SELECT 'Hello World!'";
		boost::mysql::results result;
		get_database_connection()->execute(sql, result);
		if (result.rows().at(0).at(0).as_string().compare("Hello World!") == 0)
			return true;
	} catch (boost::mysql::error_with_diagnostics &error) {
		HLog(error) << error.what();
	}
	return false;
}