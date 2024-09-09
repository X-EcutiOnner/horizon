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

#ifndef HORIZON_CHAR_TRANSMITTED_PACKETS_HPP
#define HORIZON_CHAR_TRANSMITTED_PACKETS_HPP

#include "Server/Common/Base/NetworkPacket.hpp"
#include "Server/Common/Configuration/Horizon.hpp"

namespace Horizon
{
namespace Char
{
class CharSession;
/**
 * @brief Main object for the aegis packet: HC_ACCOUNT_ID
 *
 */
class HC_ACCOUNT_ID : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACCOUNT_ID(std::shared_ptr<CharSession> s)
		: NetworkPacket(0, s)
	{ }
	virtual ~HC_ACCOUNT_ID() { }

	void deliver(int32_t account_id);
	ByteBuffer& serialize();

protected:
	int32_t _account_id;
};
enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_ACCEPT_DELETECHAR = 0x006f
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_HC_ACCEPT_DELETECHAR = 0x006f
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_ACCEPT_DELETECHAR = 0x006f
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_HC_ACCEPT_DELETECHAR = 0x006f
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACCEPT_DELETECHAR = 0x006f
#else
ID_HC_ACCEPT_DELETECHAR = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACCEPT_DELETECHAR
 *
 */ 
class HC_ACCEPT_DELETECHAR : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACCEPT_DELETECHAR(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACCEPT_DELETECHAR, s)
	{}
	virtual ~HC_ACCEPT_DELETECHAR() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_ACCEPT_ENTER = 0x006b
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_HC_ACCEPT_ENTER = 0x006b
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_ACCEPT_ENTER = 0x006b
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_HC_ACCEPT_ENTER = 0x006b
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACCEPT_ENTER = 0x006b
#else
ID_HC_ACCEPT_ENTER = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACCEPT_ENTER
 *
 */ 
class HC_ACCEPT_ENTER : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACCEPT_ENTER(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACCEPT_ENTER, s)
	{}
	virtual ~HC_ACCEPT_ENTER() {}

	bool prepare(uint32_t account_id, uint8_t max_char_slots, uint8_t permitted_slots, uint8_t total_premium_slots);
	void deliver();
	ByteBuffer &serialize();

/* Structure */
	uint16_t _packet_length{ 24 };
#if PACKET_VERSION >= 20100413
	uint8_t _max_char_slots{ MAX_CHARACTER_SLOTS };
	uint8_t _permitted_slots{ MAX_CHARACTER_SLOTS };
	// Total premium slots out of the max_char_slots which are premium. (if not permitted will show red)
	uint8_t _total_premium_slots{ MAX_CHARACTER_SLOTS };
#endif
	uint8_t _unknown_bytes[20]{ 0 }; ///< 20 Unknown bytes.

	std::vector<s_hc_character_list_data> _characters;
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20101221
ID_HC_ACCEPT_ENTER2 = 0x082d
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20101221
ID_HC_ACCEPT_ENTER2 = 0x082d
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACCEPT_ENTER2 = 0x082d
#else
ID_HC_ACCEPT_ENTER2 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACCEPT_ENTER2
 *
 */ 
class HC_ACCEPT_ENTER2 : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACCEPT_ENTER2(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACCEPT_ENTER2, s)
	{}
	virtual ~HC_ACCEPT_ENTER2() {}

