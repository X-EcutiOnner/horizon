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


#define BOOST_TEST_MODULE "MySQLTest"

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

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>


struct test{
    int idtest;
    int testint;
    std::string teststring;
    bool testbool;
    boost::mysql::date testdate;
    boost::mysql::datetime testdatetime;
};
BOOST_DESCRIBE_STRUCT(test, (), (idtest, testint, teststring, testbool, testdate, testdatetime))

void test_connection(std::shared_ptr<boost::mysql::tcp_ssl_connection> conn)
{
    boost::mysql::statement stmt;
    boost::mysql::results results;
    boost::mysql::static_results<std::tuple<>> empty_result;
    conn->execute("CREATE DATABASE IF NOT EXISTS `horizon`", results);

    try {
        conn->execute("CREATE TABLE IF NOT EXISTS `horizon`.`test` ( \
            `idtest` INT NOT NULL, \
            `testint` INT NOT NULL, \
            `teststring` VARCHAR(45) NOT NULL, \
            `testbool` TINYINT NOT NULL, \
            `testdate` DATE NOT NULL, \
            `testdatetime` DATETIME NOT NULL, \
            PRIMARY KEY(`idtest`)); \
        ", empty_result);
        std::cout << "Created Table `horizon`.`test`..." << std::endl;

        stmt = conn->prepare_statement("INSERT INTO `horizon`.`test` (`idtest`, `testint`, `teststring`, `testbool`, `testdate`, `testdatetime`) VALUES (?, ?, ?, ?, ?, ?);");
        auto b = stmt.bind(1, 100, "test string", true, "1990-10-01", "1990-11-01 01:01:01");
        conn->execute(b, empty_result);

        std::cout << "Inserted 1 column into `horizon`.`test`..." << std::endl;
    }
    catch (boost::mysql::error_with_diagnostics& error) {
        std::cout << "error:" << error.what() << std::endl;
    }
    
    try {
        std::cout << "Fetching 1 column from `horizon`.`test`..." << std::endl;
        stmt = conn->prepare_statement("SELECT `idtest`, `testint`, `teststring`, `testbool`, `testdate`, `testdatetime` FROM `horizon`.`test` WHERE `idtest` = ?");
        auto b1 = stmt.bind(1);

        boost::mysql::static_results<test> test_rows;

        conn->execute(b1, test_rows);

        if (test_rows.rows().empty()) {
            BOOST_TEST_FAIL("Failed to retrieve rows for test table");
        }
        
        const test& t = test_rows.rows()[0];

        BOOST_CHECK_EQUAL(t.idtest, 1);
        BOOST_CHECK_EQUAL(t.testint, 100);
        BOOST_CHECK_EQUAL(t.teststring.compare("test string"), 0);
        BOOST_CHECK_EQUAL(t.testbool, true);
        BOOST_CHECK_EQUAL(t.testdate.day(), 01);//654719400);
        BOOST_CHECK_EQUAL(t.testdatetime.minute(), 01); // 657401461);

        std::cout << "Dropping table `horizon`.`test`..." << std::endl;
        
        conn->execute("DROP TABLE `horizon`.`test`;", empty_result);

        std::cout << "Executed all queries successfully." << std::endl;
    }
    catch (boost::mysql::error_with_diagnostics& error) {
        std::cout << "error:" << error.what() << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(MySQLTest)
{
	try {
		std::string host = "localhost";
		int port = 3306;
		std::string user = "horizon";
		std::string pass = "horizon";
		std::string schema = "horizon";

		boost::asio::io_context ctx;
		boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tls_client);

		std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = std::make_shared<boost::mysql::tcp_ssl_connection>(ctx.get_executor(), ssl_ctx);

		boost::asio::ip::tcp::resolver resolver(ctx.get_executor());

		auto endpoints = resolver.resolve(host, boost::mysql::default_port_string);

		boost::mysql::handshake_params params(user, pass, schema);

		conn->connect(*endpoints.begin(), params);

		const char *sql = "SELECT 'Hello World!'";
		boost::mysql::results result;
		conn->execute(sql, result);

		std::cout << "Result: " << result.rows().at(0).at(0) << std::endl;

		test_connection(conn);
			
		conn->close();
	} catch(boost::mysql::error_with_diagnostics &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(std::exception &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception caught." << std::endl;
	}
}
