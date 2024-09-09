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

#ifndef HORIZON_ZONE_ZONEMAIN_HPP
#define HORIZON_ZONE_ZONEMAIN_HPP

#include "Server/pch.hpp"

#include "Server/Zone/SocketMgr/ClientSocketMgr.hpp"
#include "Server/Zone/Game/GameLogicProcess.hpp"
#include "Server/Zone/Persistence/PersistenceManager.hpp"
#include "Server/Zone/Script/ScriptManager.hpp"
#include "Core/Logging/Logger.hpp"
#include "Server/Common/Server.hpp"

namespace Horizon
{
	namespace Zone
	{
		namespace Game
		{
			namespace Units
			{
				class Player;
			}
		}
	}
}

namespace Horizon
{
namespace Zone
{

struct s_zone_server_configuration
{
	boost::filesystem::path &get_mapcache_path() { return _mapcache_path; }
	void set_mapcache_path(boost::filesystem::path p) { _mapcache_path = p; }
	
	boost::filesystem::path &get_static_db_path() { return _static_db_path; }
	void set_static_db_path(boost::filesystem::path p) { _static_db_path = p; }

	boost::filesystem::path &get_script_root_path() { return _script_root_path; }
	void set_script_root_path(boost::filesystem::path p) { _script_root_path = p; }
	
    std::time_t session_max_timeout() { return _session_max_timeout; }
    void set_session_max_timeout(std::time_t timeout) { _session_max_timeout = timeout; }
	
	int max_network_threads() { return _max_network_threads; }
	void set_max_network_threads(int threads) { _max_network_threads = threads; }

	int max_game_logic_threads() { return _max_game_logic_threads; }
	void set_max_game_logic_threads(int threads) { _max_game_logic_threads = threads; }

	int max_persistence_threads() { return _max_persistence_threads; }
	void set_max_persistence_threads(int threads) { _max_persistence_threads = threads; }

	int max_script_vm_threads() { return _max_script_vm_threads; }
	void set_max_script_vm_threads(int threads) { _max_script_vm_threads = threads; }

	boost::filesystem::path _static_db_path;
	boost::filesystem::path _mapcache_path;
    std::time_t _session_max_timeout;
	boost::filesystem::path _script_root_path;
	int _max_network_threads{1};
	int _max_game_logic_threads{1};
	int _max_persistence_threads{1};
	int _max_script_vm_threads{1};
};

class ZoneKernel : public Server
{
public:
	ZoneKernel(s_zone_server_configuration &config);
	~ZoneKernel();

	void initialize();
	void finalize();
	
	s_zone_server_configuration &config() { return _config; }

	TaskScheduler &getScheduler() { return _task_scheduler; }

	void verify_connected_sessions();

protected:
	TaskScheduler _task_scheduler;
	s_zone_server_configuration _config;
};

class ZoneRuntime : public KernelComponent
{
public:
	ZoneRuntime();

	virtual void initialize(int segment_number = 1) override
	{
		set_segment_number(segment_number);

		_is_initialized.exchange(true);

		_thread = std::thread(&ZoneRuntime::start, this);
	}

	virtual void finalize() override
	{
		if (_thread.joinable())
			_thread.join();

		_is_finalized.exchange(true);
	}

	void start();
	virtual void update(int64_t diff);

	bool is_initialized() override { return _is_initialized; }
	bool is_finalized() override { return _is_finalized; }
	
protected:
	using PrimaryResource = SharedPriorityResourceMedium<s_segment_storage<uint64_t, std::shared_ptr<ZoneSession>>>;
	using ResourceManager = SharedPriorityResourceManager<PrimaryResource>;
	ResourceManager _resource_manager;
public:
	ResourceManager &get_resource_manager() { return _resource_manager; }
private:
	std::thread _thread;
	std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};

};

class ZoneServer : public ZoneKernel
{
public:
	ZoneServer();
	~ZoneServer();

	static ZoneServer *getInstance()
	{
		static ZoneServer instance;
		return &instance;
	}

	bool read_config();
	void initialize();
	void finalize();

	void update(int64_t diff);

	s_zone_server_configuration &config() { return _zone_server_config; }

	uint64_t to_uuid(uint8_t type, uint32_t uid, uint16_t uid2, uint8_t uid3);
	void from_uuid(uint64_t unit_uuid, uint8_t& type, uint32_t& uid, uint16_t& uid2, uint8_t& uid3);

	ClientSocketMgr &get_client_socket_mgr() { return _client_socket_mgr; }
	
private:
	s_zone_server_configuration _zone_server_config;
	ClientSocketMgr _client_socket_mgr;
	boost::asio::deadline_timer _update_timer;
};
}
}

#define sZone Horizon::Zone::ZoneServer::getInstance()

#endif // HORIZON_ZONE_ZONEMAIN_HPP