	void deliver(uint8_t total_slots, uint8_t premium_slots);
	ByteBuffer& serialize();

/* Structure */
	uint16_t _packet_length{ 29 };
	uint8_t _total_slots{ MAX_CHARACTER_SLOTS };
	uint8_t _premium_slots{ MAX_CHARACTER_SLOTS };
	uint8_t _unknown_byte{ 0 };
	uint8_t _char_slots_1{ MAX_CHARACTER_SLOTS };
	uint8_t _char_slots_2{ MAX_CHARACTER_SLOTS };
	uint8_t _unknown_bytes[20]{ 0 };
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_ACCEPT_MAKECHAR = 0x006d
#elif CLIENT_TYPE == 'M' && \
	PACKET_VERSION >= 20201007 
ID_HC_ACCEPT_MAKECHAR = 0x0b6f
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_HC_ACCEPT_MAKECHAR = 0x006d
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20081217
ID_HC_ACCEPT_MAKECHAR = 0x006d
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_HC_ACCEPT_MAKECHAR = 0x006d
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACCEPT_MAKECHAR = 0x006d
#else
ID_HC_ACCEPT_MAKECHAR = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACCEPT_MAKECHAR
 *
 */ 
class HC_ACCEPT_MAKECHAR : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACCEPT_MAKECHAR(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACCEPT_MAKECHAR, s)
	{}
	virtual ~HC_ACCEPT_MAKECHAR() {}

	void deliver(uint32_t char_id, uint32_t start_zeny, std::string map_name, std::string name, uint8_t slot, uint32_t hair_color, uint32_t hair_style, uint32_t job_class, uint8_t gender);
	ByteBuffer &serialize();

/* Structure */
	s_hc_character_list_data _c;
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110906
ID_HC_ACK_CHANGE_CHARACTER_SLOT = 0x08d5
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110906
ID_HC_ACK_CHANGE_CHARACTER_SLOT = 0x08d5
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACK_CHANGE_CHARACTER_SLOT = 0x08d5
#else
ID_HC_ACK_CHANGE_CHARACTER_SLOT = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACK_CHANGE_CHARACTER_SLOT
 *
 */ 
class HC_ACK_CHANGE_CHARACTER_SLOT : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACK_CHANGE_CHARACTER_SLOT(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACK_CHANGE_CHARACTER_SLOT, s)
	{}
	virtual ~HC_ACK_CHANGE_CHARACTER_SLOT() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20061030
ID_HC_ACK_CHANGE_CHARNAME = 0x0290
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20061030
ID_HC_ACK_CHANGE_CHARNAME = 0x0290
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_ACK_CHANGE_CHARNAME = 0x0290
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20061023
ID_HC_ACK_CHANGE_CHARNAME = 0x0290
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACK_CHANGE_CHARNAME = 0x0290
#else
ID_HC_ACK_CHANGE_CHARNAME = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACK_CHANGE_CHARNAME
 *
 */ 
class HC_ACK_CHANGE_CHARNAME : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACK_CHANGE_CHARNAME(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACK_CHANGE_CHARNAME, s)
	{}
	virtual ~HC_ACK_CHANGE_CHARNAME() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' && \
	PACKET_VERSION >= 20201007
ID_HC_ACK_CHARINFO_PER_PAGE = 0x0b72
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20121024
ID_HC_ACK_CHARINFO_PER_PAGE = 0x099d
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20121024
ID_HC_ACK_CHARINFO_PER_PAGE = 0x099d
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACK_CHARINFO_PER_PAGE = 0x099d
#else
ID_HC_ACK_CHARINFO_PER_PAGE = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACK_CHARINFO_PER_PAGE
 *
 */ 
class HC_ACK_CHARINFO_PER_PAGE : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACK_CHARINFO_PER_PAGE(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACK_CHARINFO_PER_PAGE, s)
	{}
	virtual ~HC_ACK_CHARINFO_PER_PAGE() {}

