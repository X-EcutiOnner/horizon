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

#ifndef HORIZON_NETWORKING_ACCEPTSOCKETMGR_HPP
#define HORIZON_NETWORKING_ACCEPTSOCKETMGR_HPP

#include "Libraries/Networking/SocketMgr.hpp"
#include "Core/Multithreading/ThreadSafeQueue.hpp"

#include <mutex>

namespace Horizon
{
namespace Networking
{
class AsyncAcceptor;
/**
 * @brief Socket Manager for Accepted Sockets.
 *        Deals with client sockets that were accepted by the server.
 */
template <class SocketType, class NetworkThreadType>
class AcceptSocketMgr : public SocketMgr<SocketType, NetworkThreadType>
{
	typedef std::map<uint32_t, std::shared_ptr<SocketType>> SocketMap;
	typedef SocketMgr<SocketType, NetworkThreadType> BaseSocketMgr;
public:
	/**
	 * @brief Initialize and start accepting connections asynchronously.
	 *        This method also starts the networking threads for accepted sockets.
	 * @param[in|out] &io_context  const reference to the io_context object.
	 * @param[in|out] &listen_ip   const reference to the ip_address string for the acceptor to bind on.
	 * @param[in]     port         port number for the acceptor to bind on.
	 * @param[in]     threads      number of network acceptor threads to start and run.
	 * @return true on success, false on failure.
	 */
	virtual bool start(boost::asio::io_context &io_context, std::string const &listen_ip, uint16_t port, uint32_t threads = 1, bool minimal = false)
	{
		try {
			_acceptor = std::make_unique<AsyncAcceptor>(io_context, listen_ip, port);
		} catch (boost::system::system_error const &error) {
			HLog(error) << "Exception caught in AcceptSocketMgr::start (" << listen_ip.c_str() << ", " << port << ") " << error.what();
			return false;
		}

		if (!BaseSocketMgr::StartNetworkThreads(threads)) {
			HLog(error) << "AcceptSocketMgr failed to start network threads.";
			return false;
		}

		_acceptor->set_socket_factory(std::bind(&BaseSocketMgr::get_new_socket, this));

		if (minimal == false)
			_acceptor->async_accept_with_callback(std::bind(&AcceptSocketMgr<SocketType, NetworkThreadType>::on_socket_open, this, std::placeholders::_1, std::placeholders::_2));

		HLog(info) << "Networking initialized, listening on " << listen_ip << "@" << port << ".";
		HLog(info) << "Maximum Network Threads: " << threads;

		return true;
	}

	/**
	 * @brief Stop the Acceptor network and clear the client socket map.
	 * - Called from the main thread only.
	 */
	virtual bool stop_network() override
	{
		BaseSocketMgr::stop_network();
	
		if (_acceptor->is_open()) {
			_acceptor->close();
		}

		_acceptor.reset();
		return true;
	}

	/**
	 * @brief On Socket Open / Start Event. Called as a callback from an Acceptor on successful socket acceptance.
	 *        Moves the socket ownership from the caller to a network thread.
	 * - Called from global I/O thread.
	 * @param[in]     socket         shared pointer.
	 * @param[in]     thread_index   index of the thread that the socket is being moved from.
	 */
	void on_socket_open(std::shared_ptr<tcp::socket> const &socket, uint32_t thread_index)
	{
		std::shared_ptr<SocketType> new_socket = BaseSocketMgr::on_socket_open(std::move(socket), thread_index);

		set_socket_for_management(new_socket);
	}

	/**
	 * @brief Sets a socket for removal on the next session update call.
	 * - Thread safe.
	 * @param[in] sock  socket to be removed.
	 */
	void set_socket_for_removal(std::weak_ptr<SocketType> sock)
	{
		_socket_management_queue.push(std::make_pair(false, sock.lock()));
	}

	void set_socket_for_management(std::shared_ptr<SocketType> sock)
	{
		_socket_management_queue.push(std::make_pair(true, sock));
	}

	/**
	 * @brief Updates every session in the socket map and removes ones in the removal queue.
	 * - Called from the main thread only.
	 * @param[in] diff
	 */
	void manage_sockets(uint32_t time)
	{
		std::shared_ptr<std::pair<bool, std::shared_ptr<SocketType>>> sock_buf;

		while ((sock_buf = _socket_management_queue.try_pop())) {
			bool add = (*sock_buf).first;
			std::shared_ptr<SocketType> socket = (*sock_buf).second;
			auto socket_iter = _socket_map.find(socket->get_socket_id());

			if (socket_iter != _socket_map.end()) {
				if (!add)
					_socket_map.erase(socket_iter);
			} else if (add) {
				_socket_map.emplace(socket->get_socket_id(), socket);
			}
		}
	}

	SocketMap &get_sockets() { return _socket_map; }

private:
	std::unique_ptr<AsyncAcceptor> _acceptor;       ///< unique pointer to an AsyncAcceptor object.
	SocketMap _socket_map;                          ///< std::map of all connected and handled sockets.
	ThreadSafeQueue<std::pair<bool, std::shared_ptr<SocketType>>> _socket_management_queue;
	std::atomic<bool> _is_initialized;
};
}
}

#endif /* HORIZON_NETWORKING_ACCEPTSOCKETMGR_HPP */
