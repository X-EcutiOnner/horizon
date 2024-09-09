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

#include "Server/Auth/Auth.hpp"

#define BOOST_TEST_MODULE AuthServerTest
#include <boost/test/included/unit_test.hpp>


// One one test in this file as the server is a singleton and we can't have multiple instances of it.
// If another test was created, the object would be destroyed and the second test would fail / crash.
BOOST_AUTO_TEST_CASE(AuthServerTest)
{
	try {
		sAuth->general_conf().set_test_run(TEST_RUN_MINIMAL);

		sAuth->general_conf().set_config_file_path("config/auth-server.lua.dist");
		sAuth->read_config();
		set_shutdown_signal(SHUTDOWN_INITIATED);
		// Running initialize() in a separate thread will cause a memory leak due to the singleton pattern.
		// Always invoked on the main thread.
		sAuth->initialize();
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

BOOST_AUTO_TEST_CASE(DatabaseProcessTest)
{
    // Feel free to change any of these values to what you see suit
    std::string host = "127.0.0.1";
    int port = 3306;
    std::string user = "horizon";
    std::string pass = "horizon";
    std::string database = "horizon";
    int segment_number = -1;

    // Create the component
    boost::asio::io_context ctx;
    DatabaseProcess proc;

    // Use it
    try {
        proc.initialize(ctx, segment_number, host, port, user, pass, database);
    } catch(const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}