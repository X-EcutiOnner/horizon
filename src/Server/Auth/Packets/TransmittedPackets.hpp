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

#ifndef HORIZON_AUTH_TRANSMITTED_PACKETS_HPP
#define HORIZON_AUTH_TRANSMITTED_PACKETS_HPP

#include "Server/Common/Base/NetworkPacket.hpp"
#include "Server/Common/Configuration/Horizon.hpp"

namespace Horizon
{
namespace Auth
{
class AuthSession;

/* Size: 47 + (32 * char server count) bytes */
#pragma pack(push, 1)
struct s_ac_accept_login {
	int16_t packet_len{ 0 };         ///< Packet length (variable length)
	int32_t auth_code{ 0 };          ///< Authentication code
	uint32_t aid{ 0 };               ///< Account ID
	uint32_t user_level{ 0 };        ///< User level
	uint32_t last_login_ip{ 0 };     ///< Last login IP
	char last_login_time[26]{ 0 };   ///< Last login timestamp
	uint8_t sex{ 0 };                ///< Account sex
#if PACKET_VERSION >= 20170315
	char twitter_auth_token[16]{ 0 };
	uint8_t twitter_flag{ 0 };
#endif
};

struct s_ac_char_server_list
{
	uint32_t ip{ 0 };            ///< Server IP address
	int16_t port{ 0 };           ///< Server port
	char name[20]{ 0 };          ///< Server name
	uint16_t usercount{ 0 };     ///< Online users
	uint16_t is_new{ 0 };        ///< Server state
	uint16_t type{ 0 };          ///< Server type @ character_server_type
#if PACKET_VERSION >= 20170315
	char unknown2[128]{ 0 };
#endif
};
#pragma pack(pop)

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_AC_ACCEPT_LOGIN = 0x0069
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20170228
	ID_AC_ACCEPT_LOGIN = 0x0ac4
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_AC_ACCEPT_LOGIN = 0x0069
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20170228
	ID_AC_ACCEPT_LOGIN = 0x0ac4
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACCEPT_LOGIN = 0x0069
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_AC_ACCEPT_LOGIN = 0x0069
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20180000
	ID_AC_ACCEPT_LOGIN = 0x0069
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACCEPT_LOGIN = 0x0ac4
#else
ID_AC_ACCEPT_LOGIN = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACCEPT_LOGIN
 *
 */ 
class AC_ACCEPT_LOGIN : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACCEPT_LOGIN(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACCEPT_LOGIN, s)
	{}
	virtual ~AC_ACCEPT_LOGIN() {}

