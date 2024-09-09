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

#include "HandledPackets.hpp"
#include "Server/Zone/Zone.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Interface/UI/UserInterface.hpp"

using namespace Horizon::Zone;

/**
 * CZ_CHANGE_DIRECTION
 */
void CZ_CHANGE_DIRECTION::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->change_direction(_head_direction, _body_direction);
}
void CZ_CHANGE_DIRECTION::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _head_direction;
	buf >> _body_direction;
}
/**
 * CZ_ENTER
 */
void CZ_ENTER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->login(_account_id, _char_id, _auth_code, _client_time, _gender);
}
void CZ_ENTER::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _account_id;
	buf >> _char_id;
	buf >> _auth_code;
	buf >> _client_time;
	buf >> _gender;
}
/**
 * CZ_ITEM_PICKUP
 */
void CZ_ITEM_PICKUP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->pickup_item(_guid);
}
void CZ_ITEM_PICKUP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guid;
}
/**
 * CZ_ITEM_THROW
 */
void CZ_ITEM_THROW::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->throw_item(_inventory_index, _amount);
}
void CZ_ITEM_THROW::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_MOVE_ITEM_FROM_STORE_TO_BODY
 */
void CZ_MOVE_ITEM_FROM_STORE_TO_BODY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_storage_to_inventory(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_STORE_TO_BODY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_REQNAME
 */
void CZ_REQNAME::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->notify_unit_name(_guid);
}

void CZ_REQNAME::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _guid;
}
/**
 * CZ_REQNAME_BYGID
 */
void CZ_REQNAME_BYGID::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->request_name_by_char_id(_char_id);
}
void CZ_REQNAME_BYGID::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _char_id;
}
/**
 * CZ_REQUEST_ACT
 */
void CZ_REQUEST_ACT::handle(ByteBuffer &&buf)
{
	deserialize(buf);
    get_session()->clif()->action_request(_target_guid, (player_action_type) _action);
}

void CZ_REQUEST_ACT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _target_guid;
	buf >> _action;
}
/**
 * CZ_REQUEST_CHAT
 */
void CZ_REQUEST_CHAT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->parse_chat_message(_message);
}

void CZ_REQUEST_CHAT::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _packet_length;

	int str_len = _packet_length - 4;
	_message = new char[str_len + 1];
	_message[str_len] = '\0';
	buf.read(_message, str_len);
}
/**
 * CZ_REQUEST_MOVE
 */
void CZ_REQUEST_MOVE::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->walk_to_coordinates(_x, _y, _dir);
}

void CZ_REQUEST_MOVE::deserialize(ByteBuffer &buf)
{
	char packed_pos[3];
	buf >> _packet_id;
	buf.read(packed_pos, sizeof(packed_pos));
	UnpackPosition((uint8_t *) packed_pos, &_x, &_y, &_dir);
}
/**
 * CZ_REQUEST_TIME
 */
void CZ_REQUEST_TIME::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->notify_time();
}

void CZ_REQUEST_TIME::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _timestamp;
}
/**
 * CZ_REQ_WEAPONREFINE
 */
void CZ_REQ_WEAPONREFINE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_weapon_refine(_inventory_index);
}
void CZ_REQ_WEAPONREFINE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
}
/**
 * CZ_USE_SKILL
 */
void CZ_USE_SKILL::handle(ByteBuffer &&buf) {
	deserialize(buf);
	get_session()->clif()->use_skill_on_target(_skill_lv, _skill_id, _target_id); 
}
void CZ_USE_SKILL::deserialize(ByteBuffer &buf) {
	buf >> _packet_id;
	buf >> _skill_lv;
	buf >> _skill_id;
	buf >> _target_id;
}
/**
 * CZ_USE_SKILL_TOGROUND
 */
void CZ_USE_SKILL_TOGROUND::handle(ByteBuffer &&buf) {
	deserialize(buf);
	get_session()->clif()->use_skill_on_ground(_skill_lv, _skill_id, _pos_x, _pos_y); 	
}
void CZ_USE_SKILL_TOGROUND::deserialize(ByteBuffer &buf) {
	buf >> _packet_id;
	buf >> _skill_lv;
	buf >> _skill_id;
	buf >> _pos_x;	
	buf >> _pos_y;	
}
/**
 * CZ_USE_SKILL_TOGROUND_WITHTALKBOX
 */
void CZ_USE_SKILL_TOGROUND_WITHTALKBOX::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->use_skill_on_ground(_skill_lv, _skill_id, _pos_x, _pos_y, _contents);
}
void CZ_USE_SKILL_TOGROUND_WITHTALKBOX::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _skill_lv;
	buf >> _skill_id;
	buf >> _pos_x;	
	buf >> _pos_y;
	buf.read(_contents, MAX_TALKBOX_CONTENT_SIZE);
}
/**
 * CZ_ACK_EXCHANGE_ITEM
 */
void CZ_ACK_EXCHANGE_ITEM::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->trade().response(_result);
}

void CZ_ACK_EXCHANGE_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _result;
}
/**
 * CZ_ACK_REQ_ADD_FRIENDS
 */
void CZ_ACK_REQ_ADD_FRIENDS::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->friend_().request(_inviter_account_id, _inviter_char_id, _result == 0 ? CZ_ACKREQ_ADDFRIENDS_REJECTED : CZ_ACKREQ_ADDFRIENDS_ACCEPTED);
}
void CZ_ACK_REQ_ADD_FRIENDS::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inviter_account_id;
	buf >> _inviter_char_id;
	buf >> _result;
}
/**
 * CZ_ACK_SELECT_DEALTYPE
 */
void CZ_ACK_SELECT_DEALTYPE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_select_deal_type(_guid, _type == 0 ? CZ_ACKSELECT_DEALTYPE_BUY : CZ_ACKSELECT_DEALTYPE_SELL);
}
void CZ_ACK_SELECT_DEALTYPE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guid;
	buf >> _type;
}
/**
 * CZ_ADD_EXCHANGE_ITEM
 */
void CZ_ADD_EXCHANGE_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	switch (_inventory_index) {
	case 0:
		get_session()->clif()->trade().add_zeny(_amount);
		break;
	default:
		get_session()->clif()->trade().add_item(_inventory_index, _amount);
		break;
	}
}

void CZ_ADD_EXCHANGE_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_ADD_FRIENDS
 */
void CZ_ADD_FRIENDS::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->friend_().add(_name);
}
void CZ_ADD_FRIENDS::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_ALLY_GUILD
 */
void CZ_ALLY_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().invite_ally_response(_inviter_account_id, _response == 1 ? CZ_ALLY_GUILD_RESPONSE_ACCEPT : CZ_ALLY_GUILD_RESPONSE_REFUSE);
}

void CZ_ALLY_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inviter_account_id;
	buf >> _response;
}
/**
 * CZ_BROADCAST
 */
void CZ_BROADCAST::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->broadcast(_message);
}
void CZ_BROADCAST::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_message, _packet_length - 4);
}
/**
 * CZ_CANCEL_EXCHANGE_ITEM
 */
void CZ_CANCEL_EXCHANGE_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->trade().cancel();
}

void CZ_CANCEL_EXCHANGE_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
}
/**
 * CZ_CANCEL_LOCKON
 */
void CZ_CANCEL_LOCKON::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->stop_attack();
}

void CZ_CANCEL_LOCKON::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
}
/**
 * CZ_CHANGE_CHATROOM
 */
void CZ_CHANGE_CHATROOM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
}

void CZ_CHANGE_CHATROOM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _limit;
	buf >> _type;
	buf.read(_password, CHATROOM_PASS_SIZE);
	buf.read(_title, CHATROOM_TITLE_SIZE);
}
/**
 * CZ_CHANGE_EFFECTSTATE
 */
void CZ_CHANGE_EFFECTSTATE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->change_effect_state(_effect_state);
}
void CZ_CHANGE_EFFECTSTATE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _effect_state;
}
/**
 * CZ_CHANGE_GROUPEXPOPTION
 */
void CZ_CHANGE_GROUPEXPOPTION::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().change_properties(_exp_share_rule, 0, 0);
}

void CZ_CHANGE_GROUPEXPOPTION::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _exp_share_rule;
}
/**
 * CZ_CHANGE_MAPTYPE
 */
void CZ_CHANGE_MAPTYPE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->change_map_type(_x, _y, _walkable ? true : false);
}
void CZ_CHANGE_MAPTYPE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _x;
	buf >> _y;
	buf >> _walkable;
}
/**
 * CZ_CHOOSE_MENU
 */
void CZ_CHOOSE_MENU::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_select_menu(_npc_guid, _choice);
}
void CZ_CHOOSE_MENU::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _npc_guid;
	buf >> _choice;
}
/**
 * CZ_CHOPOKGI
 */
void CZ_CHOPOKGI::handle(ByteBuffer &&buf) { get_session()->clif()->novice_explosion_spirits(); }
void CZ_CHOPOKGI::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_DIALOG
 */
void CZ_CLOSE_DIALOG::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_close(_npc_guid);
}
void CZ_CLOSE_DIALOG::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _npc_guid;
}
/**
 * CZ_COMMAND_PET
 */
void CZ_COMMAND_PET::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->command_pet((cz_command_pet_type) _command);
}
void CZ_COMMAND_PET::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _command;	
}
/**
 * CZ_CONCLUDE_EXCHANGE_ITEM
 */
void CZ_CONCLUDE_EXCHANGE_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->trade().lock();
}

void CZ_CONCLUDE_EXCHANGE_ITEM::deserialize(ByteBuffer &buf) 
{ 
	buf >> _packet_id;
}
/**
 * CZ_CONTACTNPC
 */
void CZ_CONTACTNPC::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->npc_contact(_guid);
}
void CZ_CONTACTNPC::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guid;
	buf >> _type;
}
/**
 * CZ_CREATE_CHATROOM
 */
void CZ_CREATE_CHATROOM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	if (_packet_length - 15 < 1) // If title is less than size of 1.
		return;
	
	get_session()->clif()->chatroom().create_chatroom(_limit, _public, _password, _title);
}

void CZ_CREATE_CHATROOM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _limit;
	buf >> _public;
	buf.read(_password, CHATROOM_PASS_SIZE);
	buf.read(_title, std::min((_packet_length - 16), CHATROOM_TITLE_SIZE));
}
/**
 * CZ_DELETE_FRIENDS
 */
void CZ_DELETE_FRIENDS::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->friend_().remove(_account_id, _char_id);
}
void CZ_DELETE_FRIENDS::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
	buf >> _char_id;
}
/**
 * CZ_DISCONNECT_ALL_CHARACTER
 * /killall GM command
 */
void CZ_DISCONNECT_ALL_CHARACTER::handle(ByteBuffer &&buf) 
{
	get_session()->clif()->disconnect_all_players();
}
void CZ_DISCONNECT_ALL_CHARACTER::deserialize(ByteBuffer &buf) {}

/**
 * CZ_DISCONNECT_CHARACTER
 */
void CZ_DISCONNECT_CHARACTER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->disconnect_account(_account_id);	
}
void CZ_DISCONNECT_CHARACTER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_DORIDORI
 */
void CZ_DORIDORI::handle(ByteBuffer &&buf) { get_session()->clif()->novice_doridori(); }
void CZ_DORIDORI::deserialize(ByteBuffer &buf) {}
/**
 * CZ_EXEC_EXCHANGE_ITEM
 */
void CZ_EXEC_EXCHANGE_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->trade().commit();
}

void CZ_EXEC_EXCHANGE_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
}
/**
 * CZ_EXIT_ROOM
 */
void CZ_EXIT_ROOM::handle(ByteBuffer &&buf) 
{ 
	deserialize(buf);
	get_session()->clif()->chatroom().leave();
}

void CZ_EXIT_ROOM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
}
/**
 * CZ_GUILD_CHAT
 */
void CZ_GUILD_CHAT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().send_message(_message);
}

void CZ_GUILD_CHAT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_message, MAX_CHAT_STR_LENGTH);
}
/**
 * CZ_GUILD_NOTICE
 */
void CZ_GUILD_NOTICE::handle(ByteBuffer &&buf) 
{ 
	deserialize(buf);
	get_session()->clif()->guild().change_notice(_guild_id, _subject, _notice);
}

