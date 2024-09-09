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

#ifndef HORIZON_AUTH_HPP
#define HORIZON_AUTH_HPP

#include "Server/pch.hpp"

#include "Server/Common/Server.hpp"

namespace Horizon
{
namespace Auth
{
/**
 * Main Auth Server Singleton Class.
 */
struct auth_config_type {
	std::string _password_salt_mix;

	struct char_server
	{
	    std::string _name, _host;
	    uint16_t _port, _type, _is_new;  
	};

    void add_char_server(char_server c) { _char_servers.push_back(c); }
    std::vector<char_server> &get_char_servers() { return _char_servers; }

	int max_network_threads() { return _max_network_threads; }
	void set_max_network_threads(int threads) { _max_network_threads = threads; }

    std::vector<char_server> _char_servers;
	int _max_network_threads{1};
};

const int SALT_LEN = 16;
const int HASH_LEN = 32;
const int ITERATIONS = 10000;

class AuthServer : public Server
{
public:
	AuthServer();
	~AuthServer();

	static AuthServer *getInstance()
	{
		static AuthServer instance;
		return &instance;
	}

	bool read_config();

	void initialize() override;
	void finalize() override;

	void generate_salt(std::vector<unsigned char>& salt) {
	    salt.resize(SALT_LEN);
	    if (!RAND_bytes(salt.data(), SALT_LEN)) {
	        throw std::runtime_error("Failed to generate salt");
	    }
	}

	void hash_password(const std::string& password, const std::vector<unsigned char>& salt, std::vector<unsigned char>& hash) {
	    hash.resize(HASH_LEN);
	    if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt.data(), salt.size(), ITERATIONS, EVP_sha256(), HASH_LEN, hash.data())) {
	        throw std::runtime_error("Failed to hash password");
	    }
	}

	/* CLI */
	void initialize_cli_commands();
	bool clicmd_reload_config(std::string /*cmd*/);
	bool clicmd_create_new_account(std::string /*cmd*/);
	bool clicmd_reset_password(std::string /*cmd*/);

	/* Task Scheduler */
	TaskScheduler &getScheduler() { return _task_scheduler; }
    
	auth_config_type &get_auth_config()
	{
		std::lock_guard<std::mutex> lock(_conf_lock);
		return _auth_config;
	}
	
	void update(uint64_t time);
	
protected:
	TaskScheduler _task_scheduler;
	std::mutex _conf_lock;
	auth_config_type _auth_config;
	boost::asio::deadline_timer _update_timer;

};
}
}

#define sAuth Horizon::Auth::AuthServer::getInstance()

#endif /* HORIZON_AUTH_HPP */
