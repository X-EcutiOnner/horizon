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

#include "CharClientInterface.hpp"

#include "Server/Char/Char.hpp"
#include "Server/Char/Socket/CharSocket.hpp"
#include "Server/Char/Session/CharSession.hpp"
#include "Server/Char/Packets/TransmittedPackets.hpp"

#include <memory>

using namespace Horizon::Char;

CharClientInterface::CharClientInterface(std::shared_ptr<CharSession> s)
: ClientInterface(s)
{
	
}

CharClientInterface::~CharClientInterface()
{
}

bool CharClientInterface::authorize_new_connection(uint32_t account_id, uint32_t auth_code, uint32_t account_level, uint8_t gender)
{	
	HLog(warning) << "A new connection has been established from I.P. " << get_session()->get_socket()->remote_ip_address();
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT * FROM `session_data` WHERE `game_account_id` = ?");
		auto b1 = stmt.bind(account_id);
		boost::mysql::results results;
		conn->execute(b1, results);
		
		HC_ACCOUNT_ID hcad(get_session()); // first packet sent no matter what.
		hcad.deliver(account_id);

		if (results.rows().empty()) {
			HC_REFUSE_ENTER pkt(get_session());
			HLog(warning) << "Invalid connection for account with ID " << account_id << ", session wasn't found.";
			pkt.deliver(CHAR_ERR_REJECTED_FROM_SERVER);
			return false;
		}

		stmt = conn->prepare_statement("SELECT * FROM `game_accounts` WHERE `id` = ?");
		auto b2 = stmt.bind(account_id);
		boost::mysql::results results2;
		conn->execute(b2, results2);

		if (results2.rows().empty()) {
			HC_REFUSE_ENTER pkt(get_session());
			HLog(error) << "Game Account with ID " << account_id << " does not exist.";
			pkt.deliver(CHAR_ERR_REJECTED_FROM_SERVER);
			return false;
		}

		HC_ACCEPT_ENTER2 hcae2(get_session());
		HC_ACCEPT_ENTER hcae(get_session());
		HC_BLOCK_CHARACTER hcbc(get_session());

		int character_slots = results2.rows()[0][14].as_uint64();

		hcae2.deliver(character_slots, MAX_CHARACTER_SLOTS - character_slots);

		hcae.prepare(account_id, MAX_CHARACTER_SLOTS, character_slots, MAX_CHARACTER_SLOTS - character_slots);
		hcae.deliver();

		hcbc.deliver();

		s_session_data sd;
		sd._account_id = account_id;
		sd._auth_id = auth_code;
		sd._group_id = account_level;
		sd._connect_time = std::chrono::system_clock::now();
		sd._character_slots = character_slots;
		get_session()->set_session_data(sd);

		{ // Pin-code related
		  // Session data is updated here after being set prior.
			std::string pincode = results2.rows()[0][15].as_string();
			std::time_t pexp_t = results2.rows()[0][16].as_uint64();

			HC_SECOND_PASSWD_LOGIN hcspl(get_session());

			if (pincode.size() == 0)
				hcspl.deliver(PINCODE_REQUEST_NEW);
			else if (std::time(0) > pexp_t)
				hcspl.deliver(PINCODE_REQUEST_CHANGE);
			else
				hcspl.deliver(PINCODE_REQUEST_PIN);
		}

		stmt = conn->prepare_statement("UPDATE `session_data` SET `current_server` = 'C' WHERE `game_account_id` = ?");
		auto b3 = stmt.bind(account_id);
		conn->execute(b3, results);
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::authorize_new_connection:" << err.what();
		return false;
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::authorize_new_connection:" << err.what();
		return false;
	}

	// update with auth code.
	return true;
}

