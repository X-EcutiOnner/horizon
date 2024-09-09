/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2020- Horizon Dev Team.
 *
 * Base Author - Sagun Khosla. (sagunxp@gmail.com)
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

#include "AuthClientInterface.hpp"

#include "Server/Common/Configuration/Database.hpp"

#include "Server/Auth/Auth.hpp"
#include "Server/Auth/Packets/TransmittedPackets.hpp"
#include "Server/Auth/Session/AuthSession.hpp"
#include "Server/Auth/Socket/AuthSocket.hpp"

using namespace Horizon::Auth;

AuthClientInterface::AuthClientInterface(std::shared_ptr<AuthSession> s)
: ClientInterface(s)
{
	
}

AuthClientInterface::~AuthClientInterface()
{
	
}

bool AuthClientInterface::process_login(std::string username, std::string password, uint16_t version, uint16_t client_type)
{
	AC_ACCEPT_LOGIN acal(get_session());
	AC_REFUSE_LOGIN acrl(get_session());
	int pos = 0;
	char block_date[20]{ 0 };

	if (((pos = username.find("_m")) || (pos = username.find("_f"))) && pos != std::string::npos) {
		enum game_account_gender_type gender = ACCOUNT_GENDER_NONE;

		if (username.find("_m")) {
			username = username.substr(0, pos);
			gender = ACCOUNT_GENDER_MALE;
		}
		else if (username.find("_f")) {
			username = username.substr(0, pos);
			gender = ACCOUNT_GENDER_FEMALE;
		}
	
		std::vector<unsigned char> salt;
		std::vector<unsigned char> hash;

		auto conn = sAuth->get_database_connection();
		try {
			boost::mysql::statement stmt = conn->prepare_statement("SELECT * FROM `game_accounts` WHERE `username` = ?");
			auto b1 = stmt.bind(username);
			boost::mysql::results results;
			conn->execute(b1, results);

			if (results.rows().empty()) {
				HLog(info) << "Recieved connection request for unknown account '" << username << "'.";

				HLog(info) << "Creating a new account for user '" << username << "' with password '" << password << "'.";

				sAuth->generate_salt(salt);

				sAuth->hash_password(password, salt, hash);

				try {
					boost::mysql::statement stmt = conn->prepare_statement("INSERT INTO `game_accounts` (`username`, `hash`, `salt`, `email`, `birth_date`, `gender`, `group_id`, `state`, `login_count`, `last_login`, `last_ip`, `character_slots`, `pincode`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
					auto b2 = stmt.bind(username, hash, salt, "a@a.com", "0000-00-00", gender == ACCOUNT_GENDER_MALE ? "M" : (gender == ACCOUNT_GENDER_FEMALE ? "F" : "NA"), 0, (int)ACCOUNT_STATE_NONE, 1, boost::mysql::date::now(), get_session()->get_socket()->remote_ip_address(), 3, "0000");
					boost::mysql::results results2;
					conn->execute(b2, results2);
					uint32_t aid = results2.last_insert_id();
					acal.deliver(aid, aid, 0, gender);

					stmt = conn->prepare_statement("REPLACE INTO `session_data` (`auth_code`, `game_account_id`, `client_version`, `client_type`, `character_slots`, `group_id`, `connect_time`, `current_server`) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
					auto b3 = stmt.bind(results2.last_insert_id(), aid, PACKET_VERSION, client_type, 3, 0, std::time(nullptr), "A");
					boost::mysql::results results3;
					conn->execute(b3, results3);

					HLog(info) << "Session (" << aid << ") has been initiated.";
					HLog(info) << "Request for authorization of account '" << username << "' (" << aid << ")" << " has been granted.";
				}
				catch (std::exception& error) {
					HLog(error) << error.what();
					acrl.deliver(login_error_codes::ERR_SESSION_CONNECTED, block_date, 0);
					return false;
				}
				return true;
			}
		}
		catch (std::exception& error) {
			HLog(error) << error.what();
		}
		
		acrl.deliver(login_error_codes::ERR_SESSION_CONNECTED, block_date, 0);
		return false;
	}
	
	try
	{
		auto conn = sAuth->get_database_connection();
		boost::mysql::statement stmt = conn->prepare_statement("SELECT * FROM `game_accounts` WHERE `username` = ?");
		auto b1 = stmt.bind(username);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (results.rows().empty()) {
			HLog(info) << "Recieved connection request for unknown account '" << username << "'.";
			acrl.deliver(login_error_codes::ERR_UNREGISTERED_ID, block_date, 0);
			return false;
		}

		std::vector<unsigned char> hash(results.rows()[0][2].as_blob().begin(), results.rows()[0][2].as_blob().end());
		std::vector<unsigned char> salt(results.rows()[0][3].as_blob().begin(), results.rows()[0][3].as_blob().end());

		std::vector<unsigned char> hash_check;

		sAuth->hash_password(password, salt, hash_check);

		// compare salt_vec with salt and hash_vec with hash
		if (hash_check != hash) {
			HLog(info) << "Incorrect password for account '" << username << "' with password '" << password << "'.";
			acrl.deliver(login_error_codes::ERR_INCORRECT_PASSWORD, block_date, 0);
			return false;
		}
		
		uint32_t aid = results.rows()[0][0].as_uint64();
		uint32_t group_id = results.rows()[0][6].as_int64();
		uint32_t gender = results.rows()[0][4].as_string().compare("M") == 0 ? 0 : 1;

		stmt = conn->prepare_statement("SELECT * FROM `session_data` WHERE `game_account_id` = ?");
		auto b2 = stmt.bind(aid);
		boost::mysql::results r2;
		conn->execute(b2, r2);

		if (!r2.rows().empty()) {
			acrl.deliver(login_error_codes::ERR_SESSION_CONNECTED, block_date, 0);
			return false;
		}
		else {
			stmt = conn->prepare_statement("INSERT INTO `session_data` (`auth_code`, `game_account_id`, `client_version`, `client_type`, `character_slots`, `group_id`, `connect_time`, `current_server`) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
			auto b3 = stmt.bind(aid, aid, PACKET_VERSION, client_type, 3, 0, std::time(nullptr), "A");
			boost::mysql::results r3;
			conn->execute(b3, r3);

			HLog(info) << "Session (" << aid << ") has been initiated.";
		}

		acal.deliver(aid, aid, group_id, gender);

		HLog(info) << "Request for authorization of account '" << username << "' (" << aid << ")" << " has been granted.";
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << err.what();
		return false;
	}
	catch (std::exception& err) {
		HLog(error) << err.what();
		return false;
	}
	
	return true;
}

bool AuthClientInterface::client_login_otp_response()
{
	AC_LOGIN_OTP pkt(get_session());
	pkt.deliver();
	return true;
}