void CZ_GUILD_NOTICE::deserialize(ByteBuffer &buf) 
{ 
	buf >> _packet_id;
	buf >> _guild_id;
	buf.read(_subject, MAX_GUILD_SUBJECT_STR_LENGTH);
	buf.read(_notice, MAX_GUILD_NOTICE_STR_LENGTH);
}
/**
 * CZ_GUILD_ZENY
 */
void CZ_GUILD_ZENY::handle(ByteBuffer &&buf) {}
void CZ_GUILD_ZENY::deserialize(ByteBuffer &buf) {}
/**
 * CZ_INPUT_EDITDLG
 */
void CZ_INPUT_EDITDLG::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_input(_npc_guid, _value);
}
void CZ_INPUT_EDITDLG::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _npc_guid;
	buf >> _value;
}
/**
 * CZ_ITEM_CREATE
 */
void CZ_ITEM_CREATE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->create(_create);
}
void CZ_ITEM_CREATE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_create, MAX_ITEM_CREATE_CMD_INPUT_LENGTH);
}
/**
 * CZ_JOIN_BABY
 */
void CZ_JOIN_BABY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->register_baby(_account_id, _character_id, _response ? CZ_JOINBABY_ACCEPTED : CZ_JOINBABY_REJECTED);
}
void CZ_JOIN_BABY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
	buf >> _character_id;
	buf >> _response;
}
/**
 * CZ_JOIN_COUPLE
 */
void CZ_JOIN_COUPLE::handle(ByteBuffer &&buf) {}
void CZ_JOIN_COUPLE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_JOIN_GROUP
 */
void CZ_JOIN_GROUP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().invite_response(_party_id, (_response ? PARTY_INVITE_RESPONSE_ACCEPTED : PARTY_INVITE_RESPONSE_REJECTED));
}

void CZ_JOIN_GROUP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _party_id;
	buf >> _response;
}
/**
 * CZ_JOIN_GUILD
 */
void CZ_JOIN_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().invite_response(_guild_id, _response == 1 ? CZ_JOIN_GUILD_RESPONSE_ACCEPT : CZ_JOIN_GUILD_RESPONSE_REFUSE);
}

void CZ_JOIN_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guild_id;
	buf >> _response;
}
/**
 * CZ_MAKE_GROUP
 */
void CZ_MAKE_GROUP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().create(_party_name, 0, 0);
}

void CZ_MAKE_GROUP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_party_name, MAX_PARTY_NAME_LENGTH);
}
/**
 * CZ_MAKE_GROUP2
 */
void CZ_MAKE_GROUP2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().create(_party_name, _item_pickup_rule, _item_share_rule);
}

void CZ_MAKE_GROUP2::deserialize(ByteBuffer &buf) 
{ 
	buf >> _packet_id;
	buf.read(_party_name, MAX_PARTY_NAME_LENGTH);
	buf >> _item_pickup_rule;
	buf >> _item_share_rule;
}
/**
 * CZ_MONSTER_TALK
 */
void CZ_MONSTER_TALK::handle(ByteBuffer &&buf) {}
void CZ_MONSTER_TALK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MOVETO_MAP
 */
void CZ_MOVETO_MAP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->move_to_map(_map_name, _x, _y);
}
void CZ_MOVETO_MAP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_map_name, MAP_NAME_LENGTH_EXT);
	buf >> _x >> _y;
}
/**
 * CZ_MOVE_ITEM_FROM_BODY_TO_CART
 */
void CZ_MOVE_ITEM_FROM_BODY_TO_CART::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_inventory_to_cart(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_BODY_TO_CART::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_MOVE_ITEM_FROM_CART_TO_BODY
 */
void CZ_MOVE_ITEM_FROM_CART_TO_BODY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_cart_to_inventory(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_CART_TO_BODY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_MOVE_ITEM_FROM_CART_TO_STORE
 */
void CZ_MOVE_ITEM_FROM_CART_TO_STORE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_cart_to_storage(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_CART_TO_STORE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_MOVE_ITEM_FROM_STORE_TO_CART
 */
void CZ_MOVE_ITEM_FROM_STORE_TO_CART::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_storage_to_cart(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_STORE_TO_CART::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_NOTIFY_ACTORINIT
 */
void CZ_NOTIFY_ACTORINIT::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->map_enter();
}

void CZ_NOTIFY_ACTORINIT::deserialize(ByteBuffer &buf) { }
/**
 * CZ_PC_PURCHASE_ITEMLIST
 */
void CZ_PC_PURCHASE_ITEMLIST::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_purchase_items(_items);
}
void CZ_PC_PURCHASE_ITEMLIST::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	for (int i = 0; i < ((_packet_length - 4) / sizeof(cz_pc_purchase_itemlist)); i++) {
		cz_pc_purchase_itemlist item;
		buf >> item.amount;
		buf >> item.item_id;
		_items.push_back(item);
	}
}
/**
 * CZ_PC_PURCHASE_ITEMLIST_FROMMC
 */
void CZ_PC_PURCHASE_ITEMLIST_FROMMC::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->vending_purchase_items(_account_id, _items);
}
void CZ_PC_PURCHASE_ITEMLIST_FROMMC::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _account_id;
	for (int i = 0; i < ((_packet_length - 8) / sizeof(cz_pc_purchase_itemlist)); i++) {
		cz_pc_purchase_itemlist item;
		buf >> item.amount;
		buf >> item.item_id;
		_items.push_back(item);
	}
}
/**
 * CZ_PC_SELL_ITEMLIST
 */
void CZ_PC_SELL_ITEMLIST::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_sell_items(_items);
}
void CZ_PC_SELL_ITEMLIST::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _packet_length;
	for (int i = 0; i < ((_packet_length - 4) / sizeof(cz_pc_sell_itemlist)); i++) {
		cz_pc_sell_itemlist item;
		buf >> item.inventory_index;
		buf >> item.amount;
		_items.push_back(item);
	}
}
/**
 * CZ_PETEGG_INFO
 */
void CZ_PETEGG_INFO::handle(ByteBuffer &&buf) {}
void CZ_PETEGG_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PET_ACT
 */
void CZ_PET_ACT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->pet_act_emotion((cz_pet_act_emotion_type) _emotion);
}
void CZ_PET_ACT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _emotion;
}
/**
 * CZ_PKMODE_CHANGE
 */
void CZ_PKMODE_CHANGE::handle(ByteBuffer &&buf) {}
void CZ_PKMODE_CHANGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RECALL
 */
void CZ_RECALL::handle(ByteBuffer &&buf) 
{ 
	deserialize(buf);
	get_session()->clif()->recall(_account_name);
}
void CZ_RECALL::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_account_name, MAX_USERNAME_LENGTH);
}
/**
 * CZ_RECALL_GID
 */
void CZ_RECALL_GID::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->summon(_name);
}
void CZ_RECALL_GID::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_REGISTER_GUILD_EMBLEM_IMG
 */
void CZ_REGISTER_GUILD_EMBLEM_IMG::handle(ByteBuffer &&buf) 
{ 
	deserialize(buf);
	get_session()->clif()->guild().change_emblem(_emblem);
}

void CZ_REGISTER_GUILD_EMBLEM_IMG::deserialize(ByteBuffer &buf) 
{ 
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_emblem, std::min(MAX_EMBLEM_LENGTH, _packet_length - 4));
}
/**
 * CZ_REG_CHANGE_GUILD_POSITIONINFO
 */
void CZ_REG_CHANGE_GUILD_POSITIONINFO::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().change_position_info(_info);
}

void CZ_REG_CHANGE_GUILD_POSITIONINFO::deserialize(ByteBuffer &buf) 
{ 
	buf >> _packet_id;
	buf >> _packet_length;
	for (int i = 0; i < ((_packet_length - 4) / sizeof(s_cz_reg_change_guild_positioninfo)); i++) {
		s_cz_reg_change_guild_positioninfo g;
		buf >> g.position_id;
		buf >> g.mode;
		buf >> g.ranking;
		buf >> g.pay_rate;
		buf.read(g.name, MAX_GUILD_POSITION_NAME_LENGTH);
		_info.push_back(g);
	}
}
/**
 * CZ_REMEMBER_WARPPOINT
 */
void CZ_REMEMBER_WARPPOINT::handle(ByteBuffer &&buf) { get_session()->clif()->remember_warppoint(); }
void CZ_REMEMBER_WARPPOINT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REMOVE_AID
 */
void CZ_REMOVE_AID::handle(ByteBuffer &&buf) {}
void CZ_REMOVE_AID::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RENAME_PET
 */
void CZ_RENAME_PET::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->rename_pet(_name);
}
void CZ_RENAME_PET::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_REQMAKINGITEM
 */
void CZ_REQMAKINGITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	std::vector<int16_t> items;
	for (int i = 0; i < 3; i++)
		items.push_back(_material_id[i]);
	get_session()->clif()->produce_item(_item_id, items);
}
void CZ_REQMAKINGITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _item_id;
	for (int i = 0; i < 3; i++)
		buf >> _material_id[i];
}
/**
 * CZ_REQUEST_CHAT_PARTY
 */
void CZ_REQUEST_CHAT_PARTY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().send_message(_packet_length, _message);
}

void CZ_REQUEST_CHAT_PARTY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_message, MAX_CHAT_STR_LENGTH + MAX_UNIT_NAME_LENGTH + 3 + 1);
}
/**
 * CZ_REQUEST_QUIT
 */
void CZ_REQUEST_QUIT::handle(ByteBuffer &&buf) {}
void CZ_REQUEST_QUIT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ACCOUNTNAME
 */
void CZ_REQ_ACCOUNTNAME::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->request_username(_account_id);
}
void CZ_REQ_ACCOUNTNAME::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_ALLY_GUILD
 */
void CZ_REQ_ALLY_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().invite_ally(_account_id, _inviter_account_id, _inviter_char_id);
}

void CZ_REQ_ALLY_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
	buf >> _inviter_account_id;
	buf >> _inviter_char_id;
}
/**
 * CZ_REQ_BAN_GUILD
 */
void CZ_REQ_BAN_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().expel(_guild_id, _account_id, _char_id, _reason);
}

void CZ_REQ_BAN_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guild_id;
	buf >> _account_id;
	buf >> _char_id;
	buf.read(_reason, MAX_GUILD_LEAVE_REASON_STR_LENGTH);
}
/**
 * CZ_REQ_BUY_FROMMC
 */
void CZ_REQ_BUY_FROMMC::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->open_vend_shop(_account_id);
}
void CZ_REQ_BUY_FROMMC::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id; 
}
/**
 * CZ_REQ_CARTOFF
 */
void CZ_REQ_CARTOFF::handle(ByteBuffer &&buf) { get_session()->clif()->remove_cart(); }
void CZ_REQ_CARTOFF::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_CHANGECART
 */
void CZ_REQ_CHANGECART::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->change_cart(_cart_num);
}
void CZ_REQ_CHANGECART::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _cart_num;
}
/**
 * CZ_REQ_CHANGE_MEMBERPOS
 */
void CZ_REQ_CHANGE_MEMBERPOS::handle(ByteBuffer&& buf)
{
	deserialize(buf);
	get_session()->clif()->guild().change_member_positions(_members);
}

void CZ_REQ_CHANGE_MEMBERPOS::deserialize(ByteBuffer& buf)
{
	buf >> _packet_id;
	buf >> _packet_length;
	for (int i = 0; i < ((_packet_length - 4) / sizeof(s_cz_req_change_memberpos)); i++)
	{
		s_cz_req_change_memberpos m;
		buf >> m.account_id;
		buf >> m.char_id;
		buf >> m.position_id;
		_members.push_back(m);
	}
}
/**
 * CZ_REQ_CLOSESTORE
 */
void CZ_REQ_CLOSESTORE::handle(ByteBuffer &&buf) { get_session()->clif()->close_vending(); }
void CZ_REQ_CLOSESTORE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_DELETE_RELATED_GUILD
 */
void CZ_REQ_DELETE_RELATED_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().remove_related_guild(_guild_id, _relation == 1 ? CZ_REQDELETE_RELATEDGUILD_RELATION_ENEMY : CZ_REQDELETE_RELATEDGUILD_RELATION_ALLY);
}

void CZ_REQ_DELETE_RELATED_GUILD::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _guild_id;
	buf >> _relation;
}
/**
 * CZ_REQ_DISCONNECT
 */