bool CharClientInterface::make_new_character(std::string name, uint8_t slot, uint32_t hair_color, uint32_t hair_style, uint32_t job_class, uint8_t gender)
{	
	HC_REFUSE_MAKECHAR hcref(get_session());
	
	try {
		std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
		boost::mysql::statement stmt = conn->prepare_statement("SELECT * FROM `characters` WHERE `name` = ?");
		auto b1 = stmt.bind(name);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (!results.rows().empty()) {
			hcref.deliver(HC_CREATE_ERROR_ALREADY_EXISTS);
			HLog(info) << "Character creation denied for already existing character '" << name << "'. (Endpoint: " << get_session()->get_socket()->remote_ip_address() << ")";
			return false;
		}

		if (slot > get_session()->get_session_data()._character_slots) {
			hcref.deliver(HC_CREATE_ERROR_CHAR_SLOT);
			return false;
		}

		std::string new_map = sChar->config().start_map();
		uint16_t x = sChar->config().start_x(), y = sChar->config().start_y();

		stmt = conn->prepare_statement("INSERT INTO `characters` (`account_id`, `slot`, `name`, `current_map`, `current_x`, `current_y`, `saved_map`, `saved_x`, `saved_y`, `gender`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		auto b2 = stmt.bind(get_session()->get_session_data()._account_id, slot, name,
				new_map, x, y,
				new_map, x, y, gender ? "M" : "F");
		conn->execute(b2, results);

		int c_last_insert_id = results.last_insert_id();

		stmt = conn->prepare_statement("INSERT INTO `character_status` (`id`, `job_id`, `hair_color_id`, `hair_style_id`) VALUES (?, ?, ?, ?)");
		auto b3 = stmt.bind(c_last_insert_id, job_class, hair_color, hair_style);
		conn->execute(b3, results);

		for (int j = 0; j < sChar->config().start_item_count(); j++) {
			std::pair<uint32_t, uint32_t> p = sChar->config().start_item(j);
			int item = p.first, c = p.second;

			stmt = conn->prepare_statement("INSERT INTO `character_inventory` (`char_id`, `item_id`, `amount`, `is_identified`) VALUES (?, ?, ?, ?)");
			auto b4 = stmt.bind(c_last_insert_id, item, c, 1);
			conn->execute(b4, results);
		}

		uint16_t start_zeny = sChar->config().start_zeny();

		HC_ACCEPT_MAKECHAR am(get_session());
		am.deliver(c_last_insert_id, start_zeny, new_map, name, slot, hair_color, hair_style, job_class, gender);

		HLog(info) << "New character '" << name << "' has been created. (Endpoint: " << get_session()->get_socket()->remote_ip_address() << ")";
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::make_new_character:" << err.what();
		return false;
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::make_new_character:" << err.what();
		return false;
	}

	return true;
}

bool CharClientInterface::make_new_character(std::string name, uint8_t slot, uint8_t hair_color, uint8_t hair_style)
{
	return make_new_character(name, slot, hair_color, hair_style, 0, 99);
}

character_delete_result CharClientInterface::character_delete_soft(uint32_t character_id)
{	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT * FROM `characters` WHERE `id` = ?");
		auto b1 = stmt.bind(character_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (results.rows().empty()) {
			return CHAR_DEL_RESULT_DATABASE_ERR;
		}
		if (results.rows()[0][21].as_uint64() > 0) { // Guild ID
			return CHAR_DEL_RESULT_GUILD_ERR;
		}

		if (results.rows()[0][20].as_uint64() > 0) { // Party ID
			return CHAR_DEL_RESULT_PARTY_ERR;
		}

		std::time_t dt = std::time(nullptr) + sChar->config().character_deletion_time();

		stmt = conn->prepare_statement("UPDATE `characters` SET `delete_reserved_at` = ? WHERE `id` = ?");
		auto b2 = stmt.bind(dt, character_id);
		conn->execute(b2, results);

	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::character_delete_soft:" << err.what();
		return CHAR_DEL_RESULT_DATABASE_ERR;
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::character_delete_soft:" << err.what();
		return CHAR_DEL_RESULT_SYSTEM_ERR;
	}

	return CHAR_DEL_RESULT_SUCCESS;
}

bool CharClientInterface::character_delete_email(uint32_t character_id, std::string email)
{
	HC_REFUSE_DELETECHAR rd(get_session());
	HC_ACCEPT_DELETECHAR ad(get_session());
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	boost::mysql::statement stmt = conn->prepare_statement("SELECT `email` FROM `game_accounts` WHERE `id` = ?");
	auto b1 = stmt.bind(get_session()->get_session_data()._account_id);
	boost::mysql::results results;
	conn->execute(b1, results);

	if (results.rows().empty()) {
		HLog(error) << "There was an error retrieving `email` from `game_accounts` for `id` = " << get_session()->get_session_data()._account_id;
		rd.deliver();
		return false;
	}

	if (email.compare(results.rows()[0][0].as_string().substr(0, 40)) != 0) {
		rd.deliver();
		return false;
	}

	if (character_delete_soft(character_id) != CHAR_DEL_RESULT_SUCCESS) {
		rd.deliver();
		return false;
	}
	
	ad.deliver();
	
	return true;
}

