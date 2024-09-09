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

#ifndef HORIZON_CHAR_RAGEXE_PACKET_LENGTH_TABLE
#define HORIZON_CHAR_RAGEXE_PACKET_LENGTH_TABLE

#include "Core/Multithreading/LockedLookupTable.hpp"
#include "Server/Char/Packets/HandledPackets.hpp"
#include "Server/Char/Packets/TransmittedPackets.hpp"

namespace Horizon
{
namespace Char
{
	typedef std::shared_ptr<Base::NetworkPacket<CharSession>> HPacketStructPtrType;
	typedef std::shared_ptr<Base::NetworkPacket<CharSession>> TPacketStructPtrType;
	typedef std::pair<int16_t, HPacketStructPtrType> HPacketTablePairType;
	typedef std::pair<int16_t, TPacketStructPtrType> TPacketTablePairType;
/**
 * @brief Auto-generated with a python generator tool authored by Sephus (sagunxp@gmail.com).
 */
class PacketLengthTable
{
public:
	PacketLengthTable(std::shared_ptr<CharSession> s)
	: _session(s)
	{
#define ADD_HPKT(i, j, k) _hpacket_length_table.insert(i, std::make_pair(j, std::make_shared<k>(s)))
#define ADD_TPKT(i, j, k) _tpacket_length_table.insert(i, std::make_pair(j, std::make_shared<k>(s)))
		ADD_HPKT(0x0068, 46, CH_DELETE_CHAR);
		ADD_HPKT(0x0065, 17, CH_ENTER);
		ADD_HPKT(0x0067, 37, CH_MAKE_CHAR);
		ADD_HPKT(0x0066, 3, CH_SELECT_CHAR);
		ADD_HPKT(0x0187, 6, CH_UNKNOWN_PING);
		ADD_TPKT(0x006f, 2, HC_ACCEPT_DELETECHAR);
		ADD_TPKT(0x006b, -1, HC_ACCEPT_ENTER);
		ADD_TPKT(0x006d, 108, HC_ACCEPT_MAKECHAR);
		ADD_TPKT(0x0071, 28, HC_NOTIFY_ZONESVR);
		ADD_TPKT(0x0070, 3, HC_REFUSE_DELETECHAR);
		ADD_TPKT(0x006c, 3, HC_REFUSE_ENTER);
		ADD_TPKT(0x006e, 3, HC_REFUSE_MAKECHAR);
#undef ADD_HPKT
#undef ADD_TPKT
	}

	~PacketLengthTable() { }

	std::shared_ptr<CharSession> get_session() { return _session.lock(); }

	HPacketTablePairType get_hpacket_info(uint16_t packet_id) { return _hpacket_length_table.at(packet_id); }
	TPacketTablePairType get_tpacket_info(uint16_t packet_id) { return _tpacket_length_table.at(packet_id); }

protected:
	LockedLookupTable<uint16_t, HPacketTablePairType> _hpacket_length_table;
	LockedLookupTable<uint16_t, TPacketTablePairType> _tpacket_length_table;
	std::weak_ptr<CharSession> _session;

};
}
}
#endif /* HORIZON_CHAR_RAGEXE_PACKET_LENGTH_TABLE */