void CZ_REQ_DISCONNECT::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->disconnect(_type);
}

void CZ_REQ_DISCONNECT::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_REQ_DISORGANIZE_GUILD
 */
void CZ_REQ_DISORGANIZE_GUILD::handle(ByteBuffer &&buf) 
{ 
	deserialize(buf);
	get_session()->clif()->guild().disband(_key);
}

void CZ_REQ_DISORGANIZE_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_key, MAX_GUILD_NAME_LENGTH);
}
/**
 * CZ_REQ_EMOTION
 */
void CZ_REQ_EMOTION::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->emotion(_type);
}
void CZ_REQ_EMOTION::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_REQ_ENTER_ROOM
 */
void CZ_REQ_ENTER_ROOM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->chatroom().add_member(_chat_id, _password);
}

void CZ_REQ_ENTER_ROOM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _chat_id;
	buf.read(_password, CHATROOM_PASS_SIZE);
}
/**
 * CZ_REQ_EXCHANGE_ITEM
 */
void CZ_REQ_EXCHANGE_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->trade().request(_account_id);
}

void CZ_REQ_EXCHANGE_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_EXPEL_GROUP_MEMBER
 */
void CZ_REQ_EXPEL_GROUP_MEMBER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().expel_member(_account_id, _name);
}

void CZ_REQ_EXPEL_GROUP_MEMBER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_REQ_EXPEL_MEMBER
 */
void CZ_REQ_EXPEL_MEMBER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->chatroom().expel_member(_name);
}

void CZ_REQ_EXPEL_MEMBER::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_REQ_GIVE_MANNER_POINT
 */
void CZ_REQ_GIVE_MANNER_POINT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->give_manner_point(_account_id, (cz_req_give_manner_point_type) _type, _value);
}
void CZ_REQ_GIVE_MANNER_POINT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
	buf >> _type;
	buf >> _value;
}
/**
 * CZ_REQ_GUILD_EMBLEM_IMG
 */
void CZ_REQ_GUILD_EMBLEM_IMG::handle(ByteBuffer &&buf) {}
void CZ_REQ_GUILD_EMBLEM_IMG::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_GUILD_MEMBER_INFO
 */
void CZ_REQ_GUILD_MEMBER_INFO::handle(ByteBuffer &&buf) {}
void CZ_REQ_GUILD_MEMBER_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_GUILD_MENU
 */
void CZ_REQ_GUILD_MENU::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().request_guild_information(_type);
}

void CZ_REQ_GUILD_MENU::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_REQ_GUILD_MENUINTERFACE
 */
void CZ_REQ_GUILD_MENUINTERFACE::handle(ByteBuffer &&buf) 
{ 
	deserialize(buf);
	get_session()->clif()->guild().notify_menu_interface();
}

void CZ_REQ_GUILD_MENUINTERFACE::deserialize(ByteBuffer &buf) 
{ 
	buf >> _packet_id;
}
/**
 * CZ_REQ_HOSTILE_GUILD
 */
void CZ_REQ_HOSTILE_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().add_opposition(_account_id);

}

void CZ_REQ_HOSTILE_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_ITEMCOMPOSITION
 */
void CZ_REQ_ITEMCOMPOSITION::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->display_item_card_composition(_card_index, _equip_index);
}
void CZ_REQ_ITEMCOMPOSITION::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _card_index;
	buf >> _equip_index;
}
/**
 * CZ_REQ_ITEMCOMPOSITION_LIST
 */
void CZ_REQ_ITEMCOMPOSITION_LIST::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->display_item_card_composition(_card_index);
}
void CZ_REQ_ITEMCOMPOSITION_LIST::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _card_index;
}
/**
 * CZ_REQ_ITEMIDENTIFY
 */
void CZ_REQ_ITEMIDENTIFY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->identify_item(_inventory_index);
}
void CZ_REQ_ITEMIDENTIFY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
}
/**
 * CZ_REQ_ITEMREPAIR
 */
void CZ_REQ_ITEMREPAIR::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->repair_item(_inventory_index, _item_id, _refine, _card_id[0], _card_id[1], _card_id[2], _card_id[3]);
}
void CZ_REQ_ITEMREPAIR::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _item_id;
	buf >> _refine;
	for (int i = 0; i < MAX_ITEM_SLOTS; i++)
		buf >> _card_id[i];
}

/**
 * CZ_REQ_JOIN_BABY
 */
void CZ_REQ_JOIN_BABY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->invite_baby(_account_id);
}
void CZ_REQ_JOIN_BABY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_JOIN_COUPLE
 */
void CZ_REQ_JOIN_COUPLE::handle(ByteBuffer &&buf) {}
void CZ_REQ_JOIN_COUPLE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_JOIN_GROUP
 */
void CZ_REQ_JOIN_GROUP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().invite(_account_id);
}

void CZ_REQ_JOIN_GROUP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_JOIN_GUILD
 */
void CZ_REQ_JOIN_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().invite(_account_id, _inviter_account_id, _inviter_char_id);
}

void CZ_REQ_JOIN_GUILD::deserialize(ByteBuffer &buf) 
{ 
	buf >> _packet_id;
	buf >> _account_id;
	buf >> _inviter_account_id;
	buf >> _inviter_char_id;
}
/**
 * CZ_REQ_LEAVE_GROUP
 */
void CZ_REQ_LEAVE_GROUP::handle(ByteBuffer &&buf) { get_session()->clif()->party().leave(); }
void CZ_REQ_LEAVE_GROUP::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_LEAVE_GUILD
 */
void CZ_REQ_LEAVE_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().leave(_guild_id, _account_id, _char_id, _reason);
}

void CZ_REQ_LEAVE_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guild_id;
	buf >> _account_id;
	buf >> _char_id;
	buf.read(_reason, MAX_GUILD_LEAVE_REASON_STR_LENGTH);
}
/**
 * CZ_REQ_MAKE_GUILD
 */
void CZ_REQ_MAKE_GUILD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->guild().create(_char_id, _guild_name);
}

void CZ_REQ_MAKE_GUILD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _char_id;
	buf.read(_guild_name, MAX_GUILD_NAME_LENGTH);
}
/**
 * CZ_REQ_MAKINGARROW
 */
void CZ_REQ_MAKINGARROW::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->make_arrow(_item_id);
}
void CZ_REQ_MAKINGARROW::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _item_id;
}
/**
 * CZ_REQ_NEXT_SCRIPT
 */
void CZ_REQ_NEXT_SCRIPT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_next_dialog(_npc_guid);
}
void CZ_REQ_NEXT_SCRIPT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _npc_guid;
}
/**
 * CZ_REQ_OPENSTORE
 */
void CZ_REQ_OPENSTORE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->start_vending(_shop_name, _items);
}
void CZ_REQ_OPENSTORE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_shop_name, MAX_VENDING_SHOP_NAME_LENGTH);
	for (int i = 0; i < ((_packet_length - 84) / sizeof(struct cz_req_openstore_itemlist)); i++) {
		cz_req_openstore_itemlist item;
		buf >> item.inventory_index;
		buf >> item.amount;
		buf >> item.price;
		_items.push_back(item);
	}
}
/**
 * CZ_REQ_OPENSTORE2
 */
void CZ_REQ_OPENSTORE2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	if ((int) _result)
		get_session()->clif()->start_vending(_shop_name, _items);
}

void CZ_REQ_OPENSTORE2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_shop_name, MAX_VENDING_SHOP_NAME_LENGTH);
	buf >> _result;
	for (int i = 0; i < ((_packet_length - 85) / sizeof(struct cz_req_openstore_itemlist)); i++) {
		cz_req_openstore_itemlist item;
		buf >> item.inventory_index;
		buf >> item.amount;
		buf >> item.price;
		_items.push_back(item);
	}
}
/**
 * CZ_REQ_OPEN_MEMBER_INFO
 */
void CZ_REQ_OPEN_MEMBER_INFO::handle(ByteBuffer &&buf) {}
void CZ_REQ_OPEN_MEMBER_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_REMAINTIME
 */
void CZ_REQ_REMAINTIME::handle(ByteBuffer &&buf) {}
void CZ_REQ_REMAINTIME::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ROLE_CHANGE
 */
void CZ_REQ_ROLE_CHANGE::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->chatroom().role_change(_role, _name);
}

void CZ_REQ_ROLE_CHANGE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _role;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_REQ_STATUS
 */
void CZ_REQ_STATUS::handle(ByteBuffer &&buf) {}
void CZ_REQ_STATUS::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_TAKEOFF_EQUIP
 */
void CZ_REQ_TAKEOFF_EQUIP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->unequip_item(_inventory_index);
}

void CZ_REQ_TAKEOFF_EQUIP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
}
/**
 * CZ_REQ_USER_COUNT
 */
void CZ_REQ_USER_COUNT::handle(ByteBuffer &&buf) { get_session()->clif()->user_count(); }
void CZ_REQ_USER_COUNT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_WEAR_EQUIP
 */
void CZ_REQ_WEAR_EQUIP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->equip_item(_inventory_index, _equip_location_mask);
}

void CZ_REQ_WEAR_EQUIP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _equip_location_mask;
}
/**
 * CZ_REQ_WHISPER_LIST
 */
void CZ_REQ_WHISPER_LIST::handle(ByteBuffer &&buf) { get_session()->clif()->ignore_list(); }
void CZ_REQ_WHISPER_LIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RESET
 */
void CZ_RESET::handle(ByteBuffer &&buf) {}
void CZ_RESET::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RESTART
 */
void CZ_RESTART::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->restart(_type);
}

void CZ_RESTART::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_SELECTAUTOSPELL
 */
void CZ_SELECTAUTOSPELL::handle(ByteBuffer &&buf) {}
void CZ_SELECTAUTOSPELL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SELECT_PETEGG
 */
void CZ_SELECT_PETEGG::handle(ByteBuffer &&buf) {}
void CZ_SELECT_PETEGG::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SELECT_WARPPOINT
 */
void CZ_SELECT_WARPPOINT::handle(ByteBuffer &&buf) {}
void CZ_SELECT_WARPPOINT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SETTING_WHISPER_PC
 */
void CZ_SETTING_WHISPER_PC::handle(ByteBuffer &&buf) {}
void CZ_SETTING_WHISPER_PC::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SETTING_WHISPER_STATE
 */
void CZ_SETTING_WHISPER_STATE::handle(ByteBuffer &&buf) {}
void CZ_SETTING_WHISPER_STATE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SHIFT
 */
void CZ_SHIFT::handle(ByteBuffer &&buf) {}
void CZ_SHIFT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_STATUS_CHANGE
 */
void CZ_STATUS_CHANGE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->increase_status_point((status_point_type) _type, _amount);
}

void CZ_STATUS_CHANGE::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _type;
	buf >> _amount;
}
/**
 * CZ_TRYCAPTURE_MONSTER
 */
void CZ_TRYCAPTURE_MONSTER::handle(ByteBuffer &&buf) {}
void CZ_TRYCAPTURE_MONSTER::deserialize(ByteBuffer &buf) {}
/**
 * CZ_UPGRADE_SKILLLEVEL
 */
void CZ_UPGRADE_SKILLLEVEL::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->upgrade_skill_level(_skill_id);
}

void CZ_UPGRADE_SKILLLEVEL::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _skill_id;
}
/**
 * CZ_WHISPER
 */
void CZ_WHISPER::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->whisper_message(
		_name,
		strnlen(_name, MAX_UNIT_NAME_LENGTH - 1),
		_message,
		strnlen(_message, _packet_length - 4 - MAX_UNIT_NAME_LENGTH)
	);
	delete[] _message;
}

void CZ_WHISPER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
	_message = new char[_packet_length - 4 - MAX_UNIT_NAME_LENGTH];
	buf.read(_message, _packet_length - 4 - MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_INPUT_EDITDLGSTR
 */
void CZ_INPUT_EDITDLGSTR::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->npc_input(_npc_guid, _value);
}
void CZ_INPUT_EDITDLGSTR::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _npc_guid;
	buf.read(_value, _packet_length - 8);
}
/**
 * CZ_LOCALBROADCAST
 */
void CZ_LOCALBROADCAST::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->broadcast_local(_message);
}
void CZ_LOCALBROADCAST::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_message, _packet_length - 4);
}

/**
 * CZ_REQMAKINGHOMUN
 */
