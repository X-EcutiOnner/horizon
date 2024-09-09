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

/* This is an auto-generated file, please do not edit manually. */

#ifndef HORIZON_ZONE_CLIENTSOCKETMGR_HPP
#define HORIZON_ZONE_CLIENTSOCKETMGR_HPP

#include "Libraries/Networking/AcceptSocketMgr.hpp"
#include "Server/Common/System.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Socket/ZoneSocket.hpp"

#if WIN32
	#include <windows.h>
#elif __linux__
	#include <sched.h>
#endif

namespace Horizon
{
namespace Zone
{
	
class ZoneNetworkThread : public KernelComponent, public Networking::NetworkThread<ZoneSocket>
{
protected:
	void on_socket_removed(std::shared_ptr<ZoneSocket> socket) override
	{
		get_resource_manager().add<RESOURCE_PRIORITY_PRIMARY>(socket->get_socket_id(), socket);
	}

	void on_socket_added(std::shared_ptr<ZoneSocket> socket) override
	{
		get_resource_manager().remove<RESOURCE_PRIORITY_PRIMARY>(socket->get_socket_id());
	}
public:
	ZoneNetworkThread();

	bool start(int segment_number = 1) override
	{
		if (!Networking::NetworkThread<ZoneSocket>::start(segment_number))
			return false;

		initialize(segment_number);
		return true;
	}

	void run() override
	{
		Networking::NetworkThread<ZoneSocket>::run();
	}

	void update() override
	{
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
		Networking::NetworkThread<ZoneSocket>::update();

		get_system_routine_manager().process_queue();
		
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
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		set_total_execution_time(time_span.count());
	}

	virtual void initialize(int segment_number = 1) override 
	{ 
		set_segment_number(segment_number);

		_is_initialized.exchange(true);
	}

	virtual void finalize() override 
	{
		Networking::NetworkThread<ZoneSocket>::finalize();
		_is_finalized.exchange(true);
	}

	virtual bool is_initialized() override { return _is_initialized.load(); }
	virtual bool is_finalized() override { return _is_finalized.load(); }
protected:
	std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};

	using PrimaryResource = SharedPriorityResourceMedium<s_segment_storage<uint64_t, std::shared_ptr<ZoneSocket>>>;
	using ResourceManager = SharedPriorityResourceManager<PrimaryResource>;
	ResourceManager _resource_manager;
	
	ResourceManager &get_resource_manager() { return _resource_manager; }
};


/**
 * Manager of client sockets
 */
class ClientSocketMgr : public Horizon::Networking::AcceptSocketMgr<ZoneSocket, ZoneNetworkThread>
{
	typedef Horizon::Networking::AcceptSocketMgr<ZoneSocket, ZoneNetworkThread> BaseSocketMgr;
public:
	bool start(boost::asio::io_context &io_context, std::string const &listen_ip, uint16_t port, uint32_t threads = 1, bool minimal = false);

	bool stop();

	void update_sessions(uint64_t time)
	{
		auto socket_map = get_sockets();

		for (auto s : socket_map) {
			if (s.second->get_session() != nullptr)
				s.second->get_session()->update(time);
		}
	}
};
}
}

#endif /* HORIZON_ZONE_CLIENTSOCKETMGR_HPP */