	void deliver(int32_t auth_code, uint32_t aid, uint32_t user_level, uint8_t sex);
	ByteBuffer &serialize();

/* Structure */
	s_ac_accept_login _al;
	std::vector<s_ac_char_server_list> _c;
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20090701
ID_AC_ACCEPT_LOGIN2 = 0x0276
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20090701
ID_AC_ACCEPT_LOGIN2 = 0x0276
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACCEPT_LOGIN2 = 0x0276
#else
ID_AC_ACCEPT_LOGIN2 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACCEPT_LOGIN2
 *
 */ 
class AC_ACCEPT_LOGIN2 : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACCEPT_LOGIN2(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACCEPT_LOGIN2, s)
	{}
	virtual ~AC_ACCEPT_LOGIN2() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_AUTHREFUSE = 0x026a
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_AUTHREFUSE = 0x026a
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_EKEY_FAIL_AUTHREFUSE = 0x026a
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_AUTHREFUSE = 0x026a
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_EKEY_FAIL_AUTHREFUSE = 0x026a
#else
ID_AC_ACK_EKEY_FAIL_AUTHREFUSE = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_EKEY_FAIL_AUTHREFUSE
 *
 */ 
class AC_ACK_EKEY_FAIL_AUTHREFUSE : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_EKEY_FAIL_AUTHREFUSE(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_EKEY_FAIL_AUTHREFUSE, s)
	{}
	virtual ~AC_ACK_EKEY_FAIL_AUTHREFUSE() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_INPUTEKEY = 0x026b
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_INPUTEKEY = 0x026b
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_EKEY_FAIL_INPUTEKEY = 0x026b
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_INPUTEKEY = 0x026b
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_EKEY_FAIL_INPUTEKEY = 0x026b
#else
ID_AC_ACK_EKEY_FAIL_INPUTEKEY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_EKEY_FAIL_INPUTEKEY
 *
 */ 
class AC_ACK_EKEY_FAIL_INPUTEKEY : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_EKEY_FAIL_INPUTEKEY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_EKEY_FAIL_INPUTEKEY, s)
	{}
	virtual ~AC_ACK_EKEY_FAIL_INPUTEKEY() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NEEDCARDPASS = 0x026d
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NEEDCARDPASS = 0x026d
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_EKEY_FAIL_NEEDCARDPASS = 0x026d
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NEEDCARDPASS = 0x026d
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_EKEY_FAIL_NEEDCARDPASS = 0x026d
#else
ID_AC_ACK_EKEY_FAIL_NEEDCARDPASS = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_EKEY_FAIL_NEEDCARDPASS
 *
 */ 
class AC_ACK_EKEY_FAIL_NEEDCARDPASS : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_EKEY_FAIL_NEEDCARDPASS(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_EKEY_FAIL_NEEDCARDPASS, s)
	{}
	virtual ~AC_ACK_EKEY_FAIL_NEEDCARDPASS() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTEXIST = 0x0267
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTEXIST = 0x0267
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_EKEY_FAIL_NOTEXIST = 0x0267
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTEXIST = 0x0267
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_EKEY_FAIL_NOTEXIST = 0x0267
#else
ID_AC_ACK_EKEY_FAIL_NOTEXIST = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_EKEY_FAIL_NOTEXIST
 *
 */ 
class AC_ACK_EKEY_FAIL_NOTEXIST : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_EKEY_FAIL_NOTEXIST(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_EKEY_FAIL_NOTEXIST, s)
	{}
	virtual ~AC_ACK_EKEY_FAIL_NOTEXIST() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTICE = 0x026c
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTICE = 0x026c
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_EKEY_FAIL_NOTICE = 0x026c
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTICE = 0x026c
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_EKEY_FAIL_NOTICE = 0x026c
#else
ID_AC_ACK_EKEY_FAIL_NOTICE = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_EKEY_FAIL_NOTICE
 *
 */ 
class AC_ACK_EKEY_FAIL_NOTICE : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_EKEY_FAIL_NOTICE(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_EKEY_FAIL_NOTICE, s)
	{}
	virtual ~AC_ACK_EKEY_FAIL_NOTICE() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTUSEDEKEY = 0x0269
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTUSEDEKEY = 0x0269
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_EKEY_FAIL_NOTUSEDEKEY = 0x0269
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTUSEDEKEY = 0x0269
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_EKEY_FAIL_NOTUSEDEKEY = 0x0269
#else
ID_AC_ACK_EKEY_FAIL_NOTUSEDEKEY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_EKEY_FAIL_NOTUSEDEKEY
 *
 */ 
class AC_ACK_EKEY_FAIL_NOTUSEDEKEY : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_EKEY_FAIL_NOTUSEDEKEY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_EKEY_FAIL_NOTUSEDEKEY, s)
	{}
	virtual ~AC_ACK_EKEY_FAIL_NOTUSEDEKEY() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTUSESEKEY = 0x0268
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTUSESEKEY = 0x0268
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_EKEY_FAIL_NOTUSESEKEY = 0x0268
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_EKEY_FAIL_NOTUSESEKEY = 0x0268
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_EKEY_FAIL_NOTUSESEKEY = 0x0268
#else
ID_AC_ACK_EKEY_FAIL_NOTUSESEKEY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_EKEY_FAIL_NOTUSESEKEY
 *
 */ 
class AC_ACK_EKEY_FAIL_NOTUSESEKEY : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_EKEY_FAIL_NOTUSESEKEY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_EKEY_FAIL_NOTUSESEKEY, s)
	{}
	virtual ~AC_ACK_EKEY_FAIL_NOTUSESEKEY() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_FIRST_LOGIN = 0x026f
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_FIRST_LOGIN = 0x026f
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_FIRST_LOGIN = 0x026f
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_FIRST_LOGIN = 0x026f
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_FIRST_LOGIN = 0x026f
#else
ID_AC_ACK_FIRST_LOGIN = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_FIRST_LOGIN
 *
 */ 
class AC_ACK_FIRST_LOGIN : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_FIRST_LOGIN(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_FIRST_LOGIN, s)
	{}
	virtual ~AC_ACK_FIRST_LOGIN() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20050912