void CZ_REQMAKINGHOMUN::handle(ByteBuffer &&buf) {}
void CZ_REQMAKINGHOMUN::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_PVPPOINT
 */
void CZ_REQ_PVPPOINT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->pvpinfo(_character_id, _account_id);
}
void CZ_REQ_PVPPOINT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _character_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_GIVE_MANNER_BYNAME
 */
void CZ_REQ_GIVE_MANNER_BYNAME::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->adjust_manner_by_name(_name);
}
void CZ_REQ_GIVE_MANNER_BYNAME::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_REQ_STATUS_GM
 */
void CZ_REQ_STATUS_GM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->check_status(_name);
}
void CZ_REQ_STATUS_GM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_ALCHEMIST_RANK
 */
void CZ_ALCHEMIST_RANK::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->ranking_alchemist();
}
void CZ_ALCHEMIST_RANK::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
}
/**
 * CZ_BLACKSMITH_RANK
 */
void CZ_BLACKSMITH_RANK::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->ranking_blacksmith();
}
void CZ_BLACKSMITH_RANK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_LESSEFFECT
 */
void CZ_LESSEFFECT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->setting_effects(_setting);
}
void CZ_LESSEFFECT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _setting;
}
/**
 * CZ_REQ_MAKINGITEM
 */
void CZ_REQ_MAKINGITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->make_item((cz_req_makingitem_type) _type, _item_id);
}
void CZ_REQ_MAKINGITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
	buf >> _item_id;
}
/**
 * CZ_AUCTION_ADD_ITEM
 */
void CZ_AUCTION_ADD_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().add_item(_inventory_index, _amount);
}
void CZ_AUCTION_ADD_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_AUCTION_BUY
 */
void CZ_AUCTION_BUY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().buy(_auction_id, _money);
}
void CZ_AUCTION_BUY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _auction_id;
	buf >> _money;
}
/**
 * CZ_AUCTION_CREATE
 */
void CZ_AUCTION_CREATE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().create(_type ? CZ_AUCTIONCREATE_CANCEL : CZ_AUCTIONCREATE_CREATE);
}
void CZ_AUCTION_CREATE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_MAIL_ADD_ITEM
 */
void CZ_MAIL_ADD_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().add_item(_inventory_index, _amount);
}
void CZ_MAIL_ADD_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_MAIL_DELETE
 */
void CZ_MAIL_DELETE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().delete_(_mail_id);
}
void CZ_MAIL_DELETE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _mail_id;
}
/**
 * CZ_MAIL_GET_ITEM
 */
void CZ_MAIL_GET_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().retrieve_attachment(_mail_id);
}
void CZ_MAIL_GET_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _mail_id;
}
/**
 * CZ_MAIL_GET_LIST
 */
void CZ_MAIL_GET_LIST::handle(ByteBuffer &&buf) { get_session()->clif()->mail().list(); }
void CZ_MAIL_GET_LIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MAIL_OPEN
 */
void CZ_MAIL_OPEN::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().open(_mail_id);
}
void CZ_MAIL_OPEN::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _mail_id;
}
/**
 * CZ_MAIL_RESET_ITEM
 */
void CZ_MAIL_RESET_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().reset((cz_mail_reset_item_type) _type);
}
void CZ_MAIL_RESET_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_TAEKWON_RANK
 */
void CZ_TAEKWON_RANK::handle(ByteBuffer &&buf) {}
void CZ_TAEKWON_RANK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ACK_GAME_GUARD
 */
void CZ_ACK_GAME_GUARD::handle(ByteBuffer &&buf) {}
void CZ_ACK_GAME_GUARD::deserialize(ByteBuffer &buf) {}
/**
 * CZ_COMMAND_MER
 */
void CZ_COMMAND_MER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->command_mercenary(_type, (cz_command_mercenary_command_type) _command);
}
void CZ_COMMAND_MER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
	buf >> _command;
}
/**
 * CZ_REQUEST_ACTNPC
 */
void CZ_REQUEST_ACTNPC::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
}
void CZ_REQUEST_ACTNPC::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guid;
	buf >> _target_id;
	buf >> _action;
}
/**
 * CZ_REQUEST_MOVENPC
 */
void CZ_REQUEST_MOVENPC::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->move_homunculus_to_coordinates(_guid, _x, _y, _dir);
}
void CZ_REQUEST_MOVENPC::deserialize(ByteBuffer &buf) 
{
	char packed_pos[3];
	buf >> _packet_id;
	buf >> _guid;
	buf.read(packed_pos, sizeof(packed_pos));
	UnpackPosition((uint8_t *) packed_pos, &_x, &_y, &_dir);
}
/**
 * CZ_REQUEST_MOVETOOWNER
 */
void CZ_REQUEST_MOVETOOWNER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->move_homunculus_to_master(_guid);
}
void CZ_REQUEST_MOVETOOWNER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guid;
}
/**
 * CZ_KILLER_RANK
 */
void CZ_KILLER_RANK::handle(ByteBuffer &&buf) { get_session()->clif()->ranking_pk(); }
void CZ_KILLER_RANK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ACK_STORE_PASSWORD
 */
void CZ_ACK_STORE_PASSWORD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	switch(_type)
	{
		case 2: // change password
		{
			get_session()->clif()->storage_change_password(_storage_password, _new_password);
			break;
		}
		case 3: // check password
		{
			get_session()->clif()->storage_check_password(_storage_password);
			break;
		}
		default:
			HLog(debug) << "CZ_ACK_STORE_PASSWORD::handle: Received unknown type for storage password verification - " << _type;
			break;
	}
}
void CZ_ACK_STORE_PASSWORD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
	buf.read(_storage_password, MAX_STORAGE_PASSWORD_LENGTH);
	buf.read(_new_password, MAX_STORAGE_PASSWORD_LENGTH);
}
/**
 * CZ_RENAME_MER
 */
void CZ_RENAME_MER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->rename_homunculus(_name);
}
void CZ_RENAME_MER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_MAIL_SEND
 */
void CZ_MAIL_SEND::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().send(_recipient, _title, _body);
}
void CZ_MAIL_SEND::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_recipient, MAX_UNIT_NAME_LENGTH);
	buf.read(_title, MAX_MAIL_TITLE_LENGTH);
	buf >> _body_length;
	buf.read(_body, _body_length);
}
/**
 * CZ_AUCTION_ADD
 */
void CZ_AUCTION_ADD::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().add(_now_money, _max_money, _delete_hour);
}
void CZ_AUCTION_ADD::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _now_money;
	buf >> _max_money;
	buf >> _delete_hour;
}
/**
 * CZ_AUCTION_ADD_CANCEL
 */
void CZ_AUCTION_ADD_CANCEL::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().add_cancel(_auction_id);
}
void CZ_AUCTION_ADD_CANCEL::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _auction_id;
}
/**
 * CZ_AUCTION_ITEM_SEARCH
 */
void CZ_AUCTION_ITEM_SEARCH::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().search_item((cz_auction_search_type) _search_type, _auction_id, _search_text, _page_number);
}
void CZ_AUCTION_ITEM_SEARCH::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _search_type;
	buf >> _auction_id;
	buf.read(_search_text, MAX_AUCTION_SEARCH_TEXT_LENGTH);
	buf >> _page_number;
}
/**
 * CZ_AUCTION_REQ_MY_INFO
 */
void CZ_AUCTION_REQ_MY_INFO::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().own_information((cz_auction_reqmyinfo_type) _type);
}
void CZ_AUCTION_REQ_MY_INFO::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_AGREE_STARPLACE
 */
void CZ_AGREE_STARPLACE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->star_gladiator_feel_save((cz_agree_starplace_type) _type);

}
void CZ_AGREE_STARPLACE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_AUCTION_REQ_MY_SELL_STOP
 */
void CZ_AUCTION_REQ_MY_SELL_STOP::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->auction().stop(_auction_id);
}
void CZ_AUCTION_REQ_MY_SELL_STOP::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _auction_id;
}
/**
 * CZ_REQ_MAIL_RETURN
 */
void CZ_REQ_MAIL_RETURN::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().return_(_mail_id, _name);
}
void CZ_REQ_MAIL_RETURN::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _mail_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_HUNTINGLIST
 */
void CZ_HUNTINGLIST::handle(ByteBuffer &&buf) {}
void CZ_HUNTINGLIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_DEATH_QUESTION
 */
void CZ_DEATH_QUESTION::handle(ByteBuffer &&buf) {}
void CZ_DEATH_QUESTION::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GANGSI_RANK
 */
void CZ_GANGSI_RANK::handle(ByteBuffer &&buf) {}
void CZ_GANGSI_RANK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_STANDING_RESURRECTION
 */
void CZ_STANDING_RESURRECTION::handle(ByteBuffer &&buf) { get_session()->clif()->auto_revive(); }
void CZ_STANDING_RESURRECTION::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MER_COMMAND
 */
void CZ_MER_COMMAND::handle(ByteBuffer &&buf) {}
void CZ_MER_COMMAND::deserialize(ByteBuffer &buf) {}
/**
 * CZ_UNUSED_MER_UPGRADE_SKILLLEVEL
 */
void CZ_UNUSED_MER_UPGRADE_SKILLLEVEL::handle(ByteBuffer &&buf) {}
void CZ_UNUSED_MER_UPGRADE_SKILLLEVEL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_UNUSED_MER_USE_SKILL
 */
void CZ_UNUSED_MER_USE_SKILL::handle(ByteBuffer &&buf) {}
void CZ_UNUSED_MER_USE_SKILL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PC_BUY_CASH_POINT_ITEM
 */
void CZ_PC_BUY_CASH_POINT_ITEM::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

}
void CZ_PC_BUY_CASH_POINT_ITEM::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _kafra_points;
	buf >> _count;
	for (int i = 0; i < _count; i++) {
		cz_pc_buy_cash_point_item item;
		buf >> item.amount;
		buf >> item.item_id;
		_items.push_back(item);
	}
}
/**
 * CZ_SRPACKET_START
 */
void CZ_SRPACKET_START::handle(ByteBuffer &&buf) {}
void CZ_SRPACKET_START::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GAMEGUARD_LINGO_READY
 */
void CZ_GAMEGUARD_LINGO_READY::handle(ByteBuffer &&buf) {}
void CZ_GAMEGUARD_LINGO_READY::deserialize(ByteBuffer &buf) {}
/**
 * CZ_KSY_EVENT
 */
void CZ_KSY_EVENT::handle(ByteBuffer &&buf) {}
void CZ_KSY_EVENT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_HACKSHIELD_CRC_MSG
 */
void CZ_HACKSHIELD_CRC_MSG::handle(ByteBuffer &&buf) {}
void CZ_HACKSHIELD_CRC_MSG::deserialize(ByteBuffer &buf) {}
/**
 * CZ_HACKSH_CPX_MSG
 */
void CZ_HACKSH_CPX_MSG::handle(ByteBuffer &&buf) {}
void CZ_HACKSH_CPX_MSG::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ACK_CASH_PASSWORD
 */
void CZ_ACK_CASH_PASSWORD::handle(ByteBuffer &&buf) {}
void CZ_ACK_CASH_PASSWORD::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SRPACKETR2_START
 */
void CZ_SRPACKETR2_START::handle(ByteBuffer &&buf) {}
void CZ_SRPACKETR2_START::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_CONFIG
 */
void CZ_PARTY_CONFIG::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().refuse_invite(((int)_refuse_invite) ? false : true);
}
void CZ_PARTY_CONFIG::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _refuse_invite;
}
/**
 * CZ_PARTY_JOIN_REQ
 */
void CZ_PARTY_JOIN_REQ::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().invite(_name);
}

void CZ_PARTY_JOIN_REQ::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_PARTY_JOIN_REQ_ACK
 */
void CZ_PARTY_JOIN_REQ_ACK::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().invite_response(_party_id, ((int)_response) ? PARTY_INVITE_RESPONSE_ACCEPTED : PARTY_INVITE_RESPONSE_REJECTED);
}

void CZ_PARTY_JOIN_REQ_ACK::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _party_id;
	buf >> _response;
}
/**
 * CZ_ACTIVE_QUEST
 */
void CZ_ACTIVE_QUEST::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->quest().update_status(_quest_id, _active ? CZ_ACTIVE_QUEST_ACTIVE : CZ_ACTIVE_QUEST_INACTIVE);
}
void CZ_ACTIVE_QUEST::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _quest_id;
	buf >> _active;
}
/**
 * CZ_SHORTCUT_KEY_CHANGE
 */
