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
#ifndef HORIZON_AUTH_AUTHSESSION_HPP
#define HORIZON_AUTH_AUTHSESSION_HPP

#include "Server/Common/Configuration/Horizon.hpp"
#include "Libraries/Networking/Session.hpp"

#include <memory>

#if CLIENT_TYPE == 'S'
#include "Server/Auth/Packets/Sakray/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'R'
#include "Server/Auth/Packets/RE/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'Z'
#include "Server/Auth/Packets/Zero/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'M'
#include "Server/Auth/Packets/Ragexe/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'A'
#include "Server/Auth/Packets/AD/ClientPacketLengthTable.hpp"
#endif

namespace Horizon
{
namespace Auth
{
class AuthClientInterface;
class AuthSocket;
class AuthSession : public Horizon::Networking::Session<AuthSocket, AuthSession>
{
public:
	AuthSession(uint64_t uid);
	~AuthSession();

	/* */
	void initialize() override;
	virtual void update(uint32_t diff) override;
	
	std::unique_ptr<AuthClientInterface> &clif() { return _clif; }
	std::unique_ptr<ClientPacketLengthTable> &pkt_tbl() { return _pkt_tbl; }
	
	void transmit_buffer(ByteBuffer _buffer, std::size_t size);
	
private:
	std::unique_ptr<AuthClientInterface> _clif;
	std::unique_ptr<ClientPacketLengthTable> _pkt_tbl;
};
}
}
#endif /* HORIZON_SESSION_AuthSession_HPP */