ID_AC_ACK_GAME_GUARD = 0x0259
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20050912
ID_AC_ACK_GAME_GUARD = 0x0259
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_GAME_GUARD = 0x0259
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20050912
ID_AC_ACK_GAME_GUARD = 0x0259
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_GAME_GUARD = 0x0259
#else
ID_AC_ACK_GAME_GUARD = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_GAME_GUARD
 *
 */ 
class AC_ACK_GAME_GUARD : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_GAME_GUARD(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_GAME_GUARD, s)
	{}
	virtual ~AC_ACK_GAME_GUARD() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_AC_ACK_HASH = 0x01dc
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_AC_ACK_HASH = 0x01dc
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_HASH = 0x01dc
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_AC_ACK_HASH = 0x01dc
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_HASH = 0x01dc
#else
ID_AC_ACK_HASH = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_HASH
 *
 */ 
class AC_ACK_HASH : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_HASH(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_HASH, s)
	{}
	virtual ~AC_ACK_HASH() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_PT_ID_INFO = 0x0272
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_PT_ID_INFO = 0x0272
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ACK_PT_ID_INFO = 0x0272
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_ACK_PT_ID_INFO = 0x0272
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ACK_PT_ID_INFO = 0x0272
#else
ID_AC_ACK_PT_ID_INFO = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ACK_PT_ID_INFO
 *
 */ 
class AC_ACK_PT_ID_INFO : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ACK_PT_ID_INFO(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ACK_PT_ID_INFO, s)
	{}
	virtual ~AC_ACK_PT_ID_INFO() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_AC_ASK_PNGAMEROOM = 0x01be
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_AC_ASK_PNGAMEROOM = 0x01be
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_ASK_PNGAMEROOM = 0x01be
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_AC_ASK_PNGAMEROOM = 0x01be
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_ASK_PNGAMEROOM = 0x01be
#else
ID_AC_ASK_PNGAMEROOM = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_ASK_PNGAMEROOM
 *
 */ 