	int32_t prepare(bool empty = false);
	void deliver();
	ByteBuffer &serialize();

/* Structure */
	uint16_t _packet_length{ 24 };
	std::vector<s_hc_character_list_data> _characters;
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20061030
ID_HC_ACK_IS_VALID_CHARNAME = 0x028e
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20061030
ID_HC_ACK_IS_VALID_CHARNAME = 0x028e
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_ACK_IS_VALID_CHARNAME = 0x028e
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20061023
ID_HC_ACK_IS_VALID_CHARNAME = 0x028e
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_ACK_IS_VALID_CHARNAME = 0x028e
#else
ID_HC_ACK_IS_VALID_CHARNAME = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_ACK_IS_VALID_CHARNAME
 *
 */ 
class HC_ACK_IS_VALID_CHARNAME : public Base::NetworkPacket<CharSession>
{
public:
	HC_ACK_IS_VALID_CHARNAME(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_ACK_IS_VALID_CHARNAME, s)
	{}
	virtual ~HC_ACK_IS_VALID_CHARNAME() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110228
ID_HC_AVAILABLE_SECOND_PASSWD = 0x08c6
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110228
ID_HC_AVAILABLE_SECOND_PASSWD = 0x08c6
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_AVAILABLE_SECOND_PASSWD = 0x08c6
#else
ID_HC_AVAILABLE_SECOND_PASSWD = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_AVAILABLE_SECOND_PASSWD
 *
 */ 
class HC_AVAILABLE_SECOND_PASSWD : public Base::NetworkPacket<CharSession>
{
public:
	HC_AVAILABLE_SECOND_PASSWD(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_AVAILABLE_SECOND_PASSWD, s)
	{}
	virtual ~HC_AVAILABLE_SECOND_PASSWD() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_BLOCK_CHARACTER = 0x020d
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20040621
ID_HC_BLOCK_CHARACTER = 0x020d
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_BLOCK_CHARACTER = 0x020d
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20040621
ID_HC_BLOCK_CHARACTER = 0x020d
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_BLOCK_CHARACTER = 0x020d
#else
ID_HC_BLOCK_CHARACTER = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_BLOCK_CHARACTER
 *
 */ 
class HC_BLOCK_CHARACTER : public Base::NetworkPacket<CharSession>
{
public:
	HC_BLOCK_CHARACTER(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_BLOCK_CHARACTER, s)
	{}
	virtual ~HC_BLOCK_CHARACTER() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
	uint16_t _packet_length{ 4 };
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20090225
ID_HC_CHARACTER_LIST = 0x0448
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20090226
ID_HC_CHARACTER_LIST = 0x0448
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20090225
ID_HC_CHARACTER_LIST = 0x0448
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_CHARACTER_LIST = 0x0448
#else
ID_HC_CHARACTER_LIST = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_CHARACTER_LIST
 *
 */ 
class HC_CHARACTER_LIST : public Base::NetworkPacket<CharSession>
{
public:
	HC_CHARACTER_LIST(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_CHARACTER_LIST, s)
	{}
	virtual ~HC_CHARACTER_LIST() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20121212
ID_HC_CHARLIST_NOTIFY = 0x09a0
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20121212
ID_HC_CHARLIST_NOTIFY = 0x09a0
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_CHARLIST_NOTIFY = 0x09a0
#else
ID_HC_CHARLIST_NOTIFY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_CHARLIST_NOTIFY
 *
 */ 
class HC_CHARLIST_NOTIFY : public Base::NetworkPacket<CharSession>
{
public:
	HC_CHARLIST_NOTIFY(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_CHARLIST_NOTIFY, s)
	{}
	virtual ~HC_CHARLIST_NOTIFY() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20061030
ID_HC_CHARNOTBEENSELECTED = 0x028b
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20061030
ID_HC_CHARNOTBEENSELECTED = 0x028b
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_CHARNOTBEENSELECTED = 0x028b
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20061023
ID_HC_CHARNOTBEENSELECTED = 0x028b
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_CHARNOTBEENSELECTED = 0x028b
#else
ID_HC_CHARNOTBEENSELECTED = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_CHARNOTBEENSELECTED
 *
 */ 
class HC_CHARNOTBEENSELECTED : public Base::NetworkPacket<CharSession>
{
public:
	HC_CHARNOTBEENSELECTED(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_CHARNOTBEENSELECTED, s)
	{}
	virtual ~HC_CHARNOTBEENSELECTED() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20090922
ID_HC_CHECKBOT = 0x07e8
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20090922
ID_HC_CHECKBOT = 0x07e8
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_CHECKBOT = 0x07e8
#else
ID_HC_CHECKBOT = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_CHECKBOT
 *
 */ 
class HC_CHECKBOT : public Base::NetworkPacket<CharSession>
{
public:
	HC_CHECKBOT(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_CHECKBOT, s)
	{}
	virtual ~HC_CHECKBOT() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20090922
ID_HC_CHECKBOT_RESULT = 0x07e9
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20090922
ID_HC_CHECKBOT_RESULT = 0x07e9
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_CHECKBOT_RESULT = 0x07e9
#else
ID_HC_CHECKBOT_RESULT = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_CHECKBOT_RESULT
 *
 */ 
class HC_CHECKBOT_RESULT : public Base::NetworkPacket<CharSession>
{
public:
	HC_CHECKBOT_RESULT(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_CHECKBOT_RESULT, s)
	{}
	virtual ~HC_CHECKBOT_RESULT() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100713
ID_HC_DELETE_CHAR3 = 0x082a
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100713
ID_HC_DELETE_CHAR3 = 0x082a
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_DELETE_CHAR3 = 0x082a
#else
ID_HC_DELETE_CHAR3 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_DELETE_CHAR3
 *
 */ 
class HC_DELETE_CHAR3 : public Base::NetworkPacket<CharSession>
{
public:
	HC_DELETE_CHAR3(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_DELETE_CHAR3, s)
	{}
	virtual ~HC_DELETE_CHAR3() {}

