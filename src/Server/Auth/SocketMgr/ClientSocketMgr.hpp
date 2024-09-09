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

#ifndef HORIZON_CLIENTSOCKETMGR_HPP
#define HORIZON_CLIENTSOCKETMGR_HPP
#include "Libraries/Networking/AcceptSocketMgr.hpp"

#include "Server/Auth/Auth.hpp"
#include "Server/Auth/Session/AuthSession.hpp"
#include "Server/Auth/Socket/AuthSocket.hpp"
#include "Server/Common/System.hpp"
#include "Server/Common/Server.hpp"
#include "Server/Common/Configuration/ServerConfiguration.hpp"

#if WIN32
	#include <windows.h>
#elif __linux__
	#include <sched.h>
#endif

namespace Horizon
{
namespace Auth
{
class AuthNetworkThread : public KernelComponent, public Networking::NetworkThread<AuthSocket>
{
protected:
	void on_socket_removed(std::shared_ptr<AuthSocket> socket) override
	{
		get_resource_manager().add<RESOURCE_PRIORITY_PRIMARY>(socket->get_socket_id(), socket);
	}

	void on_socket_added(std::shared_ptr<AuthSocket> socket) override
	{
		get_resource_manager().remove<RESOURCE_PRIORITY_PRIMARY>(socket->get_socket_id());
	}
public:
	AuthNetworkThread() 
	: KernelComponent(sAuth, Horizon::System::RUNTIME_NETWORKING),
	_resource_manager(PrimaryResource(RESOURCE_PRIORITY_PRIMARY, std::make_shared<s_segment_storage<uint64_t, std::shared_ptr<AuthSocket>>>()))
	{
	}

	bool start(int segment_number = 1)
	{
		if (!Networking::NetworkThread<AuthSocket>::start(segment_number))
			return false;

		initialize(segment_number);
		
		return true;
	}

	void run() override
	{
		Networking::NetworkThread<AuthSocket>::run();
	}

	void update() override
	{
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
		Networking::NetworkThread<AuthSocket>::update();

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
		Networking::NetworkThread<AuthSocket>::finalize();

		_is_finalized.exchange(true);
	}

	virtual bool is_initialized() override { return _is_initialized.load(); }
	virtual bool is_finalized() override { return _is_finalized.load(); }

protected:
	std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};
	
	using PrimaryResource = SharedPriorityResourceMedium<s_segment_storage<uint64_t, std::shared_ptr<AuthSocket>>>;
	using ResourceManager = SharedPriorityResourceManager<PrimaryResource>;
	ResourceManager _resource_manager;
public:
	ResourceManager &get_resource_manager() { return _resource_manager; }
};
/**
 * Manager of client sockets and initialization of the packet db * @brief Singleton class
 */
class ClientSocketMgr : public Networking::AcceptSocketMgr<AuthSocket, AuthNetworkThread>
{
	typedef Networking::AcceptSocketMgr<AuthSocket, AuthNetworkThread> BaseSocketMgr;
public:
	static ClientSocketMgr *Instance()
	{
		static ClientSocketMgr instance;
		return &instance;
	}
	
	bool start(boost::asio::io_context &io_context, std::string const &listen_ip, uint16_t port, uint32_t threads = 1, bool minimal = false) override;

	bool stop()
	{
		get_sockets().clear();
		
		for (auto i = get_thread_map().begin(); i != get_thread_map().end(); i++)
			sAuth->deregister_component(Horizon::System::RUNTIME_NETWORKING, (std::static_pointer_cast<AuthNetworkThread>(i->second))->get_segment_number());

		if (!BaseSocketMgr::stop_network())
			return false;
		return true;
	}

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

#define sClientSocketMgr Horizon::Auth::ClientSocketMgr::Instance()

#endif /* HORIZON_CLIENTSOCKETMGR_HPP */