void CZ_SHORTCUT_KEY_CHANGE::handle(ByteBuffer &&buf) {}
void CZ_SHORTCUT_KEY_CHANGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MEMORIALDUNGEON_COMMAND
 */
void CZ_MEMORIALDUNGEON_COMMAND::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->memorial_dungeon_command((cz_memorial_dungeon_command_type) _command);
}
void CZ_MEMORIALDUNGEON_COMMAND::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _command;
}
/**
 * CZ_CONFIG
 */
void CZ_CONFIG::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->set_config((cz_config_type) _type, _setting);
}
void CZ_CONFIG::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
	buf >> _setting;
}
/**
 * CZ_EQUIPWIN_MICROSCOPE
 */
void CZ_EQUIPWIN_MICROSCOPE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->view_equipment(_account_id);
}
void CZ_EQUIPWIN_MICROSCOPE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_BATTLEFIELD_CHAT
 */
void CZ_BATTLEFIELD_CHAT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

}
void CZ_BATTLEFIELD_CHAT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_message, _packet_length - 4);
}
/**
 * CZ_BOT_CHECK
 */
void CZ_BOT_CHECK::handle(ByteBuffer &&buf) {}
void CZ_BOT_CHECK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ITEM_PICKUP_NEW_JAPEN
 */
void CZ_ITEM_PICKUP_NEW_JAPEN::handle(ByteBuffer &&buf) {}
void CZ_ITEM_PICKUP_NEW_JAPEN::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQUEST_MOVE_NEW_JAPEN
 */
void CZ_REQUEST_MOVE_NEW_JAPEN::handle(ByteBuffer &&buf) {}
void CZ_REQUEST_MOVE_NEW_JAPEN::deserialize(ByteBuffer &buf) {}
/**
 * CZ_USE_ITEM_NEW_JAPEN
 */
void CZ_USE_ITEM_NEW_JAPEN::handle(ByteBuffer &&buf) {}
void CZ_USE_ITEM_NEW_JAPEN::deserialize(ByteBuffer &buf) {}
/**
 * CZ_USE_SKILL_NEW_JAPEN
 */
void CZ_USE_SKILL_NEW_JAPEN::handle(ByteBuffer &&buf) {}
void CZ_USE_SKILL_NEW_JAPEN::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CHANGE_DIRECTION2
 */
void CZ_CHANGE_DIRECTION2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->change_direction(_head_direction, _body_direction);
}
void CZ_CHANGE_DIRECTION2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _head_direction;
	buf >> _body_direction;
}
/**
 * CZ_IRMAIL_SEND
 */
void CZ_IRMAIL_SEND::handle(ByteBuffer &&buf) {}
void CZ_IRMAIL_SEND::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ITEM_PICKUP2
 */
void CZ_ITEM_PICKUP2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->pickup_item(_guid);
}
void CZ_ITEM_PICKUP2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _guid;
}
/**
 * CZ_ITEM_THROW2
 */
void CZ_ITEM_THROW2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->throw_item(_inventory_index, _amount);	
}
void CZ_ITEM_THROW2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_MOVE_ITEM_FROM_BODY_TO_STORE2
 */
void CZ_MOVE_ITEM_FROM_BODY_TO_STORE2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_inventory_to_storage(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_BODY_TO_STORE2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_MOVE_ITEM_FROM_STORE_TO_BODY2
 */
void CZ_MOVE_ITEM_FROM_STORE_TO_BODY2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_storage_to_inventory(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_STORE_TO_BODY2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_REQNAME_BYGID2
 */
void CZ_REQNAME_BYGID2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->request_name_by_char_id(_char_id);
}
void CZ_REQNAME_BYGID2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _char_id;
}
/**
 * CZ_REQUEST_TIME2
 */
void CZ_REQUEST_TIME2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->notify_time();
}

void CZ_REQUEST_TIME2::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _timestamp;
}
/**
 * CZ_USE_SKILL_TOGROUND2
 */
void CZ_USE_SKILL_TOGROUND2::handle(ByteBuffer &&buf) {
	deserialize(buf);
	get_session()->clif()->use_skill_on_ground(_skill_lv, _skill_id, _pos_x, _pos_y); 	
}
void CZ_USE_SKILL_TOGROUND2::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _skill_lv;
	buf >> _skill_id;
	buf >> _pos_x;	
	buf >> _pos_y;	
}
/**
 * CZ_IRMAIL_LIST
 */
void CZ_IRMAIL_LIST::handle(ByteBuffer &&buf) {}
void CZ_IRMAIL_LIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQUEST_MOVE2
 */
void CZ_REQUEST_MOVE2::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->walk_to_coordinates(_x, _y, _dir);
}

void CZ_REQUEST_MOVE2::deserialize(ByteBuffer &buf)
{
	char packed_pos[3];
	buf >> _packet_id;
	buf.read(packed_pos, sizeof(packed_pos));
	UnpackPosition((uint8_t *) packed_pos, &_x, &_y, &_dir);
}
/**
 * CZ_USE_SKILL_TOGROUND_WITHTALKBOX2
 */
void CZ_USE_SKILL_TOGROUND_WITHTALKBOX2::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->use_skill_on_ground(_skill_lv, _skill_id, _pos_x, _pos_y, _contents);
}
void CZ_USE_SKILL_TOGROUND_WITHTALKBOX2::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _skill_lv;
	buf >> _skill_id;
	buf >> _pos_x;	
	buf >> _pos_y;
	buf.read(_contents, MAX_TALKBOX_CONTENT_SIZE);
}
/**
 * CZ_REQNAME2
 */
void CZ_REQNAME2::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->notify_unit_name(_guid);
}

void CZ_REQNAME2::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _guid;
}
/**
 * CZ_OPEN_SIMPLE_CASHSHOP_ITEMLIST
 */
void CZ_OPEN_SIMPLE_CASHSHOP_ITEMLIST::handle(ByteBuffer &&buf) {}
void CZ_OPEN_SIMPLE_CASHSHOP_ITEMLIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PROGRESS
 */
void CZ_PROGRESS::handle(ByteBuffer &&buf) { get_session()->clif()->progress_bar_completed(); }
void CZ_PROGRESS::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_WINDOW
 */
void CZ_CLOSE_WINDOW::handle(ByteBuffer &&buf) {}
void CZ_CLOSE_WINDOW::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ENTER2
 */
void CZ_ENTER2::handle(ByteBuffer&& buf)
{
	deserialize(buf);
	get_session()->clif()->login(_account_id, _char_id, _auth_code, _client_time, _gender);
}
void CZ_ENTER2::deserialize(ByteBuffer& buf)
{
	buf >> _packet_id;
	buf >> _account_id;
	buf >> _char_id;
	buf >> _auth_code;
	buf >> _client_time;
	buf >> _gender;
}
/**
 * CZ_ITEMLISTWIN_RES
 */
void CZ_ITEMLISTWIN_RES::handle(ByteBuffer &&buf) {}
void CZ_ITEMLISTWIN_RES::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_BOOKING_REQ_REGISTER
 */
void CZ_PARTY_BOOKING_REQ_REGISTER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	std::vector<int> jobs;
	for (int i = 0; i < MAX_PARTY_BOOKING_JOBS; i++)
		jobs.push_back(_jobs[i]);
	get_session()->clif()->party_booking().register_(_level, _map_id, jobs);
}
void CZ_PARTY_BOOKING_REQ_REGISTER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _level;
	buf >> _map_id;
	for (int i = 0; i < MAX_PARTY_BOOKING_JOBS; i++)
		buf >> _jobs[i];
}
/**
 * CZ_REQ_CLOSE_BUYING_STORE
 */
void CZ_REQ_CLOSE_BUYING_STORE::handle(ByteBuffer &&buf) { get_session()->clif()->close_buying_store(); }
void CZ_REQ_CLOSE_BUYING_STORE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_OPEN_BUYING_STORE
 */
void CZ_REQ_OPEN_BUYING_STORE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	if ((int) _result)
		get_session()->clif()->start_buying_store(_store_name, _zeny_limit, _items);
}
void CZ_REQ_OPEN_BUYING_STORE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _zeny_limit;
	buf >> _result;
	buf.read(_store_name, MAX_VENDING_SHOP_NAME_LENGTH);
	for (int i = 0; i < ((_packet_length - 85) / sizeof(cz_req_open_buying_store_itemlist)); i++) {
		cz_req_open_buying_store_itemlist item;
		buf >> item.item_id;
		buf >> item.amount;
		buf >> item.price;
		_items.push_back(item);
	}
}
/**
 * CZ_JOIN_BATTLE_FIELD
 */
void CZ_JOIN_BATTLE_FIELD::handle(ByteBuffer &&buf) {}
void CZ_JOIN_BATTLE_FIELD::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MOVE_ITEM_FROM_BODY_TO_STORE
 */
void CZ_MOVE_ITEM_FROM_BODY_TO_STORE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);

	get_session()->clif()->move_item_from_inventory_to_storage(_inventory_index, _amount);
}
void CZ_MOVE_ITEM_FROM_BODY_TO_STORE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _amount;
}
/**
 * CZ_REQ_CLICK_TO_BUYING_STORE
 */
void CZ_REQ_CLICK_TO_BUYING_STORE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->open_buying_store(_account_id);
}
void CZ_REQ_CLICK_TO_BUYING_STORE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_TRADE_BUYING_STORE
 */
void CZ_REQ_TRADE_BUYING_STORE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->sell_to_buying_store(_account_id, _store_id, _items);
}
void CZ_REQ_TRADE_BUYING_STORE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _account_id;
	buf >> _store_id;
	for (int i = 0; i < ((_packet_length - 12) / sizeof(cz_req_trade_buying_store_itemlist)); i++) {
		cz_req_trade_buying_store_itemlist item;
		buf >> item.inventory_index;
		buf >> item.item_id;
		buf >> item.amount;
		_items.push_back(item);
	}
}
/**
 * CZ_SEARCH_STORE_INFO
 */
void CZ_SEARCH_STORE_INFO::handle(ByteBuffer &&buf) {}
void CZ_SEARCH_STORE_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SEARCH_STORE_INFO_NEXT_PAGE
 */
void CZ_SEARCH_STORE_INFO_NEXT_PAGE::handle(ByteBuffer &&buf) {}
void CZ_SEARCH_STORE_INFO_NEXT_PAGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SSILIST_ITEM_CLICK
 */
void CZ_SSILIST_ITEM_CLICK::handle(ByteBuffer &&buf) {}
void CZ_SSILIST_ITEM_CLICK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLAN_CHAT
 */
void CZ_CLAN_CHAT::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->clan().message(_message);
}
void CZ_CLAN_CHAT::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf.read(_message, _packet_length - 4);
}
/**
 * CZ_REQUEST_ACT2
 */
void CZ_REQUEST_ACT2::handle(ByteBuffer &&buf) 
{
    deserialize(buf);
    get_session()->clif()->action_request(_target_guid, (player_action_type) _action);
}

void CZ_REQUEST_ACT2::deserialize(ByteBuffer &buf) 
{
    buf >> _packet_id;
    buf >> _target_guid;
    buf >> _action;
}
/**
 * CZ_USE_SKILL2
 */
void CZ_USE_SKILL2::handle(ByteBuffer &&buf) {
	deserialize(buf);
	get_session()->clif()->use_skill_on_target(_skill_lv, _skill_id, _target_id); 
}
void CZ_USE_SKILL2::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _skill_lv;
	buf >> _skill_id;
	buf >> _target_id;
}
/**
 * CZ_REQ_WEAR_EQUIP_V5
 */
void CZ_REQ_WEAR_EQUIP_V5::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->equip_item(_inventory_index, _equip_location_mask);
}

void CZ_REQ_WEAR_EQUIP_V5::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _equip_location_mask;
}
/**
 * CZ_REQ_BANKING_CHECK
 */
void CZ_REQ_BANKING_CHECK::handle(ByteBuffer &&buf) {}
void CZ_REQ_BANKING_CHECK::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_BANKING_DEPOSIT
 */
void CZ_REQ_BANKING_DEPOSIT::handle(ByteBuffer &&buf) {}
void CZ_REQ_BANKING_DEPOSIT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_BANKING_WITHDRAW
 */
