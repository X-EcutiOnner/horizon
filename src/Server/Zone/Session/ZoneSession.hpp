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
#ifndef HORIZON_ZONE_SESSION_ZONESESSION_HPP
#define HORIZON_ZONE_SESSION_ZONESESSION_HPP

#include "Libraries/Networking/Session.hpp"
#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Common/Configuration/ServerConfiguration.hpp"
#include "Server/Zone/Interface/ZoneClientInterface.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"

#if CLIENT_TYPE == 'R'
#include "Server/Zone/Packets/RE/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'M'
#include "Server/Zone/Packets/Ragexe/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'S'
#include "Server/Zone/Packets/Sakray/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'Z'
#include "Server/Zone/Packets/Zero/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'A'
#include "Server/Zone/Packets/Ad/ClientPacketLengthTable.hpp"
#endif

namespace Horizon
{
namespace Zone
{
//! @class ZoneSocket is forward declared to avoid recursive inclusion of header files.
class ZoneSocket;
//! @class ZoneSession
//! @details ZoneSession is responsible for handling the session of a client.
//! @var _player_created is a boolean variable which is used to check if the player is created or not.
//! @var _clif is a unique pointer of type ZoneClientInterface which is used to handle the client interface.
//! @var _pkt_tbl is a unique pointer of type ClientPacketLengthTable which is used to handle the client packet length table.
//! @var _player is a shared pointer of type Player which is used to handle the player.
class ZoneSession : public Horizon::Networking::Session<ZoneSocket, ZoneSession>
{
public:
	//! @details This is the constructor of the class ZoneSession.
	//! @param uid is a int64_t variable which is used to handle the unique id of the session.
	//! @param socket is a shared pointer of type ZoneSocket which is used to handle the socket.
	ZoneSession(uint64_t uid);

	//! @details This is the destructor of the class ZoneSession.
	//! @return void
	~ZoneSession();

	//! @details This function is used to queue the buffer to be sent to the client.
	//! @param _buffer is a reference to a ByteBuffer which is used to handle the buffer.
	//! @param size is a size_t variable which is used to handle the size of the buffer.
	//! @return void
	//!	@retval None
	//!	@par Side Effects: None
	//! @par Thread Safety: None
	//! @par Sync/Async: None
	//! @par Reentrancy: None
	void transmit_buffer(ByteBuffer _buffer, std::size_t size);

	//! @details This function is used to handle the incoming packets from the client
	//! and categorize the packets to be processed by the server.
	//! @param diff is a uint32_t variable which is used to handle the time difference.
	//! @return void
	void update(uint32_t diff);

	//! @details This function is used to handle the cleanup of the session.
	//! @return void
	void perform_cleanup();

	//! @details This function is used to handle the initialization of the session.
	//! @return void
	void initialize();
	
	//! @details This function is a getter function which is used to get the client interface.
	//! @return std::unique_ptr<ZoneClientInterface> &
	std::unique_ptr<ZoneClientInterface> &clif() { return _clif; }

	//! @details This function is a getter function which is used to get the client packet length table.
	//! @return std::unique_ptr<ClientPacketLengthTable> &
	std::unique_ptr<ClientPacketLengthTable> &pkt_tbl() { return _pkt_tbl; }
	
	//! @details This function is a getter function which is used to get the player.
	//! @return std::shared_ptr<Units::Player>
	std::shared_ptr<Units::Player> player() { return _player; }

	//! @details This function is a setter function which is used to set the player.
	//! @param pl is a shared pointer of type Player which is used to set the _player variable.
	//! @return void
	void set_player(std::shared_ptr<Units::Player> pl) { _player = pl; }

	//!	@details This function is a getter function which is used to get the map name.
	std::string get_map_name() { return _map_name; }
	void set_map_name(std::string map_name) { _map_name = map_name; }
protected:
	std::unique_ptr<ZoneClientInterface> _clif;
	std::unique_ptr<ClientPacketLengthTable> _pkt_tbl;
	std::shared_ptr<Units::Player> _player;
	std::string _map_name{""};
};
}
}

#endif