	void deliver(uint32_t char_id, character_delete_accept_result result);
	ByteBuffer &serialize();

/* Structure */
	uint32_t _character_id{ 0 };
	character_delete_accept_result _result{ CHAR_DEL_ACCEPT_RESULT_SUCCESS };
};

enum hc_delete_cancel_result : uint32_t {
	CHAR3_DEL_CANCEL_SUCCESS = 1,
	CHAR3_DEL_CANCEL_FAILURE = 2
};
enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100720
ID_HC_DELETE_CHAR3_CANCEL = 0x082c
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100720
ID_HC_DELETE_CHAR3_CANCEL = 0x082c
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_DELETE_CHAR3_CANCEL = 0x082c
#else
ID_HC_DELETE_CHAR3_CANCEL = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_DELETE_CHAR3_CANCEL
 *
 */ 
class HC_DELETE_CHAR3_CANCEL : public Base::NetworkPacket<CharSession>
{
public:
	HC_DELETE_CHAR3_CANCEL(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_DELETE_CHAR3_CANCEL, s)
	{}
	virtual ~HC_DELETE_CHAR3_CANCEL() {}

	void deliver(uint32_t char_id, hc_delete_cancel_result result);
	ByteBuffer &serialize();

/* Structure */
	uint32_t _character_id{ 0 };
	hc_delete_cancel_result _result{ 0 }; // either 1 or 2.
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100713
ID_HC_DELETE_CHAR3_RESERVED = 0x0828
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100713
ID_HC_DELETE_CHAR3_RESERVED = 0x0828
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_DELETE_CHAR3_RESERVED = 0x0828
#else
ID_HC_DELETE_CHAR3_RESERVED = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_DELETE_CHAR3_RESERVED
 *
 */ 
class HC_DELETE_CHAR3_RESERVED : public Base::NetworkPacket<CharSession>
{
public:
	HC_DELETE_CHAR3_RESERVED(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_DELETE_CHAR3_RESERVED, s)
	{}
	virtual ~HC_DELETE_CHAR3_RESERVED() {}

