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

#ifndef HORIZON_AUTH_HANDLED_PACKETS_HPP
#define HORIZON_AUTH_HANDLED_PACKETS_HPP

#include "Server/Common/Base/NetworkPacket.hpp"
#include "Server/Common/Configuration/Horizon.hpp"

namespace Horizon
{
namespace Auth
{
class AuthSession;
enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060126
ID_CA_ACK_LOGIN_ACCOUNT_INFO = 0x0271
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060126
ID_CA_ACK_LOGIN_ACCOUNT_INFO = 0x0271
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_ACK_LOGIN_ACCOUNT_INFO = 0x0271
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060126
ID_CA_ACK_LOGIN_ACCOUNT_INFO = 0x0271
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_ACK_LOGIN_ACCOUNT_INFO = 0x0271
#else
ID_CA_ACK_LOGIN_ACCOUNT_INFO = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_ACK_LOGIN_ACCOUNT_INFO
 *
 */ 
class CA_ACK_LOGIN_ACCOUNT_INFO : public Base::NetworkPacket<AuthSession>
{
public:
	CA_ACK_LOGIN_ACCOUNT_INFO(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_ACK_LOGIN_ACCOUNT_INFO, s)
	{}
	virtual ~CA_ACK_LOGIN_ACCOUNT_INFO() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_CARDPASS = 0x0266
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_CARDPASS = 0x0266
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_ACK_LOGIN_CARDPASS = 0x0266
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_CARDPASS = 0x0266
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_ACK_LOGIN_CARDPASS = 0x0266
#else
ID_CA_ACK_LOGIN_CARDPASS = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_ACK_LOGIN_CARDPASS
 *
 */ 
class CA_ACK_LOGIN_CARDPASS : public Base::NetworkPacket<AuthSession>
{
public:
	CA_ACK_LOGIN_CARDPASS(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_ACK_LOGIN_CARDPASS, s)
	{}
	virtual ~CA_ACK_LOGIN_CARDPASS() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_NEWEKEY = 0x0265
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_NEWEKEY = 0x0265
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_ACK_LOGIN_NEWEKEY = 0x0265
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_NEWEKEY = 0x0265
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_ACK_LOGIN_NEWEKEY = 0x0265
#else
ID_CA_ACK_LOGIN_NEWEKEY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_ACK_LOGIN_NEWEKEY
 *
 */ 
class CA_ACK_LOGIN_NEWEKEY : public Base::NetworkPacket<AuthSession>
{
public:
	CA_ACK_LOGIN_NEWEKEY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_ACK_LOGIN_NEWEKEY, s)
	{}
	virtual ~CA_ACK_LOGIN_NEWEKEY() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_OLDEKEY = 0x0264
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_OLDEKEY = 0x0264
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_ACK_LOGIN_OLDEKEY = 0x0264
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060109
ID_CA_ACK_LOGIN_OLDEKEY = 0x0264
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_ACK_LOGIN_OLDEKEY = 0x0264
#else
ID_CA_ACK_LOGIN_OLDEKEY = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_ACK_LOGIN_OLDEKEY
 *
 */ 
class CA_ACK_LOGIN_OLDEKEY : public Base::NetworkPacket<AuthSession>
{
public:
	CA_ACK_LOGIN_OLDEKEY(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_ACK_LOGIN_OLDEKEY, s)
	{}
	virtual ~CA_ACK_LOGIN_OLDEKEY() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20130130
ID_CA_ACK_MOBILE_OTP = 0x09a3
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20130130
ID_CA_ACK_MOBILE_OTP = 0x09a3
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_ACK_MOBILE_OTP = 0x09a3
#else
ID_CA_ACK_MOBILE_OTP = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_ACK_MOBILE_OTP
 *
 */ 
class CA_ACK_MOBILE_OTP : public Base::NetworkPacket<AuthSession>
{
public:
	CA_ACK_MOBILE_OTP(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_ACK_MOBILE_OTP, s)
	{}
	virtual ~CA_ACK_MOBILE_OTP() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20120716
ID_CA_ACK_NEW_USER = 0x098c
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20120716
ID_CA_ACK_NEW_USER = 0x098c
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_ACK_NEW_USER = 0x098c
#else
ID_CA_ACK_NEW_USER = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_ACK_NEW_USER
 *
 */ 
class CA_ACK_NEW_USER : public Base::NetworkPacket<AuthSession>
{
public:
	CA_ACK_NEW_USER(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_ACK_NEW_USER, s)
	{}
	virtual ~CA_ACK_NEW_USER() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060821
ID_CA_CLIENT_TYPE = 0x027f
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060821
ID_CA_CLIENT_TYPE = 0x027f
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_CLIENT_TYPE = 0x027f
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060821
ID_CA_CLIENT_TYPE = 0x027f
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_CLIENT_TYPE = 0x027f
#else
ID_CA_CLIENT_TYPE = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_CLIENT_TYPE
 *
 */ 
class CA_CLIENT_TYPE : public Base::NetworkPacket<AuthSession>
{
public:
	CA_CLIENT_TYPE(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_CLIENT_TYPE, s)
	{}
	virtual ~CA_CLIENT_TYPE() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_CA_CONNECT_INFO_CHANGED = 0x0200
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20040517
ID_CA_CONNECT_INFO_CHANGED = 0x0200
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_CONNECT_INFO_CHANGED = 0x0200
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20040517
ID_CA_CONNECT_INFO_CHANGED = 0x0200
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_CONNECT_INFO_CHANGED = 0x0200
#else
ID_CA_CONNECT_INFO_CHANGED = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_CONNECT_INFO_CHANGED
 *
 */ 
class CA_CONNECT_INFO_CHANGED : public Base::NetworkPacket<AuthSession>
{
public:
	CA_CONNECT_INFO_CHANGED(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_CONNECT_INFO_CHANGED, s)
	{}
	virtual ~CA_CONNECT_INFO_CHANGED() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_CA_EXE_HASHCHECK = 0x0204
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20040531
ID_CA_EXE_HASHCHECK = 0x0204
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_EXE_HASHCHECK = 0x0204
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20040531
ID_CA_EXE_HASHCHECK = 0x0204
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_EXE_HASHCHECK = 0x0204
#else
ID_CA_EXE_HASHCHECK = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_EXE_HASHCHECK
 *
 */ 
class CA_EXE_HASHCHECK : public Base::NetworkPacket<AuthSession>
{
public:
	CA_EXE_HASHCHECK(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_EXE_HASHCHECK, s)
	{}
	virtual ~CA_EXE_HASHCHECK() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_CA_LOGIN = 0x0064
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_CA_LOGIN = 0x0064
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_LOGIN = 0x0064
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_CA_LOGIN = 0x0064
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN = 0x0064
#else
ID_CA_LOGIN = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN
 *
 */ 
class CA_LOGIN : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN, s)
	{}
	virtual ~CA_LOGIN() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);
	ByteBuffer serialize(int32_t version, std::string username, std::string password, uint8_t client_type);

/* Structure */
	uint32_t _version{ 0 };       ///< Client Version Information
	char _username[24]{ 0 };      ///< Username
	char _password[24]{ 0 };      ///< Password
	uint8_t _client_type{ 0 };    ///< Client Type
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_CA_LOGIN2 = 0x01dd
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_CA_LOGIN2 = 0x01dd
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_LOGIN2 = 0x01dd
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_CA_LOGIN2 = 0x01dd
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN2 = 0x01dd
#else
ID_CA_LOGIN2 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN2
 *
 */ 
class CA_LOGIN2 : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN2(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN2, s)
	{}
	virtual ~CA_LOGIN2() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_CA_LOGIN3 = 0x01fa
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20040322
ID_CA_LOGIN3 = 0x01fa
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_LOGIN3 = 0x01fa
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20040322
ID_CA_LOGIN3 = 0x01fa
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN3 = 0x01fa
#else
ID_CA_LOGIN3 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN3
 *
 */ 
class CA_LOGIN3 : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN3(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN3, s)
	{}
	virtual ~CA_LOGIN3() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060626
ID_CA_LOGIN4 = 0x027c
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060626
ID_CA_LOGIN4 = 0x027c
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_LOGIN4 = 0x027c
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060626
ID_CA_LOGIN4 = 0x027c
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN4 = 0x027c
#else
ID_CA_LOGIN4 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN4
 *
 */ 
class CA_LOGIN4 : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN4(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN4, s)
	{}
	virtual ~CA_LOGIN4() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20110726
ID_CA_LOGIN5 = 0x08cc
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20110726
ID_CA_LOGIN5 = 0x08cc
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN5 = 0x08cc
#else
ID_CA_LOGIN5 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN5
 *
 */ 
class CA_LOGIN5 : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN5(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN5, s)
	{}
	virtual ~CA_LOGIN5() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20120702
ID_CA_LOGIN6 = 0x0987
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20120702
ID_CA_LOGIN6 = 0x0987
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN6 = 0x0987
#else
ID_CA_LOGIN6 = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN6
 *
 */ 
class CA_LOGIN6 : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN6(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN6, s)
	{}
	virtual ~CA_LOGIN6() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20070514
ID_CA_LOGIN_HAN = 0x02b0
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20070514
ID_CA_LOGIN_HAN = 0x02b0
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_LOGIN_HAN = 0x02b0
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20070514
ID_CA_LOGIN_HAN = 0x02b0
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN_HAN = 0x02b0
#else
ID_CA_LOGIN_HAN = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN_HAN
 *
 */ 
class CA_LOGIN_HAN : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN_HAN(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN_HAN, s)
	{}
	virtual ~CA_LOGIN_HAN() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20171115
ID_CA_LOGIN_OTP = 0x0acf
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20130605
ID_CA_LOGIN_OTP = 0x09c8
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20130605
ID_CA_LOGIN_OTP = 0x09c8
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20171115
ID_CA_LOGIN_OTP = 0x0acf
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN_OTP = 0x09c8
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20171113
ID_CA_LOGIN_OTP = 0x0acf
#else
ID_CA_LOGIN_OTP = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN_OTP
 *
 */ 
class CA_LOGIN_OTP : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN_OTP(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN_OTP, s)
	{}
	virtual ~CA_LOGIN_OTP() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20060424
ID_CA_LOGIN_PCBANG = 0x0277
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20060424
ID_CA_LOGIN_PCBANG = 0x0277
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_LOGIN_PCBANG = 0x0277
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20060424
ID_CA_LOGIN_PCBANG = 0x0277
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_LOGIN_PCBANG = 0x0277
#else
ID_CA_LOGIN_PCBANG = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_LOGIN_PCBANG
 *
 */ 
class CA_LOGIN_PCBANG : public Base::NetworkPacket<AuthSession>
{
public:
	CA_LOGIN_PCBANG(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_LOGIN_PCBANG, s)
	{}
	virtual ~CA_LOGIN_PCBANG() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100413
ID_CA_OTP_AUTH_REQ = 0x0822
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100413
ID_CA_OTP_AUTH_REQ = 0x0822
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_OTP_AUTH_REQ = 0x0822
#else
ID_CA_OTP_AUTH_REQ = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_OTP_AUTH_REQ
 *
 */ 
class CA_OTP_AUTH_REQ : public Base::NetworkPacket<AuthSession>
{
public:
	CA_OTP_AUTH_REQ(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_OTP_AUTH_REQ, s)
	{}
	virtual ~CA_OTP_AUTH_REQ() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20170614
ID_CA_OTP_CODE = 0x0ad0
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20170614
ID_CA_OTP_CODE = 0x0ad0
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_OTP_CODE = 0x0ad0
#else
ID_CA_OTP_CODE = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_OTP_CODE
 *
 */ 
class CA_OTP_CODE : public Base::NetworkPacket<AuthSession>
{
public:
	CA_OTP_CODE(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_OTP_CODE, s)
	{}
	virtual ~CA_OTP_CODE() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_CA_REPLY_PNGAMEROOM = 0x01bf
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_CA_REPLY_PNGAMEROOM = 0x01bf
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_REPLY_PNGAMEROOM = 0x01bf
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_CA_REPLY_PNGAMEROOM = 0x01bf
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_REPLY_PNGAMEROOM = 0x01bf
#else
ID_CA_REPLY_PNGAMEROOM = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_REPLY_PNGAMEROOM
 *
 */ 
class CA_REPLY_PNGAMEROOM : public Base::NetworkPacket<AuthSession>
{
public:
	CA_REPLY_PNGAMEROOM(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_REPLY_PNGAMEROOM, s)
	{}
	virtual ~CA_REPLY_PNGAMEROOM() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20050912
ID_CA_REQ_GAME_GUARD_CHECK = 0x0258
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20050912
ID_CA_REQ_GAME_GUARD_CHECK = 0x0258
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_REQ_GAME_GUARD_CHECK = 0x0258
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20050912
ID_CA_REQ_GAME_GUARD_CHECK = 0x0258
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_REQ_GAME_GUARD_CHECK = 0x0258
#else
ID_CA_REQ_GAME_GUARD_CHECK = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_REQ_GAME_GUARD_CHECK
 *
 */ 
class CA_REQ_GAME_GUARD_CHECK : public Base::NetworkPacket<AuthSession>
{
public:
	CA_REQ_GAME_GUARD_CHECK(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_REQ_GAME_GUARD_CHECK, s)
	{}
	virtual ~CA_REQ_GAME_GUARD_CHECK() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'A' &&  \
	PACKET_VERSION >= 20040000
ID_CA_REQ_HASH = 0x01db
#elif CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20030000
ID_CA_REQ_HASH = 0x01db
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20080000
ID_CA_REQ_HASH = 0x01db
#elif CLIENT_TYPE == 'S' &&  \
	PACKET_VERSION >= 20030000
ID_CA_REQ_HASH = 0x01db
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_REQ_HASH = 0x01db
#else
ID_CA_REQ_HASH = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_REQ_HASH
 *
 */ 
class CA_REQ_HASH : public Base::NetworkPacket<AuthSession>
{
public:
	CA_REQ_HASH(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_REQ_HASH, s)
	{}
	virtual ~CA_REQ_HASH() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

enum {
#if CLIENT_TYPE == 'M' &&  \
	PACKET_VERSION >= 20100601
ID_CA_SSO_LOGIN_REQ = 0x0825
#elif CLIENT_TYPE == 'R' &&  \
	PACKET_VERSION >= 20100601
ID_CA_SSO_LOGIN_REQ = 0x0825
#elif CLIENT_TYPE == 'Z' &&  \
	PACKET_VERSION >= 20170000
ID_CA_SSO_LOGIN_REQ = 0x0825
#else
ID_CA_SSO_LOGIN_REQ = 0x0000 // Disabled
#endif
};
/**
 * @brief Main object for the aegis packet: CA_SSO_LOGIN_REQ
 *
 */ 
class CA_SSO_LOGIN_REQ : public Base::NetworkPacket<AuthSession>
{
public:
	CA_SSO_LOGIN_REQ(std::shared_ptr<AuthSession> s)
	: NetworkPacket<AuthSession>(ID_CA_SSO_LOGIN_REQ, s)
	{}
	virtual ~CA_SSO_LOGIN_REQ() {}

	void handle(ByteBuffer &&buf);
	void deserialize(ByteBuffer &buf);

/* Structure */
};

} /* namespace Auth */
} /* namespace Horizon */
#endif /* HORIZON_AUTH_TRANSMITTED_PACKETS_HPP */