void CZ_REQ_BANKING_WITHDRAW::handle(ByteBuffer &&buf) {}
void CZ_REQ_BANKING_WITHDRAW::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_APPLY_BARGAIN_SALE_ITEM
 */
void CZ_REQ_APPLY_BARGAIN_SALE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_APPLY_BARGAIN_SALE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_CASH_BARGAIN_SALE_ITEM_INFO
 */
void CZ_REQ_CASH_BARGAIN_SALE_ITEM_INFO::handle(ByteBuffer &&buf) {}
void CZ_REQ_CASH_BARGAIN_SALE_ITEM_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_REMOVE_BARGAIN_SALE_ITEM
 */
void CZ_REQ_REMOVE_BARGAIN_SALE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_REMOVE_BARGAIN_SALE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_OPEN_BARGAIN_SALE_TOOL
 */
void CZ_OPEN_BARGAIN_SALE_TOOL::handle(ByteBuffer &&buf) {}
void CZ_OPEN_BARGAIN_SALE_TOOL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_CLOSE_BANKING
 */
void CZ_REQ_CLOSE_BANKING::handle(ByteBuffer &&buf) {}
void CZ_REQ_CLOSE_BANKING::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_OPEN_BANKING
 */
void CZ_REQ_OPEN_BANKING::handle(ByteBuffer &&buf) {}
void CZ_REQ_OPEN_BANKING::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_BARGAIN_SALE_TOOL
 */
void CZ_CLOSE_BARGAIN_SALE_TOOL::handle(ByteBuffer &&buf) { get_session()->clif()->bargain_sale_tool_close(); }
void CZ_CLOSE_BARGAIN_SALE_TOOL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_COUNT_BARGAIN_SALE_ITEM
 */
void CZ_REQ_COUNT_BARGAIN_SALE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_COUNT_BARGAIN_SALE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ITEM_CREATE_EX
 */
void CZ_ITEM_CREATE_EX::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->create(_create);
}
void CZ_ITEM_CREATE_EX::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_create, MAX_ITEM_CREATE_CMD_INPUT_LENGTH);
}
/**
 * CZ_NPROTECTGAMEGUARDCSAUTH
 */
void CZ_NPROTECTGAMEGUARDCSAUTH::handle(ByteBuffer &&buf) {}
void CZ_NPROTECTGAMEGUARDCSAUTH::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_CLOSE_GUILD_STORAGE
 */
void CZ_REQ_CLOSE_GUILD_STORAGE::handle(ByteBuffer &&buf) {}
void CZ_REQ_CLOSE_GUILD_STORAGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_OPEN_GUILD_STORAGE
 */
void CZ_REQ_OPEN_GUILD_STORAGE::handle(ByteBuffer &&buf) {}
void CZ_REQ_OPEN_GUILD_STORAGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_NPC_MARKET_CLOSE
 */
void CZ_NPC_MARKET_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_NPC_MARKET_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_NPC_MARKET_PURCHASE
 */
void CZ_NPC_MARKET_PURCHASE::handle(ByteBuffer &&buf) {}
void CZ_NPC_MARKET_PURCHASE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_NPC_TRADE_QUIT
 */
void CZ_NPC_TRADE_QUIT::handle(ByteBuffer &&buf) {}
void CZ_NPC_TRADE_QUIT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_GUILDSTORAGE_LOG
 */
void CZ_REQ_GUILDSTORAGE_LOG::handle(ByteBuffer &&buf) {}
void CZ_REQ_GUILDSTORAGE_LOG::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MOVE_ITEM_FROM_BODY_TO_GUILDSTORAGE
 */
void CZ_MOVE_ITEM_FROM_BODY_TO_GUILDSTORAGE::handle(ByteBuffer &&buf) {}
void CZ_MOVE_ITEM_FROM_BODY_TO_GUILDSTORAGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MOVE_ITEM_FROM_CART_TO_GUILDSTORAGE
 */
void CZ_MOVE_ITEM_FROM_CART_TO_GUILDSTORAGE::handle(ByteBuffer &&buf) {}
void CZ_MOVE_ITEM_FROM_CART_TO_GUILDSTORAGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MOVE_ITEM_FROM_GUILDSTORAGE_TO_BODY
 */
void CZ_MOVE_ITEM_FROM_GUILDSTORAGE_TO_BODY::handle(ByteBuffer &&buf) {}
void CZ_MOVE_ITEM_FROM_GUILDSTORAGE_TO_BODY::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MOVE_ITEM_FROM_GUILDSTORAGE_TO_CART
 */
void CZ_MOVE_ITEM_FROM_GUILDSTORAGE_TO_CART::handle(ByteBuffer &&buf) {}
void CZ_MOVE_ITEM_FROM_GUILDSTORAGE_TO_CART::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_RODEXBOX
 */
void CZ_CLOSE_RODEXBOX::handle(ByteBuffer &&buf) {}
void CZ_CLOSE_RODEXBOX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_OPEN_RODEXBOX
 */
void CZ_OPEN_RODEXBOX::handle(ByteBuffer &&buf) {}
void CZ_OPEN_RODEXBOX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_DELETE_RODEX
 */
void CZ_REQ_DELETE_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_DELETE_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_NEXT_RODEX
 */
void CZ_REQ_NEXT_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_NEXT_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_REFRESH_RODEX
 */
void CZ_REQ_REFRESH_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_REFRESH_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_READ_RODEX
 */
void CZ_REQ_READ_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_READ_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_SEND_RODEX
 */
void CZ_REQ_SEND_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_SEND_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PET_EVOLUTION
 */
void CZ_PET_EVOLUTION::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->pet_evolve(_evolved_pet_egg_id, _items);
}
void CZ_PET_EVOLUTION::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _evolved_pet_egg_id;
	for (int i = 0; i < ((_packet_length - 6) / sizeof(cz_pet_evolution_itemlist)); i++) {
		cz_pet_evolution_itemlist item;
		buf >> item.inventory_index;
		buf >> item.amount;
		_items.push_back(item);
	}
}
/**
 * CZ_SHORTCUTKEYBAR_ROTATE
 */
void CZ_SHORTCUTKEYBAR_ROTATE::handle(ByteBuffer &&buf) {}
void CZ_SHORTCUTKEYBAR_ROTATE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_CANCEL_WRITE_RODEX
 */
void CZ_REQ_CANCEL_WRITE_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_CANCEL_WRITE_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ITEM_FROM_RODEX
 */
void CZ_REQ_ITEM_FROM_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_ITEM_FROM_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ZENY_FROM_RODEX
 */
void CZ_REQ_ZENY_FROM_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_ZENY_FROM_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ADD_ITEM_RODEX
 */
void CZ_REQ_ADD_ITEM_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_ADD_ITEM_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_OPEN_WRITE_RODEX
 */
void CZ_REQ_OPEN_WRITE_RODEX::handle(ByteBuffer &&buf) {}
void CZ_REQ_OPEN_WRITE_RODEX::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_REMOVE_RODEX_ITEM
 */
void CZ_REQ_REMOVE_RODEX_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_REMOVE_RODEX_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CHECK_RECEIVE_CHARACTER_NAME
 */
void CZ_CHECK_RECEIVE_CHARACTER_NAME::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->mail().check_receiver_name(_name);
}
void CZ_CHECK_RECEIVE_CHARACTER_NAME::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_DYNAMICNPC_CREATE_REQUEST
 */
void CZ_DYNAMICNPC_CREATE_REQUEST::handle(ByteBuffer &&buf) {}
void CZ_DYNAMICNPC_CREATE_REQUEST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ROULETTE_INFO
 */
void CZ_REQ_ROULETTE_INFO::handle(ByteBuffer &&buf) {}
void CZ_REQ_ROULETTE_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ACH_REWARD
 */
void CZ_REQ_ACH_REWARD::handle(ByteBuffer &&buf) {}
void CZ_REQ_ACH_REWARD::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_CLOSE_ROULETTE
 */
void CZ_REQ_CLOSE_ROULETTE::handle(ByteBuffer &&buf) {}
void CZ_REQ_CLOSE_ROULETTE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_GENERATE_ROULETTE
 */
void CZ_REQ_GENERATE_ROULETTE::handle(ByteBuffer &&buf) {}
void CZ_REQ_GENERATE_ROULETTE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_OPEN_ROULETTE
 */
void CZ_REQ_OPEN_ROULETTE::handle(ByteBuffer &&buf) {}
void CZ_REQ_OPEN_ROULETTE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_NPC_BARTER_PURCHASE
 */
void CZ_NPC_BARTER_PURCHASE::handle(ByteBuffer &&buf) {}
void CZ_NPC_BARTER_PURCHASE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_START_USE_SKILL
 */
void CZ_START_USE_SKILL::handle(ByteBuffer &&buf) {}
void CZ_START_USE_SKILL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_STOP_USE_SKILL
 */
void CZ_STOP_USE_SKILL::handle(ByteBuffer &&buf) {}
void CZ_STOP_USE_SKILL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_NPC_BARTER_CLOSE
 */
void CZ_NPC_BARTER_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_NPC_BARTER_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_INVENTORY_EXPAND
 */
void CZ_INVENTORY_EXPAND::handle(ByteBuffer &&buf) {}
void CZ_INVENTORY_EXPAND::deserialize(ByteBuffer &buf) {}
/**
 * CZ_INVENTORY_EXPAND_CONFIRMED
 */
void CZ_INVENTORY_EXPAND_CONFIRMED::handle(ByteBuffer &&buf) {}
void CZ_INVENTORY_EXPAND_CONFIRMED::deserialize(ByteBuffer &buf) {}
/**
 * CZ_INVENTORY_EXPAND_REJECTED
 */
void CZ_INVENTORY_EXPAND_REJECTED::handle(ByteBuffer &&buf) {}
void CZ_INVENTORY_EXPAND_REJECTED::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RECV_ROULETTE_ITEM
 */
void CZ_RECV_ROULETTE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_RECV_ROULETTE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ACK_AU_BOT
 */
void CZ_ACK_AU_BOT::handle(ByteBuffer &&buf) {}
void CZ_ACK_AU_BOT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_CHANGE_TITLE
 */
void CZ_REQ_CHANGE_TITLE::handle(ByteBuffer &&buf) {}
void CZ_REQ_CHANGE_TITLE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ONECLICK_ITEMIDENTIFY
 */
void CZ_REQ_ONECLICK_ITEMIDENTIFY::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->identify_item(_inventory_index);
}
void CZ_REQ_ONECLICK_ITEMIDENTIFY::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _inventory_index;
}
/**
 * CZ_PRIVATE_AIRSHIP_REQUEST
 */
void CZ_PRIVATE_AIRSHIP_REQUEST::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->private_airship_request(_map_name, _item_id);
}
void CZ_PRIVATE_AIRSHIP_REQUEST::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf.read(_map_name, MAP_NAME_LENGTH_EXT);
	buf >> _item_id;
}
/**
 * CZ_REQ_OPEN_UI
 */
void CZ_REQ_OPEN_UI::handle(ByteBuffer &&buf) {}
void CZ_REQ_OPEN_UI::deserialize(ByteBuffer &buf) {}
/**
 * CZ_OFFLINE_STORE_CREATE
 */
void CZ_OFFLINE_STORE_CREATE::handle(ByteBuffer &&buf) {}
void CZ_OFFLINE_STORE_CREATE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ALT_EQUIPMENT_EQUIP
 */
void CZ_ALT_EQUIPMENT_EQUIP::handle(ByteBuffer &&buf) {}
void CZ_ALT_EQUIPMENT_EQUIP::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REFINE_ADD_ITEM
 */
void CZ_REFINE_ADD_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REFINE_ADD_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REFINE_ITEM_REQUEST
 */
void CZ_REFINE_ITEM_REQUEST::handle(ByteBuffer &&buf) {}
void CZ_REFINE_ITEM_REQUEST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REFINE_WINDOW_CLOSE
 */
void CZ_REFINE_WINDOW_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_REFINE_WINDOW_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ATTENDANCE_REWARD_REQUEST
 */
void CZ_ATTENDANCE_REWARD_REQUEST::handle(ByteBuffer &&buf) {}
void CZ_ATTENDANCE_REWARD_REQUEST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PING
 */