bool CharClientInterface::character_delete_birthdate(uint32_t character_id, std::string birthdate)
{
	HC_DELETE_CHAR3 dc3(get_session());
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `birth_date`, `character_slots` FROM `game_accounts` WHERE `id` = ?");
		auto b1 = stmt.bind(get_session()->get_session_data()._account_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (results.rows().empty()) {
			HLog(error) << "Attempt to delete character with non-existent game-account. (CID: " << character_id << ", AID:" << get_session()->get_session_data()._account_id << ")";
			dc3.deliver(character_id, CHAR_DEL_ACCEPT_RESULT_SYSTEM_ERR);
			return false;
		}

		std::string bd = "000000";

		std::string b = results.rows()[0][0].as_string();
		HLog(debug) << b;

		bd = b.substr(2, 2);
		bd.append(b.substr(5, 2));
		bd.append(b.substr(8, 2));

		if (birthdate.length() > 6)
			birthdate = birthdate.substr(0, 6);

		HLog(debug) << birthdate << " - " << bd;
		if (bd.compare(birthdate) != 0) {
			dc3.deliver(character_id, CHAR_DEL_ACCEPT_RESULT_BIRTHDAY_ERR);
			return false;
		}

		if (sChar->config().char_hard_delete()) {
			boost::mysql::results results;
			stmt = conn->prepare_statement("REMOVE FROM `character_status` WHERE `id` = ?");
			auto b2 = stmt.bind(character_id);
			conn->execute(b2, results);

			stmt = conn->prepare_statement("REMOVE FROM `character_inventory` WHERE `char_id` = ?");
			auto b3 = stmt.bind(character_id);
			conn->execute(b3, results);

			stmt = conn->prepare_statement("REMOVE FROM `characters` WHERE `id` = ?");
			auto b4 = stmt.bind(character_id);
			conn->execute(b4, results);
		}
		else {
			boost::mysql::results results;
			stmt = conn->prepare_statement("UPDATE `characters` SET `deleted_at` = ? WHERE `id` = ?");
			auto b2 = stmt.bind(std::time(nullptr), character_id);
			conn->execute(b2, results);
		}

	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::character_delete_birthdate:" << err.what();
		return false;
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::character_delete_birthdate:" << err.what();
		return false;
	}
	
	dc3.deliver(character_id, CHAR_DEL_ACCEPT_RESULT_SUCCESS);

	HC_ACK_CHARINFO_PER_PAGE chpp(get_session());
	int count = chpp.prepare();
	chpp.deliver();

	// Hercules has this snippet to send an empty packet for the finishing of the previously sent one.
	// Apparently it doesn't trigger the end code in the client if this isn't sent.
	// And this only occurs if the characters sent previously are 3.
	if (count == 3) {
		HC_ACK_CHARINFO_PER_PAGE chpp2(get_session());
		chpp2.prepare(true);
		chpp2.deliver();
	}
	
	return true;
}

bool CharClientInterface::character_delete_reserve(uint32_t character_id)
{
	HC_DELETE_CHAR3_RESERVED dc(get_session());
	
	character_delete_result result = character_delete_soft(character_id);
	
	if (result != CHAR_DEL_RESULT_SUCCESS) {
		dc.deliver(character_id, result, 0);
		return false;
	}
	
	dc.deliver(character_id, CHAR_DEL_RESULT_SUCCESS, sChar->config().character_deletion_time());
	
	return true;
}

bool CharClientInterface::character_delete_cancel(uint32_t char_id)
{
	HC_DELETE_CHAR3_CANCEL dcc(get_session());
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `delete_reserved_at` FROM `characters` WHERE `id` = ?");
		boost::mysql::results results;
		auto b1 = stmt.bind(char_id);
		conn->execute(b1, results);

		if (results.rows().empty()) {
			dcc.deliver(char_id, CHAR3_DEL_CANCEL_FAILURE);
			return false;
		}

		stmt = conn->prepare_statement("UPDATE `characters` SET `delete_reserved_at` = ? WHERE `id` = ?");
		auto b2 = stmt.bind(0, char_id);
		conn->execute(b2, results);
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::character_delete_cancel:" << err.what();
		return false;
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::character_delete_cancel:" << err.what();
		return false;
	}
	
	dcc.deliver(char_id, CHAR3_DEL_CANCEL_SUCCESS);

	return true;
}