class AC_ASK_PNGAMEROOM : public Base::NetworkPacket<AuthSession>
{
public:
	AC_ASK_PNGAMEROOM(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_ASK_PNGAMEROOM, s)
	{}
	virtual ~AC_ASK_PNGAMEROOM() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20050628
ID_AC_EVENT_RESULT = 0x023d
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20050628
ID_AC_EVENT_RESULT = 0x023d
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_EVENT_RESULT = 0x023d
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20050628
ID_AC_EVENT_RESULT = 0x023d
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_EVENT_RESULT = 0x023d
#else
ID_AC_EVENT_RESULT = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_EVENT_RESULT
 *
 */ 
class AC_EVENT_RESULT : public Base::NetworkPacket<AuthSession>
{
public:
	AC_EVENT_RESULT(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_EVENT_RESULT, s)
	{}
	virtual ~AC_EVENT_RESULT() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20190000
ID_AC_LOGIN_OTP = 0x0ad1
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20171213
ID_AC_LOGIN_OTP = 0x0ae3
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20190000
	ID_AC_LOGIN_OTP = 0x0ad1
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20171213
ID_AC_LOGIN_OTP = 0x0ae3
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20190000
	ID_AC_LOGIN_OTP = 0x0ad1
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20171123
ID_AC_LOGIN_OTP = 0x0ae3
#else
ID_AC_LOGIN_OTP = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_LOGIN_OTP
 *
 */ 
class AC_LOGIN_OTP : public Base::NetworkPacket<AuthSession>
{
public:
	AC_LOGIN_OTP(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_LOGIN_OTP, s)
	{}
	virtual ~AC_LOGIN_OTP() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
	int16_t _packet_length{ 0 };
	int32_t _loginFlag{ 0 };
#if (CLIENT_TYPE == 'M' && PACKET_VERSION >= 20171213) \
	|| (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20171213)
	char _loginFlag2[20];
#elif CLEINT_TYPE == 'Z' && PACKET_VERSION >= 20171123 
	char _loginFlag[6];
#endif
	char _token[6];
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_AC_NOTIFY_ERROR = 0x01f1
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20031223
ID_AC_NOTIFY_ERROR = 0x01f1
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_NOTIFY_ERROR = 0x01f1
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20031223
ID_AC_NOTIFY_ERROR = 0x01f1
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_NOTIFY_ERROR = 0x01f1
#else
ID_AC_NOTIFY_ERROR = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_NOTIFY_ERROR
 *
 */ 
class AC_NOTIFY_ERROR : public Base::NetworkPacket<AuthSession>
{
public:
	AC_NOTIFY_ERROR(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_NOTIFY_ERROR, s)
	{}
	virtual ~AC_NOTIFY_ERROR() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100413
ID_AC_OTP_AUTH_ACK = 0x0823
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100413
ID_AC_OTP_AUTH_ACK = 0x0823
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_OTP_AUTH_ACK = 0x0823
#else
ID_AC_OTP_AUTH_ACK = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_OTP_AUTH_ACK
 *
 */ 
class AC_OTP_AUTH_ACK : public Base::NetworkPacket<AuthSession>
{
public:
	AC_OTP_AUTH_ACK(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_OTP_AUTH_ACK, s)
	{}
	virtual ~AC_OTP_AUTH_ACK() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100413
ID_AC_OTP_USER = 0x0821
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100413
ID_AC_OTP_USER = 0x0821
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_OTP_USER = 0x0821
#else
ID_AC_OTP_USER = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_OTP_USER
 *
 */ 
class AC_OTP_USER : public Base::NetworkPacket<AuthSession>
{
public:
	AC_OTP_USER(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_OTP_USER, s)
	{}
	virtual ~AC_OTP_USER() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20101228
ID_AC_REALNAME_AUTH = 0x08b2
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20101228
ID_AC_REALNAME_AUTH = 0x08b2
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REALNAME_AUTH = 0x08b2
#else
ID_AC_REALNAME_AUTH = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REALNAME_AUTH
 *
 */ 
class AC_REALNAME_AUTH : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REALNAME_AUTH(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REALNAME_AUTH, s)
	{}
	virtual ~AC_REALNAME_AUTH() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum class login_error_codes
{
	ERR_NONE = -1,  // All ok!
	ERR_UNREGISTERED_ID = 0,  // Unregistered ID
	ERR_INCORRECT_PASSWORD = 1,  // Incorrect Password
	ERR_EXPIRED_ID = 2,  // This ID is expired
	ERR_REJECTED_FROM_SERVER = 3,  // Rejected from Server
	ERR_GM_BLOCK = 4,  // You have been blocked by the GM Team
	ERR_INCORRECT_CLIENT_VERSION = 5,  // Your Game's EXE file is not the latest version
	ERR_BAN_TIMESTAMP = 6,  // Your are Prohibited to log in until %s
	ERR_SERVER_OVERPOPULATED = 7,  // Server is jammed due to over populated
	ERR_COMPANY_LIMIT = 8,  // No more accounts may be connected from this company
	ERR_MSI_REFUSE_BAN_BY_DBA = 9,  // MSI_REFUSE_BAN_BY_DBA
	ERR_MSI_REFUSE_EMAIL_NOT_CONFIRMED = 10,  // MSI_REFUSE_EMAIL_NOT_CONFIRMED
	ERR_MSI_REFUSE_BAN_BY_GM = 11,  // MSI_REFUSE_BAN_BY_GM
	ERR_MSI_REFUSE_TEMP_BAN_FOR_DBWORK = 12,  // MSI_REFUSE_TEMP_BAN_FOR_DBWORK
	ERR_MSI_REFUSE_SELF_LOCK = 13,  // MSI_REFUSE_SELF_LOCK
	ERR_MSI_REFUSE_NOT_PERMITTED_GROUP = 14,  // MSI_REFUSE_NOT_PERMITTED_GROUP
	ERR_MSI_REFUSE_NOT_PERMITTED_GROUP2 = 15,  // MSI_REFUSE_NOT_PERMITTED_GROUP
	ERR_DELETED_ACCOUNT = 99,  // This ID has been totally erased
	ERR_SESSION_CONNECTED = 100, // Login information remains at %s
	ERR_HACKING_INVESTIGATION = 101, // Account has been locked for a hacking investigation. Please contact the GM Team for more information
	ERR_BUG_INVESTIGATION = 102, // This account has been temporarily prohibited from login due to a bug-related investigation
	ERR_DELETING_CHAR = 103, // This character is being deleted. Login is temporarily unavailable for the time being
	ERR_DELETING_SPOUSE = 104, // This character is being deleted. Login is temporarily unavailable for the time being
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_AC_REFUSE_LOGIN = 0x006a
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_AC_REFUSE_LOGIN = 0x006a
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_REFUSE_LOGIN = 0x006a
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_AC_REFUSE_LOGIN = 0x006a
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REFUSE_LOGIN = 0x006a
#else
ID_AC_REFUSE_LOGIN = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REFUSE_LOGIN
 *
 */ 
class AC_REFUSE_LOGIN : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REFUSE_LOGIN(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REFUSE_LOGIN, s)
	{}
	virtual ~AC_REFUSE_LOGIN() {}

	void deliver(login_error_codes error_code, char* block_date, std::size_t bd_size);
	ByteBuffer &serialize();

/* Structure */
	uint8_t _error_code{ 0 };      ///< Error code
	char _block_date[20]{ 0 };      ///< Ban expiration date
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100615
ID_AC_REFUSE_LOGIN2 = 0x083e
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100615
ID_AC_REFUSE_LOGIN2 = 0x083e
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REFUSE_LOGIN2 = 0x083e
#else
ID_AC_REFUSE_LOGIN2 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REFUSE_LOGIN2
 *
 */ 
class AC_REFUSE_LOGIN2 : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REFUSE_LOGIN2(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REFUSE_LOGIN2, s)
	{}
	virtual ~AC_REFUSE_LOGIN2() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20130220
ID_AC_REFUSE_LOGIN3 = 0x09a5
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20130220
ID_AC_REFUSE_LOGIN3 = 0x09a5
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REFUSE_LOGIN3 = 0x09a5
#else
ID_AC_REFUSE_LOGIN3 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REFUSE_LOGIN3
 *
 */ 
class AC_REFUSE_LOGIN3 : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REFUSE_LOGIN3(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REFUSE_LOGIN3, s)
	{}
	virtual ~AC_REFUSE_LOGIN3() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20180704
ID_AC_REFUSE_LOGIN4 = 0x0b02
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20180704
ID_AC_REFUSE_LOGIN4 = 0x0b02
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20180627
ID_AC_REFUSE_LOGIN4 = 0x0b02
#else
ID_AC_REFUSE_LOGIN4 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REFUSE_LOGIN4
 *
 */ 
class AC_REFUSE_LOGIN4 : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REFUSE_LOGIN4(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REFUSE_LOGIN4, s)
	{}
	virtual ~AC_REFUSE_LOGIN4() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20070227
ID_AC_REQUEST_SECOND_PASSWORD = 0x02ad
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20070227
ID_AC_REQUEST_SECOND_PASSWORD = 0x02ad
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_REQUEST_SECOND_PASSWORD = 0x02ad
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20070227
ID_AC_REQUEST_SECOND_PASSWORD = 0x02ad
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REQUEST_SECOND_PASSWORD = 0x02ad
#else
ID_AC_REQUEST_SECOND_PASSWORD = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REQUEST_SECOND_PASSWORD
 *
 */ 
class AC_REQUEST_SECOND_PASSWORD : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REQUEST_SECOND_PASSWORD(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REQUEST_SECOND_PASSWORD, s)
	{}
	virtual ~AC_REQUEST_SECOND_PASSWORD() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_ACCOUNT_INFO = 0x0270
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_ACCOUNT_INFO = 0x0270
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_REQ_LOGIN_ACCOUNT_INFO = 0x0270
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_ACCOUNT_INFO = 0x0270
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REQ_LOGIN_ACCOUNT_INFO = 0x0270
#else
ID_AC_REQ_LOGIN_ACCOUNT_INFO = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REQ_LOGIN_ACCOUNT_INFO
 *
 */ 
class AC_REQ_LOGIN_ACCOUNT_INFO : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REQ_LOGIN_ACCOUNT_INFO(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REQ_LOGIN_ACCOUNT_INFO, s)
	{}
	virtual ~AC_REQ_LOGIN_ACCOUNT_INFO() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_CARDPASS = 0x0263
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_CARDPASS = 0x0263
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_REQ_LOGIN_CARDPASS = 0x0263
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_CARDPASS = 0x0263
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REQ_LOGIN_CARDPASS = 0x0263
#else
ID_AC_REQ_LOGIN_CARDPASS = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REQ_LOGIN_CARDPASS
 *
 */ 
class AC_REQ_LOGIN_CARDPASS : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REQ_LOGIN_CARDPASS(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REQ_LOGIN_CARDPASS, s)
	{}
	virtual ~AC_REQ_LOGIN_CARDPASS() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_NEWEKEY = 0x0262
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_NEWEKEY = 0x0262
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_REQ_LOGIN_NEWEKEY = 0x0262
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_NEWEKEY = 0x0262
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REQ_LOGIN_NEWEKEY = 0x0262
#else
ID_AC_REQ_LOGIN_NEWEKEY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REQ_LOGIN_NEWEKEY
 *
 */ 
class AC_REQ_LOGIN_NEWEKEY : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REQ_LOGIN_NEWEKEY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REQ_LOGIN_NEWEKEY, s)
	{}
	virtual ~AC_REQ_LOGIN_NEWEKEY() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_OLDEKEY = 0x0261
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_OLDEKEY = 0x0261
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_AC_REQ_LOGIN_OLDEKEY = 0x0261
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_AC_REQ_LOGIN_OLDEKEY = 0x0261
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REQ_LOGIN_OLDEKEY = 0x0261
#else
ID_AC_REQ_LOGIN_OLDEKEY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REQ_LOGIN_OLDEKEY
 *
 */ 
class AC_REQ_LOGIN_OLDEKEY : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REQ_LOGIN_OLDEKEY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REQ_LOGIN_OLDEKEY, s)
	{}
	virtual ~AC_REQ_LOGIN_OLDEKEY() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20130130
ID_AC_REQ_MOBILE_OTP = 0x09a2
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20130130
ID_AC_REQ_MOBILE_OTP = 0x09a2
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REQ_MOBILE_OTP = 0x09a2
#else
ID_AC_REQ_MOBILE_OTP = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REQ_MOBILE_OTP
 *
 */ 
class AC_REQ_MOBILE_OTP : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REQ_MOBILE_OTP(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REQ_MOBILE_OTP, s)
	{}
	virtual ~AC_REQ_MOBILE_OTP() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20120716
ID_AC_REQ_NEW_USER = 0x098b
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20120716
ID_AC_REQ_NEW_USER = 0x098b
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_REQ_NEW_USER = 0x098b
#else
ID_AC_REQ_NEW_USER = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_REQ_NEW_USER
 *
 */ 
class AC_REQ_NEW_USER : public Base::NetworkPacket<AuthSession>
{
public:
	AC_REQ_NEW_USER(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_REQ_NEW_USER, s)
	{}
	virtual ~AC_REQ_NEW_USER() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20111025
ID_AC_SHUTDOWN_INFO = 0x08e4
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20111025
ID_AC_SHUTDOWN_INFO = 0x08e4
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_SHUTDOWN_INFO = 0x08e4
#else
ID_AC_SHUTDOWN_INFO = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_SHUTDOWN_INFO
 *
 */ 
class AC_SHUTDOWN_INFO : public Base::NetworkPacket<AuthSession>
{
public:
	AC_SHUTDOWN_INFO(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_SHUTDOWN_INFO, s)
	{}
	virtual ~AC_SHUTDOWN_INFO() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20120626
ID_AC_SHUTDOWN_NOTIFY = 0x0986
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20120626
ID_AC_SHUTDOWN_NOTIFY = 0x0986
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_AC_SHUTDOWN_NOTIFY = 0x0986
#else
ID_AC_SHUTDOWN_NOTIFY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_SHUTDOWN_NOTIFY
 *
 */ 
class AC_SHUTDOWN_NOTIFY : public Base::NetworkPacket<AuthSession>
{
public:
	AC_SHUTDOWN_NOTIFY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_SHUTDOWN_NOTIFY, s)
	{}
	virtual ~AC_SHUTDOWN_NOTIFY() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100601
ID_AC_SSO_LOGIN_ACK = 0x0826
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100601
ID_AC_SSO_LOGIN_ACK = 0x0826
#else
ID_AC_SSO_LOGIN_ACK = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: AC_SSO_LOGIN_ACK
 *
 */ 
class AC_SSO_LOGIN_ACK : public Base::NetworkPacket<AuthSession>
{
public:
	AC_SSO_LOGIN_ACK(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_AC_SSO_LOGIN_ACK, s)
	{}
	virtual ~AC_SSO_LOGIN_ACK() {}

	void deliver();
	ByteBuffer &serialize();

/* Structure */
};

} /* namespace Auth
 */} /* namespace Horizon */
#endif /* HORIZON_AUTH_TRANSMITTED_PACKETS_HPP */