	void deliver(uint32_t char_id, character_delete_result res, uint32_t date);
	ByteBuffer &serialize();

/* Structure */
	uint32_t _character_id{ 0 };
	character_delete_result _result{ CHAR_DEL_RESULT_UNKNOWN };
	uint32_t _deletion_date{ 0 };
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20111207
ID_HC_DELETE_SECOND_PASSWD = 0x08bd
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20111207
ID_HC_DELETE_SECOND_PASSWD = 0x08bd
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_DELETE_SECOND_PASSWD = 0x08bd
#else
ID_HC_DELETE_SECOND_PASSWD = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_DELETE_SECOND_PASSWD
 *
 */ 
class HC_DELETE_SECOND_PASSWD : public Base::NetworkPacket<CharSession>
{
public:
	HC_DELETE_SECOND_PASSWD(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_DELETE_SECOND_PASSWD, s)
	{}
	virtual ~HC_DELETE_SECOND_PASSWD() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110308
ID_HC_EDIT_SECOND_PASSWD = 0x08bf
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110308
ID_HC_EDIT_SECOND_PASSWD = 0x08bf
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_EDIT_SECOND_PASSWD = 0x08bf
#else
ID_HC_EDIT_SECOND_PASSWD = 0x0000 // Disabled
#endif
};
enum pincode_edit_response : short {
	PINCODE_EDIT_SUCCESS = 0x0,
	PINCODE_EDIT_FAILED = 0x1,
	PINCODE_EDIT_RESTRICT_PW = 0x2,
	PINCODE_EDIT_PERSONALNUM_PW = 0x3,
};
/**
 * @brief Main object for the aegis packet: HC_EDIT_SECOND_PASSWD
 *
 */ 
class HC_EDIT_SECOND_PASSWD : public Base::NetworkPacket<CharSession>
{
public:
	HC_EDIT_SECOND_PASSWD(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_EDIT_SECOND_PASSWD, s)
	{}
	virtual ~HC_EDIT_SECOND_PASSWD() {}

	void deliver(pincode_edit_response state);
	ByteBuffer &serialize();

/* Structure */
	pincode_edit_response _state;
	uint32_t _seed;
};
enum pincode_make_response : short {
	PINCODE_MAKE_SUCCESS = 0,
	PINCODE_MAKE_DUPLICATED = 1,
	PINCODE_MAKE_RESTRICT_PW = 2,
	PINCODE_MAKE_PERSONALNUM_PW = 3,
	PINCODE_MAKE_FAILED = 4,
};
enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110308
ID_HC_MAKE_SECOND_PASSWD = 0x08bb
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110308
ID_HC_MAKE_SECOND_PASSWD = 0x08bb
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_MAKE_SECOND_PASSWD = 0x08bb
#else
ID_HC_MAKE_SECOND_PASSWD = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_MAKE_SECOND_PASSWD
 *
 */ 
class HC_MAKE_SECOND_PASSWD : public Base::NetworkPacket<CharSession>
{
public:
	HC_MAKE_SECOND_PASSWD(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_MAKE_SECOND_PASSWD, s)
	{}
	virtual ~HC_MAKE_SECOND_PASSWD() {}

	void deliver(pincode_make_response state);
	ByteBuffer &serialize();

/* Structure */
	pincode_make_response _state;
	uint32_t _seed;
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100713
ID_HC_NOTIFY_ACCESSIBLE_MAPNAME = 0x0840
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100713
ID_HC_NOTIFY_ACCESSIBLE_MAPNAME = 0x0840
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_NOTIFY_ACCESSIBLE_MAPNAME = 0x0840
#else
ID_HC_NOTIFY_ACCESSIBLE_MAPNAME = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_NOTIFY_ACCESSIBLE_MAPNAME
 *
 */ 
class HC_NOTIFY_ACCESSIBLE_MAPNAME : public Base::NetworkPacket<CharSession>
{
public:
	HC_NOTIFY_ACCESSIBLE_MAPNAME(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_NOTIFY_ACCESSIBLE_MAPNAME, s)
	{}
	virtual ~HC_NOTIFY_ACCESSIBLE_MAPNAME() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_NOTIFY_ZONESVR = 0x0071
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_HC_NOTIFY_ZONESVR = 0x0071
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20170228
ID_HC_NOTIFY_ZONESVR = 0x0ac5
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_NOTIFY_ZONESVR = 0x0071
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20170228
ID_HC_NOTIFY_ZONESVR = 0x0ac5
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_HC_NOTIFY_ZONESVR = 0x0071
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_NOTIFY_ZONESVR = 0x0ac5
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20180000
ID_HC_NOTIFY_ZONESVR = 0x0071
#else
ID_HC_NOTIFY_ZONESVR = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_NOTIFY_ZONESVR
 *
 */ 
class HC_NOTIFY_ZONESVR : public Base::NetworkPacket<CharSession>
{
public:
	HC_NOTIFY_ZONESVR(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_NOTIFY_ZONESVR, s)
	{}
	virtual ~HC_NOTIFY_ZONESVR() {}

