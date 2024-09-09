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
#ifndef HORIZON_SESSION_CHARSESSION_HPP
#define HORIZON_SESSION_CHARSESSION_HPP

#include "Libraries/Networking/Session.hpp"
#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Common/Configuration/ServerConfiguration.hpp"
#include "Server/Char/Interface/CharClientInterface.hpp"

#include <memory>
#include <chrono>
#include <mutex>

#if CLIENT_TYPE == 'S'
#include "Server/Char/Packets/Sakray/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'R'
#include "Server/Char/Packets/RE/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'Z'
#include "Server/Char/Packets/Zero/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'M'
#include "Server/Char/Packets/Ragexe/ClientPacketLengthTable.hpp"
#elif CLIENT_TYPE == 'A'
#include "Server/Char/Packets/AD/ClientPacketLengthTable.hpp"
#endif

namespace Horizon
{
namespace Char
{
struct s_session_data
{
	uint32_t _account_id{0};
	uint32_t _auth_id{0};
	uint32_t _group_id{0};
	std::chrono::system_clock::time_point _connect_time;
	uint8_t _character_slots{0};
	uint32_t _pincode_seed{0};
	uint8_t _pincode_tries{0};
};
class CharSocket;
class CharSession : public Horizon::Networking::Session<CharSocket, CharSession>
{
public:
	CharSession(uint64_t uid);
	~CharSession();

	void initialize();
	void update(uint32_t diff);

	void perform_cleanup();
	
	std::unique_ptr<CharClientInterface> &clif() { return _clif; }
	std::unique_ptr<ClientPacketLengthTable> &pkt_tbl() { return _pkt_tbl; }
	
	s_session_data &get_session_data() { std::lock_guard<std::mutex> lock(_sd_mutex); return _session_data; }
	void set_session_data(s_session_data &data) { std::lock_guard<std::mutex> lock(_sd_mutex); _session_data = data; }
	
	void transmit_buffer(ByteBuffer _buffer, std::size_t size);
	

protected:
	std::unique_ptr<CharClientInterface> _clif;
	std::unique_ptr<ClientPacketLengthTable> _pkt_tbl;
	s_session_data _session_data;
	std::mutex _sd_mutex;
	bool _first_packet_sent{false};
};
}
}
#endif /* HORIZON_CHAR_SESSION_CHARSESSION_HPP */