bool CharClientInterface::pincode_create(uint32_t account_id, char *pincode)
{
	HC_MAKE_SECOND_PASSWD makestate(get_session());
	HC_SECOND_PASSWD_LOGIN login(get_session());
	
	char decrypted[5] = "\0\0\0\0";
	
	uint32_t seed = get_session()->get_session_data()._pincode_seed;
	pincode_decrypt(seed, pincode, decrypted);
	
	{
		uint32_t pincode_expiry = std::time(nullptr) + sChar->config().pincode_expiry();
		std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
		try {
			boost::mysql::statement stmt = conn->prepare_statement("UPDATE `game_accounts` SET `pincode` = ?, `pincode_expiry` = ? WHERE `id` = ? ");
			auto b1 = stmt.bind(decrypted, pincode_expiry, account_id);
			boost::mysql::results results;
			conn->execute(b1, results);
		}
		catch (boost::mysql::error_with_diagnostics &err) {
			HLog(error) << "CharClientInterface::pincode_create:" << err.what();
			return false;
		}
	}
	
	strncpy(_pincode_confirm, decrypted, MAX_PINCODE_STRING_LENGTH);
	makestate.deliver(PINCODE_MAKE_SUCCESS);
	login.deliver(PINCODE_REQUEST_PIN);
	return true;
}

bool CharClientInterface::pincode_change(uint32_t account_id, char *old_pin, char *new_pin)
{
	HC_SECOND_PASSWD_LOGIN sp(get_session());
	HC_EDIT_SECOND_PASSWD esp(get_session());

	char old_decrypted[5] = "\0\0\0\0", new_decrypted[5] = "\0\0\0\0";
	
	uint32_t seed = get_session()->get_session_data()._pincode_seed;
	
	pincode_decrypt(seed, old_pin, old_decrypted);
	pincode_decrypt(seed, new_pin, new_decrypted);
	
	std::cout << old_decrypted << " - " << new_decrypted << std::endl;
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `pincode` FROM `game_accounts` WHERE `id` = ?");
		auto b1 = stmt.bind(account_id);
		boost::mysql::results results;
		conn->execute(b1, results);

		std::string s_pincode = results.rows()[0][0].as_string();

		if (s_pincode.compare(old_decrypted) != 0) {
			esp.deliver(PINCODE_EDIT_FAILED);
#if CLIENT_TYPE == 'M' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180131
			sp.deliver(PINCODE_INCORRECT, PINCODE_LOGIN_FLAG_WRONG);
#else
			sp.deliver(PINCODE_INCORRECT);
#endif
			return false;
		}

		{
			uint32_t pincode_expiry = std::time(nullptr) + sChar->config().pincode_expiry();
			stmt = conn->prepare_statement("UPDATE `game_accounts` SET `pincode` = ?, `pincode_expiry` = ? WHERE `id` = ?");
			boost::mysql::results results;
			auto b2 = stmt.bind(new_decrypted, pincode_expiry, account_id);
			conn->execute(b2, results);
		}
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::pincode_change: " << err.what();
		return false;
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::pincode_change: " << err.what();
		return false;
	}
	
	esp.deliver(PINCODE_EDIT_SUCCESS);
	sp.deliver(PINCODE_CORRECT);
	return true;
}