	void deliver(uint32_t char_id, std::string map_name, uint32_t ip_address, uint16_t port);
	ByteBuffer &serialize();

/* Structure */
	uint32_t _char_id{ 0 };
	char _map_name[MAP_NAME_LENGTH_EXT]{ 0 };
	uint32_t _ip_address{ 0 };
	uint16_t _port{ 0 };
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110308
ID_HC_NOT_AVAILABLE_SECOND_PASSWD = 0x08c4
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110308
ID_HC_NOT_AVAILABLE_SECOND_PASSWD = 0x08c4
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_NOT_AVAILABLE_SECOND_PASSWD = 0x08c4
#else
ID_HC_NOT_AVAILABLE_SECOND_PASSWD = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_NOT_AVAILABLE_SECOND_PASSWD
 *
 */ 
class HC_NOT_AVAILABLE_SECOND_PASSWD : public Base::NetworkPacket<CharSession>
{
public:
	HC_NOT_AVAILABLE_SECOND_PASSWD(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_NOT_AVAILABLE_SECOND_PASSWD, s)
	{}
	virtual ~HC_NOT_AVAILABLE_SECOND_PASSWD() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20121128
ID_HC_QUEUE_ORDER = 0x099e
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20121128
ID_HC_QUEUE_ORDER = 0x099e
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_QUEUE_ORDER = 0x099e
#else
ID_HC_QUEUE_ORDER = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_QUEUE_ORDER
 *
 */ 
class HC_QUEUE_ORDER : public Base::NetworkPacket<CharSession>
{
public:
	HC_QUEUE_ORDER(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_QUEUE_ORDER, s)
	{}
	virtual ~HC_QUEUE_ORDER() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_REFUSE_DELETECHAR = 0x0070
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_HC_REFUSE_DELETECHAR = 0x0070
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_REFUSE_DELETECHAR = 0x0070
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_HC_REFUSE_DELETECHAR = 0x0070
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_REFUSE_DELETECHAR = 0x0070
#else
ID_HC_REFUSE_DELETECHAR = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_REFUSE_DELETECHAR
 *
 */ 
class HC_REFUSE_DELETECHAR : public Base::NetworkPacket<CharSession>
{
public:
	HC_REFUSE_DELETECHAR(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_REFUSE_DELETECHAR, s)
	{}
	virtual ~HC_REFUSE_DELETECHAR() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
	uint8_t _error{ 0 };
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_REFUSE_ENTER = 0x006c
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_HC_REFUSE_ENTER = 0x006c
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_REFUSE_ENTER = 0x006c
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_HC_REFUSE_ENTER = 0x006c
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_REFUSE_ENTER = 0x006c
#else
ID_HC_REFUSE_ENTER = 0x0000 // Disabled
#endif
};
enum hc_char_connect_error_type : uint8_t
{
	CHAR_ERR_REJECTED_FROM_SERVER = 0, // 0 = Rejected from server.
};
/**
 * @brief Main object for the aegis packet: HC_REFUSE_ENTER
 *
 */ 
class HC_REFUSE_ENTER : public Base::NetworkPacket<CharSession>
{
public:
	HC_REFUSE_ENTER(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_REFUSE_ENTER, s)
	{}
	virtual ~HC_REFUSE_ENTER() {}

