/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2023 Horizon Dev Team.
 *
 * Base Author - Sephus. (sagunxp@gmail.com)
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

#ifndef HORIZON_AUTH_RAGEXE_PACKET_LENGTH_TABLE
#define HORIZON_AUTH_RAGEXE_PACKET_LENGTH_TABLE

#include "Core/Multithreading/LockedLookupTable.hpp"
#include "Server/Auth/Packets/HandledPackets.hpp"
#include "Server/Auth/Packets/TransmittedPackets.hpp"

namespace Horizon
{
namespace Auth
{
	typedef std::shared_ptr<Base::NetworkPacket<AuthSession>> HPacketStructPtrType;
	typedef std::shared_ptr<Base::NetworkPacket<AuthSession>> TPacketStructPtrType;
	typedef std::pair<int16_t, HPacketStructPtrType> HPacketTablePairType;
	typedef std::pair<int16_t, TPacketStructPtrType> TPacketTablePairType;
/**
 * @brief Auto-generated with a python generator tool authored by Sephus (sagunxp@gmail.com).
 */
class PacketLengthTable
{
public:
	PacketLengthTable(std::shared_ptr<AuthSession> s)
	: _session(s)
	{
#define ADD_HPKT(i, j, k) _hpacket_length_table.insert(i, std::make_pair(j, std::make_shared<k>(s)))
#define ADD_TPKT(i, j, k) _tpacket_length_table.insert(i, std::make_pair(j, std::make_shared<k>(s)))
		ADD_TPKT(0x0069, -1, AC_ACCEPT_LOGIN);
		ADD_TPKT(0x01dc, -1, AC_ACK_HASH);
		ADD_TPKT(0x01be, 2, AC_ASK_PNGAMEROOM);
		ADD_TPKT(0x006a, 23, AC_REFUSE_LOGIN);
		ADD_HPKT(0x0064, 55, CA_LOGIN);
		ADD_HPKT(0x01dd, 47, CA_LOGIN2);
		ADD_HPKT(0x01bf, 3, CA_REPLY_PNGAMEROOM);
		ADD_HPKT(0x01db, 2, CA_REQ_HASH);
#undef ADD_HPKT
#undef ADD_TPKT
	}

	~PacketLengthTable() { }

	std::shared_ptr<AuthSession> get_session() { return _session.lock(); }

	HPacketTablePairType get_hpacket_info(uint16_t packet_id) { return _hpacket_length_table.at(packet_id); }
	TPacketTablePairType get_tpacket_info(uint16_t packet_id) { return _tpacket_length_table.at(packet_id); }

protected:
	LockedLookupTable<uint16_t, HPacketTablePairType> _hpacket_length_table;
	LockedLookupTable<uint16_t, TPacketTablePairType> _tpacket_length_table;
	std::weak_ptr<AuthSession> _session;

};
}
}
#endif /* HORIZON_AUTH_RAGEXE_PACKET_LENGTH_TABLE */
