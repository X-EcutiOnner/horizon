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
#include "Server/Char/Char.hpp"
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

#define BOOST_TEST_MODULE AuthServerTest
#include <boost/test/included/unit_test.hpp>

// One one test in this file as the server is a singleton and we can't have multiple instances of it.
// If another test was created, the object would be destroyed and the second test would fail / crash.
BOOST_AUTO_TEST_CASE(CharServerTest)
{
	try {
		sChar->general_conf().set_test_run(TEST_RUN_MINIMAL);

		sChar->general_conf().set_config_file_path("config/char-server.lua.dist");
		sChar->read_config();

		set_shutdown_stage(SHUTDOWN_INITIATED);
		sChar->initialize();
	} catch(std::length_error &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(std::bad_alloc &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(std::exception &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception caught." << std::endl;
	}
}