	void deliver(hc_char_connect_error_type error);
	ByteBuffer &serialize();

/* Structure */
	hc_char_connect_error_type _error;
};
enum hc_char_create_error_type : uint8_t
{
	HC_CREATE_ERROR_ALREADY_EXISTS = 0x00,
	HC_CREATE_ERROR_DENIED = 0xFF,
	HC_CREATE_ERROR_UNDERAGE = 0x01,
	HC_CREATE_ERROR_SYMBOLS = 0x02,
	HC_CREATE_ERROR_CHAR_SLOT = 0x03
};
enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_HC_REFUSE_MAKECHAR = 0x006e
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_HC_REFUSE_MAKECHAR = 0x006e
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_REFUSE_MAKECHAR = 0x006e
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_HC_REFUSE_MAKECHAR = 0x006e
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_REFUSE_MAKECHAR = 0x006e
#else
ID_HC_REFUSE_MAKECHAR = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_REFUSE_MAKECHAR
 *
 */ 
class HC_REFUSE_MAKECHAR : public Base::NetworkPacket<CharSession>
{
public:
	HC_REFUSE_MAKECHAR(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_REFUSE_MAKECHAR, s)
	{}
	virtual ~HC_REFUSE_MAKECHAR() {}

	void deliver(hc_char_create_error_type error);
	ByteBuffer &serialize();

/* Structure */
	hc_char_create_error_type _error;
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20070821
ID_HC_REFUSE_SELECTCHAR = 0x02ca
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20070821
ID_HC_REFUSE_SELECTCHAR = 0x02ca
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_REFUSE_SELECTCHAR = 0x02ca
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20070821
ID_HC_REFUSE_SELECTCHAR = 0x02ca
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_REFUSE_SELECTCHAR = 0x02ca
#else
ID_HC_REFUSE_SELECTCHAR = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_REFUSE_SELECTCHAR
 *
 */ 
class HC_REFUSE_SELECTCHAR : public Base::NetworkPacket<CharSession>
{
public:
	HC_REFUSE_SELECTCHAR(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_REFUSE_SELECTCHAR, s)
	{}
	virtual ~HC_REFUSE_SELECTCHAR() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20050628
ID_HC_REQUEST_CHARACTER_PASSWORD = 0x023e
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20050628
ID_HC_REQUEST_CHARACTER_PASSWORD = 0x023e
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_HC_REQUEST_CHARACTER_PASSWORD = 0x023e
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20050628
ID_HC_REQUEST_CHARACTER_PASSWORD = 0x023e
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_REQUEST_CHARACTER_PASSWORD = 0x023e
#else
ID_HC_REQUEST_CHARACTER_PASSWORD = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_REQUEST_CHARACTER_PASSWORD
 *
 */ 
class HC_REQUEST_CHARACTER_PASSWORD : public Base::NetworkPacket<CharSession>
{
public:
	HC_REQUEST_CHARACTER_PASSWORD(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_REQUEST_CHARACTER_PASSWORD, s)
	{}
	virtual ~HC_REQUEST_CHARACTER_PASSWORD() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20171220
ID_HC_SECOND_PASSWD_LOGIN = 0x0ae9
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110222
ID_HC_SECOND_PASSWD_LOGIN = 0x08b9
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20171220
ID_HC_SECOND_PASSWD_LOGIN = 0x0ae9
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110222
	ID_HC_SECOND_PASSWD_LOGIN = 0x08b9
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_SECOND_PASSWD_LOGIN = 0x08b9
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20171220
ID_HC_SECOND_PASSWD_LOGIN = 0x0ae9
#else
ID_HC_SECOND_PASSWD_LOGIN = 0x0000 // Disabled
#endif
};


enum hc_pincode_state_type : uint16_t
{
	PINCODE_CORRECT = 0,
	PINCODE_REQUEST_PIN = 1,
	PINCODE_REQUEST_NEW = 2,
	PINCODE_REQUEST_CHANGE = 3,
	PINCODE_REQUEST_NEW_2 = 4,
	PINCODE_LOGIN_RESTRICT = 5,
	PINCODE_LOGIN_UNUSED = 6,
	PINCODE_SHOW_BUTTON = 7,
	PINCODE_INCORRECT = 8
};

#if CLIENT_TYPE == 'M' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180131
enum hc_pincode_login_response2 {
	PINCODE_LOGIN_FLAG_LOCKED = 0,
	PINCODE_LOGIN_FLAG_CORRECT = 1,
	PINCODE_LOGIN_FLAG_WRONG = 2,
};
#endif

/**
 * @brief Main object for the aegis packet: HC_SECOND_PASSWD_LOGIN
 *
 */ 
class HC_SECOND_PASSWD_LOGIN : public Base::NetworkPacket<CharSession>
{
public:
	HC_SECOND_PASSWD_LOGIN(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_SECOND_PASSWD_LOGIN, s)
	{}
	virtual ~HC_SECOND_PASSWD_LOGIN() {}