void CZ_PING::handle(ByteBuffer &&buf) {}
void CZ_PING::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_STORE
 */
void CZ_CLOSE_STORE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->storage_close();
}
void CZ_CLOSE_STORE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
}
/**
 * CZ_USE_ITEM
 */
void CZ_USE_ITEM::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->use_item(_inventory_index, _guid);
}

void CZ_USE_ITEM::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _guid;
}
/**
 * CZ_USE_ITEM2
 */
void CZ_USE_ITEM2::handle(ByteBuffer &&buf)
{
	deserialize(buf);
	get_session()->clif()->use_item(_inventory_index, _guid);
}

void CZ_USE_ITEM2::deserialize(ByteBuffer &buf)
{
	buf >> _packet_id;
	buf >> _inventory_index;
	buf >> _guid;
}
/**
 * CZ_SKILL_SELECT_RESPONSE
 */
void CZ_SKILL_SELECT_RESPONSE::handle(ByteBuffer &&buf) {}
void CZ_SKILL_SELECT_RESPONSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SIMPLE_BUY_CASH_POINT_ITEM
 */
void CZ_SIMPLE_BUY_CASH_POINT_ITEM::handle(ByteBuffer &&buf) {}
void CZ_SIMPLE_BUY_CASH_POINT_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLIENT_VERSION
 */
void CZ_CLIENT_VERSION::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->client_version(_version);
}
void CZ_CLIENT_VERSION::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _version;
}
/**
 * CZ_SEEK_PARTY
 */
void CZ_SEEK_PARTY::handle(ByteBuffer &&buf) {}
void CZ_SEEK_PARTY::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SEEK_PARTY_MEMBER
 */
void CZ_SEEK_PARTY_MEMBER::handle(ByteBuffer &&buf) {}
void CZ_SEEK_PARTY_MEMBER::deserialize(ByteBuffer &buf) {}
/**
 * CZ_BLOCKING_PLAY_CANCEL
 */
void CZ_BLOCKING_PLAY_CANCEL::handle(ByteBuffer &&buf) 
{
	get_session()->clif()->blocking_play_cancel();
}
void CZ_BLOCKING_PLAY_CANCEL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ES_CANCEL
 */
void CZ_ES_CANCEL::handle(ByteBuffer &&buf) {}
void CZ_ES_CANCEL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ES_CHOOSE
 */
void CZ_ES_CHOOSE::handle(ByteBuffer &&buf) {}
void CZ_ES_CHOOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ES_GET_LIST
 */
void CZ_ES_GET_LIST::handle(ByteBuffer &&buf) {}
void CZ_ES_GET_LIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_SIMPLECASH_SHOP
 */
void CZ_CLOSE_SIMPLECASH_SHOP::handle(ByteBuffer &&buf) {}
void CZ_CLOSE_SIMPLECASH_SHOP::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CHANGE_GROUP_MASTER
 */
void CZ_CHANGE_GROUP_MASTER::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().change_leader(_account_id);
}

void CZ_CHANGE_GROUP_MASTER::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_GROUPINFO_CHANGE_V2
 */
void CZ_GROUPINFO_CHANGE_V2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party().change_properties(_exp_share_rule, _item_pickup_rule, _item_share_rule);
}

void CZ_GROUPINFO_CHANGE_V2::deserialize(ByteBuffer& buf)
{
	buf >> _packet_id;
	buf >> _exp_share_rule;
	buf >> _item_pickup_rule;
	buf >> _item_share_rule;
}
/**
 * CZ_BATTLE_FIELD_LIST
 */
void CZ_BATTLE_FIELD_LIST::handle(ByteBuffer &&buf) {}
void CZ_BATTLE_FIELD_LIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CANCEL_BATTLE_FIELD
 */
void CZ_CANCEL_BATTLE_FIELD::handle(ByteBuffer &&buf) {}
void CZ_CANCEL_BATTLE_FIELD::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_BATTLE_STATE_MONITOR
 */
void CZ_REQ_BATTLE_STATE_MONITOR::handle(ByteBuffer &&buf) {}
void CZ_REQ_BATTLE_STATE_MONITOR::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GM_FULLSTRIP
 */
void CZ_GM_FULLSTRIP::handle(ByteBuffer &&buf) {}
void CZ_GM_FULLSTRIP::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PC_PURCHASE_ITEMLIST_FROMMC2
 */
void CZ_PC_PURCHASE_ITEMLIST_FROMMC2::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->vending_purchase_items(_account_id, _unique_id, _items);
}
void CZ_PC_PURCHASE_ITEMLIST_FROMMC2::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _packet_length;
	buf >> _account_id;
	buf >> _unique_id;
	for (int i = 0; i < ((_packet_length - 12) / sizeof(cz_pc_purchase_itemlist)); i++) {
		cz_pc_purchase_itemlist item;
		buf >> item.amount;
		buf >> item.item_id;
		_items.push_back(item);
	}
}
/**
 * CZ_SIMPLE_CASH_BTNSHOW
 */
void CZ_SIMPLE_CASH_BTNSHOW::handle(ByteBuffer &&buf) {}
void CZ_SIMPLE_CASH_BTNSHOW::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_BOOKING_REQ_DELETE
 */
void CZ_PARTY_BOOKING_REQ_DELETE::handle(ByteBuffer &&buf) { get_session()->clif()->party_booking().delete_(); }
void CZ_PARTY_BOOKING_REQ_DELETE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_BOOKING_REQ_SEARCH
 */
void CZ_PARTY_BOOKING_REQ_SEARCH::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->party_booking().search(_level, _map_id, _job, _last_index, _result_count);

}
void CZ_PARTY_BOOKING_REQ_SEARCH::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _level;
	buf >> _map_id;
	buf >> _job;
	buf >> _last_index;
	buf >> _result_count;
}
/**
 * CZ_PARTY_BOOKING_REQ_UPDATE
 */
void CZ_PARTY_BOOKING_REQ_UPDATE::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	std::vector<int> jobs;
	for (int i = 0; i < MAX_PARTY_BOOKING_JOBS; i++)
		jobs.push_back(_jobs[i]);
	get_session()->clif()->party_booking().update(jobs);
}
void CZ_PARTY_BOOKING_REQ_UPDATE::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	for (int i = 0; i < MAX_PARTY_BOOKING_JOBS; i++)
		buf >> _jobs[i];
}
/**
 * CZ_CLOSE_SEARCH_STORE_INFO
 */
void CZ_CLOSE_SEARCH_STORE_INFO::handle(ByteBuffer &&buf) { get_session()->clif()->searchstore_close(); }
void CZ_CLOSE_SEARCH_STORE_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RECALL_SSO
 */
void CZ_RECALL_SSO::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->recall(_account_id);
}
void CZ_RECALL_SSO::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REMOVE_AID_SSO
 */
void CZ_REMOVE_AID_SSO::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->warp_to(_account_id);
}
void CZ_REMOVE_AID_SSO::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _account_id;
}
/**
 * CZ_REQ_SE_CASH_TAB_CODE
 */
void CZ_REQ_SE_CASH_TAB_CODE::handle(ByteBuffer &&buf) {}
void CZ_REQ_SE_CASH_TAB_CODE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SE_CASHSHOP_OPEN
 */
void CZ_SE_CASHSHOP_OPEN::handle(ByteBuffer &&buf) {}
void CZ_SE_CASHSHOP_OPEN::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SE_PC_BUY_CASHITEM_LIST
 */
void CZ_SE_PC_BUY_CASHITEM_LIST::handle(ByteBuffer &&buf) {}
void CZ_SE_PC_BUY_CASHITEM_LIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SE_CASHSHOP_CLOSE
 */
void CZ_SE_CASHSHOP_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_SE_CASHSHOP_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MACRO_ITEM_PICKUP
 */
void CZ_MACRO_ITEM_PICKUP::handle(ByteBuffer &&buf) {}
void CZ_MACRO_ITEM_PICKUP::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MACRO_REQUEST_ACT
 */
void CZ_MACRO_REQUEST_ACT::handle(ByteBuffer &&buf) {}
void CZ_MACRO_REQUEST_ACT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MACRO_REQUEST_MOVE
 */
void CZ_MACRO_REQUEST_MOVE::handle(ByteBuffer &&buf) {}
void CZ_MACRO_REQUEST_MOVE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MACRO_USE_SKILL
 */
void CZ_MACRO_USE_SKILL::handle(ByteBuffer &&buf) {}
void CZ_MACRO_USE_SKILL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MACRO_USE_SKILL_TOGROUND
 */
void CZ_MACRO_USE_SKILL_TOGROUND::handle(ByteBuffer &&buf) {}
void CZ_MACRO_USE_SKILL_TOGROUND::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GPK_AUTH
 */
void CZ_GPK_AUTH::handle(ByteBuffer &&buf) {}
void CZ_GPK_AUTH::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GPK_DYNCODE_RELOAD
 */
void CZ_GPK_DYNCODE_RELOAD::handle(ByteBuffer &&buf) {}
void CZ_GPK_DYNCODE_RELOAD::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ENTRY_QUEUE_RANKING
 */
void CZ_REQ_ENTRY_QUEUE_RANKING::handle(ByteBuffer &&buf) {}
void CZ_REQ_ENTRY_QUEUE_RANKING::deserialize(ByteBuffer &buf) {}
/**
 * CZ_INVENTORY_TAB
 */
void CZ_INVENTORY_TAB::handle(ByteBuffer &&buf) {}
void CZ_INVENTORY_TAB::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_CANCEL_VOLUNTEER
 */
void CZ_PARTY_RECRUIT_CANCEL_VOLUNTEER::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_CANCEL_VOLUNTEER::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_ADD_FILTERLINGLIST
 */
void CZ_PARTY_RECRUIT_ADD_FILTERLINGLIST::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_ADD_FILTERLINGLIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_SUB_FILTERLINGLIST
 */
void CZ_PARTY_RECRUIT_SUB_FILTERLINGLIST::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_SUB_FILTERLINGLIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_REQ_DELETE
 */
void CZ_PARTY_RECRUIT_REQ_DELETE::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_REQ_DELETE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_REQ_REGISTER
 */
void CZ_PARTY_RECRUIT_REQ_REGISTER::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_REQ_REGISTER::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_REQ_SEARCH
 */
void CZ_PARTY_RECRUIT_REQ_SEARCH::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_REQ_SEARCH::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_REQ_UPDATE
 */
void CZ_PARTY_RECRUIT_REQ_UPDATE::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_REQ_UPDATE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_REQ_VOLUNTEER
 */
void CZ_PARTY_RECRUIT_REQ_VOLUNTEER::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_REQ_VOLUNTEER::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_SHOW_EQUIPMENT
 */
void CZ_PARTY_RECRUIT_SHOW_EQUIPMENT::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_SHOW_EQUIPMENT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MACRO_START
 */
void CZ_MACRO_START::handle(ByteBuffer &&buf) {}
void CZ_MACRO_START::deserialize(ByteBuffer &buf) {}
/**
 * CZ_MACRO_STOP
 */
void CZ_MACRO_STOP::handle(ByteBuffer &&buf) {}
void CZ_MACRO_STOP::deserialize(ByteBuffer &buf) {}
/**
 * CZ_TRYCOLLECTION
 */
void CZ_TRYCOLLECTION::handle(ByteBuffer &&buf) {}
void CZ_TRYCOLLECTION::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_ACK_RECALL
 */
void CZ_PARTY_RECRUIT_ACK_RECALL::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_ACK_RECALL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_PARTY_RECRUIT_REFUSE_VOLUNTEER
 */
void CZ_PARTY_RECRUIT_REFUSE_VOLUNTEER::handle(ByteBuffer &&buf) {}
void CZ_PARTY_RECRUIT_REFUSE_VOLUNTEER::deserialize(ByteBuffer &buf) {}
/**
 * CZ_UNKNOWN_BOOKING_SOMMON_MEMBER_08f5
 */
void CZ_UNKNOWN_BOOKING_SOMMON_MEMBER_08f5::handle(ByteBuffer &&buf) {}
void CZ_UNKNOWN_BOOKING_SOMMON_MEMBER_08f5::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REPLY_ENTRY_QUEUE_ADMISSION
 */
void CZ_REPLY_ENTRY_QUEUE_ADMISSION::handle(ByteBuffer &&buf) {}
void CZ_REPLY_ENTRY_QUEUE_ADMISSION::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REPLY_LOBBY_ADMISSION
 */
