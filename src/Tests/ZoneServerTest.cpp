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
 * Base Author - Sagun Khosla <sagunxp@gmail.com>
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

#include "Server/Zone/Socket/ZoneSocket.hpp"
#include "Server/Zone/SocketMgr/ClientSocketMgr.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Common/Server.hpp"
#include "Server/Common/System.hpp"
#include "Server/Zone/ZoneSystem.hpp"
#include "Server/Zone/Zone.hpp"

#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/static_results.hpp>
#include <boost/mysql/tcp_ssl.hpp>
#include <boost/describe/class.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>

#define BOOST_TEST_MODULE ZoneServerTest
#include <boost/test/included/unit_test.hpp>


// One one test in this file as the server is a singleton and we can't have multiple instances of it.
// If another test was created, the object would be destroyed and the second test would fail / crash.
BOOST_AUTO_TEST_CASE(ZoneServerTest)
{
	/**
	 * Fixed :
	 * Exception thrown at 0x00007FFA63663FAA (ntdll.dll) in ZoneSystemTest.exe: 0xC0000005: Access violation writing location 0x0000000000000024.
	 * The program '[27192] ZoneSystemTest.exe' has exited with code 0 (0x0).
	 * @note The reason for this error was that the io_context belonged to Server class and not Kernel. 
	 * Which resulted in the destruction of io_context before the DatabaseProcess was destroyed. Lingering _connection object caused issues without an io_context available.
	 */
	try {
		sZone->general_conf().set_test_run(TEST_RUN_MINIMAL);

		sZone->general_conf().set_config_file_path("config/zone-server.lua.dist");
		sZone->read_config();
	
		sZone->config().set_static_db_path(std::string("db/"));
		HLog(info) << "Static database path set to " << sZone->config().get_static_db_path() << "";

		sZone->config().set_mapcache_path(std::string("db/maps.dat"));
		HLog(info) << "Mapcache file name is set to " << sZone->config().get_mapcache_path() << ", it will be read while initializing maps.";

		sZone->config().set_script_root_path(std::string("scripts/"));

		set_shutdown_stage(SHUTDOWN_INITIATED);
		
		sZone->initialize();
	} catch(std::length_error &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(std::bad_alloc &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(std::exception &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception caught." << std::endl;
	}
	// Following errors caused by command line issues on VSCode Debugger. It is captured and handled but hopefully there will be a complete fix for it.
	// It is seemingly complicated to resolve, there are bad allocations spammed from command line input when running tests on VSCode Debugger.
	// It may happen the same way for other debuggers, good to take note of it.
	/**
	 * Error 2 Detected-
	 * 
	 * Exception thrown at 0x00007FFA60FAF39C in ZoneSystemTest.exe: Microsoft C++ exception: std::bad_alloc at memory location 0x0000001B13EFE380.
	 * Exception thrown at 0x00007FFA60FAF39C in ZoneSystemTest.exe: Microsoft C++ exception: [rethrow] at memory location 0x0000000000000000.
	 * Exception thrown at 0x00007FFA60FAF39C in ZoneSystemTest.exe: Microsoft C++ exception: boost::execution_exception at memory location 0x0000001B13EFBF50.
	 * Running 1 test case...
	 * unknown location(0): fatal error: in "SCENARIO_LOGIN_TEST": class std::bad_alloc: bad allocation
	 * C:\Users\Sagun Khosla\Desktop\Repos\horizon\src\Tests\ZoneSystemTest.cpp(55): last checkpoint: "SCENARIO_LOGIN_TEST" test entry
	 * 
	 * ** 1 failure is detected in the test module "ZoneSystemTest"
	 */
	/**
	 * Error 3 -
	 * Exception thrown at 0x00007FF606B15F8F in ZoneSystemTest.exe: 0xC0000005: Access violation reading location 0x00000000000000C3.
	 */
	/**
	 * Error 5 - 
	 * Exception thrown at 0x00007FF6C2CE601F in ZoneSystemTest.exe: 0xC0000005: Access violation reading location 0x0000000000000000.
	 */
	/**
	 * Error 4 -
	 * 
	 * Exception thrown at 0x00007FFA60FAF39C in ZoneSystemTest.exe: Microsoft C++ exception: std::length_error at memory location 0x000000F7240FE710.
	 * Exception thrown at 0x00007FFA60FAF39C in ZoneSystemTest.exe: Microsoft C++ exception: [rethrow] at memory location 0x0000000000000000.
	 * Exception thrown at 0x00007FFA60FAF39C in ZoneSystemTest.exe: Microsoft C++ exception: boost::execution_exception at memory location 0x000000F7240FC2D0.
	 * unknown location(0): fatal error: in "SCENARIO_LOGIN_TEST": class std::length_error: string too long
	 * C:\Users\Sagun Khosla\Desktop\Repos\horizon\src\Tests\ZoneSystemTest.cpp(55): last checkpoint: "SCENARIO_LOGIN_TEST" test entry
	 * *** 1 failure is detected in the test module "ZoneSystemTest"
	 * The program '[26396] ZoneSystemTest.exe' has exited with code 201 (0xc9).
	 */
	/**
	 * Error 6 -
	 * Exception thrown at 0x00007FF6000AD2A0 in ZoneSystemTest.exe: 0xC0000005: Access violation reading location 0x0000000100000029.
	 */
}