	void deliver(hc_pincode_state_type state);

#if CLIENT_TYPE == 'M' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180131
	void deliver(hc_pincode_state_type state, hc_pincode_login_response2 state2);
#endif
	ByteBuffer &serialize();

/* Structure */
	uint32_t _pincode_seed{ 0 };
	uint32_t _account_id{ 0 };
	hc_pincode_state_type _state{ PINCODE_CORRECT };
#if CLIENT_TYPE == 'M' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180124 \
|| CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180131
	hc_pincode_login_response2 _state2{ PINCODE_LOGIN_FLAG_WRONG };
#endif
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110215
ID_HC_SECOND_PASSWD_REQ = 0x08b7
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110215
ID_HC_SECOND_PASSWD_REQ = 0x08b7
#else
ID_HC_SECOND_PASSWD_REQ = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_SECOND_PASSWD_REQ
 *
 */ 
class HC_SECOND_PASSWD_REQ : public Base::NetworkPacket<CharSession>
{
public:
	HC_SECOND_PASSWD_REQ(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_SECOND_PASSWD_REQ, s)
	{}
	virtual ~HC_SECOND_PASSWD_REQ() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20130522
ID_HC_SECRETSCAN_DATA = 0x09c2
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20130522
ID_HC_SECRETSCAN_DATA = 0x09c2
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_SECRETSCAN_DATA = 0x09c2
#else
ID_HC_SECRETSCAN_DATA = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_SECRETSCAN_DATA
 *
 */ 
class HC_SECRETSCAN_DATA : public Base::NetworkPacket<CharSession>
{
public:
	HC_SECRETSCAN_DATA(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_SECRETSCAN_DATA, s)
	{}
	virtual ~HC_SECRETSCAN_DATA() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20111101
ID_HC_UPDATE_CHARINFO = 0x08e3
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20111025
ID_HC_UPDATE_CHARINFO = 0x08e3
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_UPDATE_CHARINFO = 0x08e3
#else
ID_HC_UPDATE_CHARINFO = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_UPDATE_CHARINFO
 *
 */ 
class HC_UPDATE_CHARINFO : public Base::NetworkPacket<CharSession>
{
public:
	HC_UPDATE_CHARINFO(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_UPDATE_CHARINFO, s)
	{}
	virtual ~HC_UPDATE_CHARINFO() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20101228
ID_HC_WAITING_LOGIN = 0x08af
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20101228
ID_HC_WAITING_LOGIN = 0x08af
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_HC_WAITING_LOGIN = 0x08af
#else
ID_HC_WAITING_LOGIN = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: HC_WAITING_LOGIN
 *
 */ 
class HC_WAITING_LOGIN : public Base::NetworkPacket<CharSession>
{
public:
	HC_WAITING_LOGIN(std::shared_ptr<CharSession> s)
	: NetworkPacket<CharSession>(ID_HC_WAITING_LOGIN, s)
	{}
	virtual ~HC_WAITING_LOGIN() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

} /* namespace Char
 */} /* namespace Horizon */
#endif /* HORIZON_CHAR_TRANSMITTED_PACKETS_HPP */