void CZ_REPLY_LOBBY_ADMISSION::handle(ByteBuffer &&buf) {}
void CZ_REPLY_LOBBY_ADMISSION::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ENTRY_QUEUE_APPLY
 */
void CZ_REQ_ENTRY_QUEUE_APPLY::handle(ByteBuffer &&buf) {}
void CZ_REQ_ENTRY_QUEUE_APPLY::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ENTRY_QUEUE_CANCEL
 */
void CZ_REQ_ENTRY_QUEUE_CANCEL::handle(ByteBuffer &&buf) {}
void CZ_REQ_ENTRY_QUEUE_CANCEL::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_SCHEDULER_CASHITEM
 */
void CZ_REQ_SCHEDULER_CASHITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_SCHEDULER_CASHITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_RANKING
 */
void CZ_REQ_RANKING::handle(ByteBuffer &&buf) 
{
	deserialize(buf);
	get_session()->clif()->ranking((cz_req_ranking_type) _type);
}
void CZ_REQ_RANKING::deserialize(ByteBuffer &buf) 
{
	buf >> _packet_id;
	buf >> _type;
}
/**
 * CZ_SELECTCART
 */
void CZ_SELECTCART::handle(ByteBuffer &&buf) {}
void CZ_SELECTCART::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_JOIN_GUILD2
 */
void CZ_REQ_JOIN_GUILD2::handle(ByteBuffer&& buf) 
{ 
	deserialize(buf);
	get_session()->clif()->guild().invite(_name);
}

void CZ_REQ_JOIN_GUILD2::deserialize(ByteBuffer& buf) 
{
	buf >> _packet_id;
	buf.read(_name, MAX_UNIT_NAME_LENGTH);
}
/**
 * CZ_REQ_GUILD_NAME
 */
void CZ_REQ_GUILD_NAME::handle(ByteBuffer &&buf) {}
void CZ_REQ_GUILD_NAME::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_PARTY_NAME
 */
void CZ_REQ_PARTY_NAME::handle(ByteBuffer &&buf) {}
void CZ_REQ_PARTY_NAME::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_MERGE_ITEM
 */
void CZ_REQ_MERGE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_MERGE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CANCEL_MERGE_ITEM
 */
void CZ_CANCEL_MERGE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_CANCEL_MERGE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_BEFORE_WORLD_INFO
 */
void CZ_REQ_BEFORE_WORLD_INFO::handle(ByteBuffer &&buf) {}
void CZ_REQ_BEFORE_WORLD_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ALT_EQUIPMENT_ADD
 */
void CZ_ALT_EQUIPMENT_ADD::handle(ByteBuffer &&buf) {}
void CZ_ALT_EQUIPMENT_ADD::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ALT_EQUIPMENT_REMOVE
 */
void CZ_ALT_EQUIPMENT_REMOVE::handle(ByteBuffer &&buf) {}
void CZ_ALT_EQUIPMENT_REMOVE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ALT_EQUIPMENT_SWITCH
 */
void CZ_ALT_EQUIPMENT_SWITCH::handle(ByteBuffer &&buf) {}
void CZ_ALT_EQUIPMENT_SWITCH::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CAMERA_INFO
 */
void CZ_CAMERA_INFO::handle(ByteBuffer &&buf) {}
void CZ_CAMERA_INFO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_COOLDOWN_RESET
 */
void CZ_COOLDOWN_RESET::handle(ByteBuffer &&buf) {}
void CZ_COOLDOWN_RESET::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_STYLE_CHANGE
 */
void CZ_REQ_STYLE_CHANGE::handle(ByteBuffer &&buf) {}
void CZ_REQ_STYLE_CHANGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_STYLE_CHANGE2
 */
void CZ_REQ_STYLE_CHANGE2::handle(ByteBuffer &&buf) {}
void CZ_REQ_STYLE_CHANGE2::deserialize(ByteBuffer &buf) {}
/**
 * CZ_STYLE_CLOSE
 */
void CZ_STYLE_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_STYLE_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ACK_ANSWER_MACRO_DETECTOR
 */
void CZ_ACK_ANSWER_MACRO_DETECTOR::handle(ByteBuffer &&buf) {}
void CZ_ACK_ANSWER_MACRO_DETECTOR::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ADVENTURER_AGENCY_JOIN_REQ
 */
void CZ_ADVENTURER_AGENCY_JOIN_REQ::handle(ByteBuffer &&buf) {}
void CZ_ADVENTURER_AGENCY_JOIN_REQ::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ADVENTURER_AGENCY_JOIN_RESULT
 */
void CZ_ADVENTURER_AGENCY_JOIN_RESULT::handle(ByteBuffer &&buf) {}
void CZ_ADVENTURER_AGENCY_JOIN_RESULT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_COMPLETE_APPLY_MACRO_DETECTOR_CAPTCHA
 */
void CZ_COMPLETE_APPLY_MACRO_DETECTOR_CAPTCHA::handle(ByteBuffer &&buf) {}
void CZ_COMPLETE_APPLY_MACRO_DETECTOR_CAPTCHA::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GET_ACCOUNT_LIMTIED_SALE_LIST
 */
void CZ_GET_ACCOUNT_LIMTIED_SALE_LIST::handle(ByteBuffer &&buf) {}
void CZ_GET_ACCOUNT_LIMTIED_SALE_LIST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GRADE_ENCHANT_CLOSE_UI
 */
void CZ_GRADE_ENCHANT_CLOSE_UI::handle(ByteBuffer &&buf) {}
void CZ_GRADE_ENCHANT_CLOSE_UI::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GRADE_ENCHANT_REQUEST
 */
void CZ_GRADE_ENCHANT_REQUEST::handle(ByteBuffer &&buf) {}
void CZ_GRADE_ENCHANT_REQUEST::deserialize(ByteBuffer &buf) {}
/**
 * CZ_GRADE_ENCHANT_SELECT_EQUIPMENT
 */
void CZ_GRADE_ENCHANT_SELECT_EQUIPMENT::handle(ByteBuffer &&buf) {}
void CZ_GRADE_ENCHANT_SELECT_EQUIPMENT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_NPC_EXPANDED_BARTER_MARKET_CLOSE
 */
void CZ_NPC_EXPANDED_BARTER_MARKET_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_NPC_EXPANDED_BARTER_MARKET_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_NPC_EXPANDED_BARTER_MARKET_PURCHASE
 */
void CZ_NPC_EXPANDED_BARTER_MARKET_PURCHASE::handle(ByteBuffer &&buf) {}
void CZ_NPC_EXPANDED_BARTER_MARKET_PURCHASE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RANDOM_COMBINE_ITEM_UI_CLOSE
 */
void CZ_RANDOM_COMBINE_ITEM_UI_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_RANDOM_COMBINE_ITEM_UI_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_RANDOM_UPGRADE_ITEM_UI_CLOSE
 */
void CZ_RANDOM_UPGRADE_ITEM_UI_CLOSE::handle(ByteBuffer &&buf) {}
void CZ_RANDOM_UPGRADE_ITEM_UI_CLOSE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_AGIT_INVESTMENT
 */
void CZ_REQ_AGIT_INVESTMENT::handle(ByteBuffer &&buf) {}
void CZ_REQ_AGIT_INVESTMENT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_APPLY_MACRO_DETECTOR
 */
void CZ_REQ_APPLY_MACRO_DETECTOR::handle(ByteBuffer &&buf) {}
void CZ_REQ_APPLY_MACRO_DETECTOR::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_GUILD_EMBLEM_IMG2
 */
void CZ_REQ_GUILD_EMBLEM_IMG2::handle(ByteBuffer &&buf) {}
void CZ_REQ_GUILD_EMBLEM_IMG2::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_GUILD_EMBLEM_IMG3
 */
void CZ_REQ_GUILD_EMBLEM_IMG3::handle(ByteBuffer &&buf) {}
void CZ_REQ_GUILD_EMBLEM_IMG3::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_ITEMREPAIR2
 */
void CZ_REQ_ITEMREPAIR2::handle(ByteBuffer &&buf) {}
void CZ_REQ_ITEMREPAIR2::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_MOVE_GUILD_AGIT
 */
void CZ_REQ_MOVE_GUILD_AGIT::handle(ByteBuffer &&buf) {}
void CZ_REQ_MOVE_GUILD_AGIT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_PLAYER_AID_IN_RANGE
 */
void CZ_REQ_PLAYER_AID_IN_RANGE::handle(ByteBuffer &&buf) {}
void CZ_REQ_PLAYER_AID_IN_RANGE::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_PREVIEW_MACRO_DETECTOR
 */
void CZ_REQ_PREVIEW_MACRO_DETECTOR::handle(ByteBuffer &&buf) {}
void CZ_REQ_PREVIEW_MACRO_DETECTOR::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_RANDOM_COMBINE_ITEM
 */
void CZ_REQ_RANDOM_COMBINE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_RANDOM_COMBINE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_RANDOM_UPGRADE_ITEM
 */
void CZ_REQ_RANDOM_UPGRADE_ITEM::handle(ByteBuffer &&buf) {}
void CZ_REQ_RANDOM_UPGRADE_ITEM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_UPLOAD_MACRO_DETECTOR
 */
void CZ_REQ_UPLOAD_MACRO_DETECTOR::handle(ByteBuffer &&buf) {}
void CZ_REQ_UPLOAD_MACRO_DETECTOR::deserialize(ByteBuffer &buf) {}
/**
 * CZ_SE_CASHSHOP_OPEN2
 */
void CZ_SE_CASHSHOP_OPEN2::handle(ByteBuffer &&buf) {}
void CZ_SE_CASHSHOP_OPEN2::deserialize(ByteBuffer &buf) {}
/**
 * CZ_UNINSTALLATION
 */
void CZ_UNINSTALLATION::handle(ByteBuffer &&buf) {}
void CZ_UNINSTALLATION::deserialize(ByteBuffer &buf) {}
/**
 * CZ_UPLOAD_MACRO_DETECTOR_CAPTCHA
 */
void CZ_UPLOAD_MACRO_DETECTOR_CAPTCHA::handle(ByteBuffer &&buf) {}
void CZ_UPLOAD_MACRO_DETECTOR_CAPTCHA::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_REFORM_UI
 */
void CZ_CLOSE_REFORM_UI::handle(ByteBuffer &&buf) {}
void CZ_CLOSE_REFORM_UI::deserialize(ByteBuffer &buf) {}
/**
 * CZ_ITEM_REFORM
 */
void CZ_ITEM_REFORM::handle(ByteBuffer &&buf) {}
void CZ_ITEM_REFORM::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CLOSE_UI_ENCHANT
 */
void CZ_CLOSE_UI_ENCHANT::handle(ByteBuffer &&buf) {}
void CZ_CLOSE_UI_ENCHANT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQUEST_PERFECT_ENCHANT
 */
void CZ_REQUEST_PERFECT_ENCHANT::handle(ByteBuffer &&buf) {}
void CZ_REQUEST_PERFECT_ENCHANT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQUEST_RANDOM_ENCHANT
 */
void CZ_REQUEST_RANDOM_ENCHANT::handle(ByteBuffer &&buf) {}
void CZ_REQUEST_RANDOM_ENCHANT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQUEST_RESET_ENCHANT
 */
void CZ_REQUEST_RESET_ENCHANT::handle(ByteBuffer &&buf) {}
void CZ_REQUEST_RESET_ENCHANT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQUEST_UPGRADE_ENCHANT
 */
void CZ_REQUEST_UPGRADE_ENCHANT::handle(ByteBuffer &&buf) {}
void CZ_REQUEST_UPGRADE_ENCHANT::deserialize(ByteBuffer &buf) {}
/**
 * CZ_CHOOSE_MENU_ZERO
 */
void CZ_CHOOSE_MENU_ZERO::handle(ByteBuffer &&buf) {}
void CZ_CHOOSE_MENU_ZERO::deserialize(ByteBuffer &buf) {}
/**
 * CZ_REQ_TAKEOFF_EQUIP_ALL
 */
void CZ_REQ_TAKEOFF_EQUIP_ALL::handle(ByteBuffer &&buf) {}
void CZ_REQ_TAKEOFF_EQUIP_ALL::deserialize(ByteBuffer &buf) {}