void CharClientInterface::pincode_decrypt(uint32_t seed, char *input, char *output)
{
	int i;
	char tab[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	
	if (input == nullptr || output == nullptr)
		return;
	
	for (i = 1; i < 10; i++) {
		int pos;
		seed = 0x881234 + seed * 0x3498;
		pos = seed % (i + 1);
		if (i != pos) {
			tab[i] ^= tab[pos];
			tab[pos] ^= tab[i];
			tab[i] ^= tab[pos];
		}
	}
	
	for (i = 0; i < 4; i++) {
		if (input[i] < '0' || input[i] > '9')
			input[i] = '0';
		else
			input[i] = tab[input[i] - '0'];
	}
	
	sprintf(output, "%d%d%d%d", input[0], input[1], input[2], input[3]);
}

bool CharClientInterface::pincode_verify(uint32_t account_id, char *pincode)
{
	HC_SECOND_PASSWD_LOGIN sp(get_session());
	
	if (get_session()->get_session_data()._pincode_tries == sChar->config().pincode_max_retry()) {
#if CLIENT_TYPE == 'M' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180131
		sp.deliver(PINCODE_INCORRECT, PINCODE_LOGIN_FLAG_LOCKED);
#else
		sp.deliver(PINCODE_LOGIN_RESTRICT);
#endif
		return false;
	}
	
	char decrypted[5] = "\0\0\0\0";
	
	uint32_t seed = get_session()->get_session_data()._pincode_seed;
	pincode_decrypt(seed, pincode, decrypted);
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `pincode` FROM `game_accounts` WHERE `id` = ?");
		boost::mysql::results results;
		auto b1 = stmt.bind(account_id);
		conn->execute(b1, results);

		if (results.rows().empty()) {
			HC_REFUSE_ENTER hre(get_session());
			HLog(error) << "Attempt to select non-existent account " << account_id << " when verifying pincode.";
			hre.deliver(CHAR_ERR_REJECTED_FROM_SERVER);
			return false;
		}
		std::string s_pincode = results.rows()[0][0].as_string();

		if (s_pincode.compare(decrypted) != 0) {
			get_session()->get_session_data()._pincode_tries++;
#if CLIENT_TYPE == 'M' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180131
			sp.deliver(PINCODE_INCORRECT, PINCODE_LOGIN_FLAG_WRONG);
#else
			sp.deliver(PINCODE_INCORRECT);
#endif
			return false;
		}
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::pincode_verify:" << err.what();
		return false;
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::pincode_verify:" << err.what();
		return false;
	}
	

	sp.deliver(PINCODE_CORRECT);
	return true;
}

bool CharClientInterface::select_character(int slot)
{	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT `id`, `current_map` FROM `characters` WHERE `account_id` = ? AND `slot` = ? AND `deleted_at` = ?");
		auto b1 = stmt.bind(get_session()->get_session_data()._account_id, slot, 0);
		boost::mysql::results results;
		conn->execute(b1, results);

		if (results.rows().empty()) {
			HC_REFUSE_ENTER hre(get_session());
			HLog(error) << "Attempt to select non-existent character in slot " << slot << " for account " << get_session()->get_session_data()._account_id << ".";
			hre.deliver(CHAR_ERR_REJECTED_FROM_SERVER);
			return false;
		}

		HC_NOTIFY_ZONESVR hnz(get_session());
		std::string current_map = results.rows()[0][1].as_string();
		current_map.append(".gat");
		int char_id = results.rows()[0][0].get_int64();
		hnz.deliver(char_id, current_map, inet_addr(sChar->config().zone_server_ip().c_str()), sChar->config().zone_server_port());
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::select_character:" << err.what();
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::select_character:" << err.what();
	}
	return true;
}

bool CharClientInterface::update_session(int32_t account_id)
{
	HLog(debug) << "Updating session from I.P. address " << get_session()->get_socket()->remote_ip_address();
	
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sChar->get_database_connection();
	
	try {
		boost::mysql::statement stmt = conn->prepare_statement("SELECT * FROM `session_data` WHERE `game_account_id` = ?");
		auto a1 = stmt.bind(account_id);
		boost::mysql::results results;
		conn->execute(a1, results);

		if (results.rows().empty()) {
			HC_REFUSE_ENTER pkt(get_session());
			HLog(warning) << "Invalid connection for account with ID " << account_id << ", session wasn't found.";
			pkt.deliver(CHAR_ERR_REJECTED_FROM_SERVER);
			return false;
		}

		std::string current_server = results.rows()[0][7].as_string();

		if (current_server.compare("C") != 0) {
			HC_REFUSE_ENTER pkt(get_session());
			HLog(warning) << "Invalid connection for account with ID " << account_id << ", session wasn't found.";
			pkt.deliver(CHAR_ERR_REJECTED_FROM_SERVER);
			return false;
		}

		stmt = conn->prepare_statement("UPDATE `session_data` SET `last_update` = ? WHERE `game_account_id` = ?");
		auto a2 = stmt.bind(std::time(nullptr), account_id);
		conn->execute(a2, results);
	}
	catch (boost::mysql::error_with_diagnostics &err) {
		HLog(error) << "CharClientInterface::update_session:" << err.what();
	}
	catch (std::exception& err) {
		HLog(error) << "CharClientInterface::update_session:" << err.what();
	}

	return true;
}
