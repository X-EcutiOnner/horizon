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

#include "ZoneClientInterface.hpp"

#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"

#include "Server/Zone/Game/Units/Battle/Combat.hpp"
#include "Server/Zone/Game/Units/Player/Assets/Inventory.hpp"
#include "Server/Zone/Game/Units/Player/Assets/Storage.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Game/Units/Item/Item.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/StaticDB/SkillDB.hpp"
#include "Server/Zone/Game/SkillSystem/SkillExecution.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/SocketMgr/ClientSocketMgr.hpp"
#include "Server/Common/System.hpp"
#include "Server/Common/Server.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include "Server/Zone/ZoneSystem.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

ZoneClientInterface::ZoneClientInterface(std::shared_ptr<ZoneSession> s)
: ClientInterface(s), _chat_room(s), _trade(s), _party(s), _guild(s), _friend(s), _quest(s), 
  _auction(s), _mail(s), _clan(s), _party_booking(s)
{
	
}

ZoneClientInterface::~ZoneClientInterface()
{
}

bool ZoneClientInterface::login(uint32_t account_id, uint32_t char_id, uint32_t auth_code, uint32_t client_time, uint8_t gender)
{	
	std::shared_ptr<Horizon::System::RuntimeContextChain> chain = std::make_shared<Horizon::System::RuntimeContextChain>(Horizon::System::RUNTIME_RUNTIME);
	std::shared_ptr<Horizon::Zone::SCENARIO_LOGIN> s_login = std::make_shared<Horizon::Zone::SCENARIO_LOGIN>(sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager());
	std::shared_ptr<Horizon::Zone::SCENARIO_LOGIN::Login> w_login = std::make_shared<Horizon::Zone::SCENARIO_LOGIN::Login>(s_login);

	Horizon::Zone::s_scenario_login_request w_l_request;
	w_l_request.account_id = account_id;
	w_l_request.char_id = char_id;
	w_l_request.auth_code = auth_code;
	w_l_request.client_time = client_time;
	w_l_request.gender = gender;

	s_login->get_runtime_synchronization_mutex().lock();
	s_login->set_session(get_session());
	w_login->set_request(w_l_request);
	s_login->get_runtime_synchronization_mutex().unlock();
	s_login->push(w_login);

	chain->push(s_login);

	sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_resource_manager().add<RESOURCE_PRIORITY_PRIMARY>(get_session()->get_session_id(), get_session());

	sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager().push(chain);

	while(s_login->get_context_result() != Horizon::System::RUNTIME_CONTEXT_PASS)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	};

	std::shared_ptr<Horizon::System::RuntimeContextChain> chain_2 = std::make_shared<Horizon::System::RuntimeContextChain>(Horizon::System::RUNTIME_RUNTIME);

	std::shared_ptr<Horizon::Zone::SCENARIO_CREATE_PLAYER> s_create_player;
	s_create_player = std::make_shared<Horizon::Zone::SCENARIO_CREATE_PLAYER>(sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, 1)->get_system_routine_manager());

	std::shared_ptr<Horizon::Zone::SCENARIO_CREATE_PLAYER::CreatePlayer> w_create_player = std::make_shared<Horizon::Zone::SCENARIO_CREATE_PLAYER::CreatePlayer>(s_create_player);
	s_create_player->get_runtime_synchronization_mutex().lock();
	w_create_player->set_request(Horizon::Zone::s_player_loaded_data{(int32_t)account_id});
	s_create_player->set_previous_context_result(s_login->get_result());
	s_create_player->set_session(get_session());
	s_create_player->get_runtime_synchronization_mutex().unlock();
	s_create_player->push(w_create_player);

	std::shared_ptr<Horizon::Zone::SCENARIO_LOGIN_RESPONSE> s_login_response;
	s_login_response = std::make_shared<Horizon::Zone::SCENARIO_LOGIN_RESPONSE>(sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager());
	s_login_response->get_runtime_synchronization_mutex().lock();
	s_login_response->set_session(get_session());
	s_login_response->get_runtime_synchronization_mutex().unlock();

	std::shared_ptr<Horizon::Zone::SCENARIO_LOGIN_RESPONSE::LoginResponse> w_login_response = std::make_shared<Horizon::Zone::SCENARIO_LOGIN_RESPONSE::LoginResponse>(s_login_response);
	Horizon::Zone::SCENARIO_LOGIN_RESPONSE::s_scenario_login_response_request w_l_r_request;

	w_l_r_request.account_id = account_id;
	w_l_r_request.current_x = s_login->get_result().get_one().current_x;
	w_l_r_request.current_y = s_login->get_result().get_one().current_y;
	w_l_r_request.font = 1;

	w_login_response->set_request(w_l_r_request);
	s_login_response->push(w_login_response);

	chain_2->push(s_create_player);
	chain_2->push(s_login_response);

	sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager().push(chain_2);

	while(s_login_response->get_context_result() != Horizon::System::RUNTIME_CONTEXT_PASS) {
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	get_session()->player()->initialize();
	return true;
}

/**
 * All cleanups are managed in ZoneSocket::perform_cleanup
 */
bool ZoneClientInterface::restart(uint8_t type)
{
	std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK> s_task = std::make_shared<Horizon::Zone::SCENARIO_GENERIC_TASK>(sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager());
	std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask> w_task = std::make_shared<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask>(s_task);
	s_task->get_runtime_synchronization_mutex().lock();
	s_task->set_session(get_session());
	w_task->set_task([type](std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask> task) {
		std::shared_ptr<ZoneSession> session = std::dynamic_pointer_cast<Horizon::Zone::ActiveRuntimeScenario>(task->get_runtime_context())->get_session();
		ZC_RESTART_ACK rpkt(session);
		switch (type) {
			case 0:
				task->get_message_agent().set_status_message("Character (GUID:" + std::to_string(session->player()->guid()) + ") is being respawned.");
				break;
			default:
				task->get_message_agent().set_status_message("Character (GUID:" + std::to_string(session->player()->guid()) + ") has moved to the character server.");
				break;
		};
		rpkt.deliver(type);
	});
	s_task->get_runtime_synchronization_mutex().unlock();
	s_task->push(w_task);
	sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager().push(s_task);

	return true;
}

/**
 * Character
 */
void ZoneClientInterface::request_name_by_char_id(int char_id)
{

}
void ZoneClientInterface::invite_baby(int account_id)
{

}
void ZoneClientInterface::pvpinfo(int character_id, int account_id)
{

}
bool ZoneClientInterface::disconnect(int8_t type)
{
	std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK> s_task = std::make_shared<Horizon::Zone::SCENARIO_GENERIC_TASK>(sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager());
	std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask> w_task = std::make_shared<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask>(s_task);
	s_task->get_runtime_synchronization_mutex().lock();
	s_task->set_session(get_session());
	w_task->set_task([type](std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask> task) {
		std::shared_ptr<ZoneSession> session = std::dynamic_pointer_cast<Horizon::Zone::ActiveRuntimeScenario>(task->get_runtime_context())->get_session();
		ZC_ACK_REQ_DISCONNECT rpkt(session);
		rpkt.deliver(type);
	});
	s_task->get_runtime_synchronization_mutex().unlock();
	s_task->push(w_task);
	sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager().push(s_task);

	//sZone->get_component_of_type<GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC)->get_map_process().manage_session_in_map(SESSION_ACTION_LOGOUT_AND_REMOVE, get_session()->get_map_name(), get_session());
	return true;
}
bool ZoneClientInterface::update_session(int32_t account_id)
{
	std::shared_ptr<boost::mysql::tcp_ssl_connection> conn = sZone->get_database_connection();

	boost::mysql::statement stmt = conn->prepare_statement("SELECT `current_server` FROM `session_data` WHERE `id` = ?");
	auto b1 = stmt.bind(account_id);
	boost::mysql::results results;
	conn->execute(b1, results);
	
	HLog(debug) << "Updating session from I.P. address " << get_session()->get_socket()->remote_ip_address();

	if (results.rows().empty()) {
		ZC_ACK_REQ_DISCONNECT pkt(get_session());
		HLog(warning) << "Invalid connection for account with ID " << account_id << ", session wasn't found.";
		pkt.deliver(0);
		return false;
	}

	auto r = results.rows()[0];

	std::string current_server = r[0].as_string();

	if (current_server.compare("C") != 0) {
		ZC_ACK_REQ_DISCONNECT pkt(get_session());
		HLog(warning) << "Invalid connection for account with ID " << account_id << ", session wasn't found.";
		pkt.deliver(0);
		return false;
	}
	
	stmt = conn->prepare_statement("UPDATE `session_data` SET `last_update` = ? WHERE `game_account_id` = ?");
	auto b2 = stmt.bind(account_id, std::time(nullptr));
	conn->execute(b2, results);

	return true;
}
bool ZoneClientInterface::walk_to_coordinates(uint16_t x, uint16_t y, uint8_t dir)
{
	std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK> s_task = std::make_shared<Horizon::Zone::SCENARIO_GENERIC_TASK>(sZone->get_component_of_type<Horizon::Zone::ZoneRuntime>(Horizon::System::RUNTIME_RUNTIME)->get_system_routine_manager());
	std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask> w_task = std::make_shared<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask>(s_task);
	s_task->get_runtime_synchronization_mutex().lock();
	s_task->set_session(get_session());
	w_task->set_task([x, y](std::shared_ptr<Horizon::Zone::SCENARIO_GENERIC_TASK::GenericTask> task) {
		std::shared_ptr<ZoneSession> session = std::dynamic_pointer_cast<Horizon::Zone::ActiveRuntimeScenario>(task->get_runtime_context())->get_session();

		if (session->player() == nullptr)
			return;

		session->player()->walk_to_coordinates(x, y);
	});
	s_task->push(w_task);
	int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_TERTIARY, uint64_t, std::shared_ptr<Horizon::Zone::Unit>>(Horizon::System::RUNTIME_GAMELOGIC, get_session()->player()->uuid(), nullptr);
	
	s_task->get_runtime_synchronization_mutex().unlock();

	if (segment_number == 0) {
		HLog(warning) << "ZoneClientInterface::walk_to_coordinates:Player " << get_session()->player()->guid() << " is not in a map.";
		return false;
	}

	sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number)->get_system_routine_manager().push(s_task);


	return true;
}
bool ZoneClientInterface::notify_time()
{
	ZC_NOTIFY_TIME pkt(get_session());
	pkt.deliver();
	return true;
}


bool ZoneClientInterface::notify_unit_name(uint32_t guid)
{
	std::shared_ptr<Unit> unit = get_session()->player()->get_nearby_unit(guid);
	
	if (unit == nullptr)
		return false;

#if (CLIENT_TYPE == 'M' && PACKET_VERSION >= 20150225) \
	|| (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20141126) \
	|| (CLIENT_TYPE == 'Z')
	ZC_ACK_REQNAMEALL2 req(get_session());
	req.deliver(guid, std::string(unit->name()) , "", "", "", 0);
#else
	ZC_ACK_REQNAMEALL req(get_session());
	req.deliver(guid, std::string(unit->name()), "", "", "");
#endif
	
	return true;
}
bool ZoneClientInterface::stop_attack()
{
	get_session()->player()->stop_attack();
	return true;
}
item_viewport_entry ZoneClientInterface::create_viewport_item_entry(std::shared_ptr<Units::Item> item)
{
	item_viewport_entry entry;

	if (item == nullptr)
		return entry;
	
	entry._guid = item->guid();
	entry.item_id = item->config()->item_id;
	entry.x = item->map_coords().x();
	entry.y = item->map_coords().y();
	entry.amount = item->amount();
	entry.is_identified = item->is_identified();
	entry.x_area = item->x_area();
	entry.y_area = item->y_area();

	return entry;
}
unit_viewport_entry ZoneClientInterface::create_viewport_entry(std::shared_ptr<Unit> unit)
{
	unit_viewport_entry entry;

	if (unit == nullptr)
		return entry;
	
	std::shared_ptr<Horizon::Zone::Traits::Status> status = unit->status();

	if (status == nullptr)
		return entry;

	entry._guid = unit->guid();
	entry._unit_type = unit->type();
	entry.speed = status->movement_speed()->total();
	entry.body_state = 0;
	entry.health_state = 0;
	entry.effect_state = 0;
	entry.job_id = unit->job_id();
	entry.hair_style_id = status->hair_style()->get();
	entry.hair_color_id = status->hair_color()->get();
	entry.robe_id = status->robe_sprite()->get();
	entry.guild_id = 0;
	entry.guild_emblem_version = 0;
	entry.honor = unit->type() == UNIT_PLAYER ? status->honor()->total() : 0;
	entry.virtue = unit->type() == UNIT_PLAYER ? status->virtue()->total() : 0;
	entry.in_pk_mode = 0;
	entry.current_x = unit->map_coords().x();
	entry.current_y = unit->map_coords().y();
	entry.current_dir = unit->direction();
	
	if (unit->is_walking()) {
		entry.to_x = unit->dest_coords().x();
		entry.to_y = unit->dest_coords().y();
	}
	
	entry.posture = unit->posture();
	entry.base_level = status->base_level()->total();
	entry.font = 1;

	if (status->current_hp()->total() < status->max_hp()->total()) {
		entry.max_hp = status->max_hp()->total();
		entry.hp = status->current_hp()->total();
	}
	else {
		entry.max_hp = -1;
		entry.hp = -1;
	}
	
	entry.is_boss = 0;
	entry.body_style_id = 0;
	std::strncpy(entry.name, unit->name().c_str(), unit->name().size());
	
	switch (entry._unit_type)
	{
		case UNIT_PLAYER:
			entry.character_id = unit->downcast<Horizon::Zone::Units::Player>()->character()._character_id;
			entry.x_size = entry.y_size = 0;
			entry.gender = unit->downcast<Horizon::Zone::Units::Player>()->character()._gender;
			break;
		case UNIT_NPC:
		default:
			entry.x_size = entry.y_size = 0;
			break;
	}
	
	return entry;
}
bool ZoneClientInterface::notify_player_movement(MapCoords from, MapCoords to)
{
	ZC_NOTIFY_PLAYERMOVE pkt(get_session());
	pkt.deliver(from.x(), from.y(), to.x(), to.y());
	return true;
}
bool ZoneClientInterface::notify_movement_stop(int32_t guid, int16_t x, int16_t y)
{
	ZC_STOPMOVE pkt(get_session());
	pkt.deliver(guid, x, y);
	return true;
}
bool ZoneClientInterface::notify_viewport_add_unit(unit_viewport_entry entry)
{
#if PACKET_VERSION >= 20150513
	ZC_NOTIFY_STANDENTRY11 pkt(get_session());
	pkt.deliver(entry);
#endif
	return true;
}
bool ZoneClientInterface::notify_viewport_spawn_unit(unit_viewport_entry entry)
{
	ZC_NOTIFY_NEWENTRY11 pkt(get_session());
	pkt.deliver(entry);
	return true;
}
bool ZoneClientInterface::notify_viewport_moving_unit(unit_viewport_entry entry)
{
#if PACKET_VERSION >= 20150513
	ZC_NOTIFY_MOVEENTRY11 pkt(get_session());
	pkt.deliver(entry);
#endif
	return true;
}
bool ZoneClientInterface::notify_viewport_item_entry(item_viewport_entry entry)
{
	ZC_ITEM_ENTRY pkt(get_session());
	pkt.deliver(entry);
	return true;
}
bool ZoneClientInterface::notify_unit_move(int32_t guid, MapCoords from, MapCoords to)
{
	ZC_NOTIFY_MOVE pkt(get_session());
	pkt.deliver(guid, from.x(), from.y(), to.x(), to.y());
	return true;
}
bool ZoneClientInterface::notify_viewport_remove_unit(std::shared_ptr<Unit> unit, unit_viewport_notification_type type)
{
	if (unit == nullptr)
		 return false;

	ZC_NOTIFY_VANISH pkt(get_session());
	pkt.deliver(unit->guid(), type);
	return true;
}
bool ZoneClientInterface::notify_initial_status()
{
	if (get_session()->player() == nullptr)
		return false;

	std::shared_ptr<Horizon::Zone::Traits::Status> status = get_session()->player()->status();

	if (status == nullptr)
		return false;

	ZC_STATUS zcs(get_session());
	zc_status_data data;
	
	data.status_points = status->status_point()->get_base();
	data.strength = status->strength()->get_base();
	data.strength_req_stats = status->strength_cost()->get_base();
	data.agility = status->agility()->get_base();
	data.agility_req_stats = status->agility_cost()->get_base();
	data.vitality = status->vitality()->get_base();
	data.vitality_req_stats = status->vitality_cost()->get_base();
	data.intelligence = status->intelligence()->get_base();
	data.intelligence_req_stats = status->intelligence_cost()->get_base();
	data.dexterity = status->dexterity()->get_base();
	data.dexterity_req_stats = status->dexterity_cost()->get_base();
	data.luck = status->luck()->get_base();
	data.luck_req_stats = status->luck_cost()->get_base();
	data.status_atk = status->status_atk()->total();
	data.equip_atk = status->equip_atk()->total();
	data.status_matk = status->status_matk()->total();
	data.equip_matk = 0;
	data.soft_def = status->soft_def()->total();
	data.hard_def = 0;
	data.soft_mdef = status->soft_mdef()->total();
	data.hard_mdef = 0;
	data.hit = status->hit()->total();
	data.flee = status->flee()->total();
	data.perfect_dodge = 0;
	data.critical = status->crit()->total() / 10;
	data.attack_speed = status->attack_speed()->total();
	data.plus_aspd = 0;
	
	zcs.deliver(data);

	return true;
}
bool ZoneClientInterface::notify_appearance_update(unit_appearance_type type, int32_t value, int32_t value2)
{
	ZC_SPRITE_CHANGE2 pkt(get_session());
	pkt.deliver(get_session()->player()->guid(), type, value, value2);
	return true;
}

// 0x00b0
bool ZoneClientInterface::notify_compound_attribute_update(status_point_type type, int32_t value)
{
	ZC_PAR_CHANGE pkt(get_session());
	pkt.deliver(type, value);
	return true;
}

// 0x00bc
bool ZoneClientInterface::notify_status_attribute_update(status_point_type type, int32_t value, bool success)
{
	ZC_STATUS_CHANGE_ACK pkt(get_session());
	pkt.deliver(type, value, success);
	return true;
}

// 0x00be
bool ZoneClientInterface::notify_required_attribute_update(status_point_type type, int32_t value)
{
	ZC_STATUS_CHANGE pkt(get_session());
	pkt.deliver(type, value);
	return true;
}

// 0x0121
bool ZoneClientInterface::notify_cart_weight_update()
{
	ZC_NOTIFY_CARTITEM_COUNTINFO pkt(get_session());
	//pkt.deliver(get_session()->player()->cart()->get_item_count(), MAX_CART_ITEMS, get_session()->player()->cart()->weight(), get_session()->player()->cart()->max_weight());
	return true;
}

// 0x0141
bool ZoneClientInterface::notify_attribute_update(status_point_type type, int32_t value, int32_t value2)
{
	ZC_COUPLESTATUS pkt(get_session());
	pkt.deliver(type, value, value2);
	return true;
}

// 0x013a
bool ZoneClientInterface::notify_attack_range_update(int32_t value)
{
	ZC_ATTACK_RANGE pkt(get_session());
	pkt.deliver(value);
	return true;

}

// 0x0acb
bool ZoneClientInterface::notify_experience_update(status_point_type type, int32_t value)
{

#if CLIENT_TYPE == 'Z' || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION >= 20170906) || \
	(CLIENT_TYPE == 'R' && PACKET_VERSION >= 20170830)
	ZC_LONGLONGPAR_CHANGE pkt(get_session());
#else
	ZC_LONGPAR_CHANGE pkt(get_session());
#endif
	pkt.deliver(type, value);
	return true;
}

// 0x00b1
bool ZoneClientInterface::notify_zeny_update()
{
	return true;
}
bool ZoneClientInterface::increase_status_point(status_point_type type, uint8_t amount)
{
	std::shared_ptr<Horizon::Zone::Units::Player> pl = get_session()->player();

	if (pl == nullptr)
		return false;

	pl->status()->increase_status_point(type, amount);
	return true;
}
void ZoneClientInterface::register_baby(int account_id, int character_id, cz_join_baby_reply_type response)
{

}
void ZoneClientInterface::npc_weapon_refine(int32_t inventory_index)
{

}
void ZoneClientInterface::npc_next_dialog(int32_t npc_guid)
{

}
void ZoneClientInterface::npc_contact(int32_t guid)
{

}
void ZoneClientInterface::npc_close(int32_t npc_guid)
{

}
void ZoneClientInterface::notify_npc_dialog(uint32_t npc_guid, std::string dialog)
{
	uint32_t current_npc_guid = get_session()->player()->npc_contact_guid();
	
}
void ZoneClientInterface::notify_npc_next_dialog(uint32_t npc_guid)
{
	ZC_WAIT_DIALOG pkt(get_session());
	pkt.deliver(npc_guid);
}
void ZoneClientInterface::notify_npc_close_dialog(uint32_t npc_guid)
{
	ZC_CLOSE_DIALOG pkt(get_session());
	pkt.deliver(npc_guid);
}
void ZoneClientInterface::notify_npc_menu_list(uint32_t npc_guid, std::string const &menu)
{
	ZC_MENU_LIST pkt(get_session());
	pkt.deliver(npc_guid, menu);
}
void ZoneClientInterface::npc_select_deal_type(int guid, cz_ack_select_dealtype deal_type)
{

}
void ZoneClientInterface::npc_select_menu(int guid, int choice)
{

}
void ZoneClientInterface::npc_input(int guid, int value)
{

}
void ZoneClientInterface::npc_input(int guid, std::string value)
{

}
void ZoneClientInterface::npc_purchase_items(std::vector<cz_pc_purchase_itemlist> items)
{

}
void ZoneClientInterface::npc_sell_items(std::vector<cz_pc_sell_itemlist> items)
{

}
void ZoneClientInterface::progress_bar_completed()
{

}
void ZoneClientInterface::vending_purchase_items(int account_id, std::vector<cz_pc_purchase_itemlist> items)
{

}
void ZoneClientInterface::vending_purchase_items(int account_id, int unique_id, std::vector<cz_pc_purchase_itemlist> items)
{

}
void ZoneClientInterface::broadcast(std::string message)
{

}
void ZoneClientInterface::broadcast_local(std::string message)
{

}

/**
 * Storage
 */
void ZoneClientInterface::storage_check_password(std::string password)
{

}
void ZoneClientInterface::storage_change_password(std::string password, std::string new_password)
{

}
bool ZoneClientInterface::notify_move_to_map(std::string map_name, int16_t x, int16_t y)
{
	ZC_NPCACK_MAPMOVE pkt(get_session());
	pkt.deliver(map_name, x, y);
	return true;
}
bool ZoneClientInterface::notify_chat(std::string message)
{
	ZC_NOTIFY_PLAYERCHAT pkt(get_session());
	pkt.deliver(message);
	return true;
}
void ZoneClientInterface::parse_chat_message(std::string message)
{
	ZC_NOTIFY_PLAYERCHAT notify_player_chat(get_session());
	ZC_NOTIFY_CHAT notify_chat(get_session());

	HLog(debug) << get_session()->player()->name() << ": " << message;

	int guid = get_session()->player()->guid();
	int msg_first_char = get_session()->player()->name().size() + 3;

	// @TODO @commands
	if (message[msg_first_char] == '@') {
		sZone->get_component_of_type<Horizon::Zone::ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM)->player()->perform_command_from_player(get_session()->player(), &message[msg_first_char + 1]);
		return;
	}

	ByteBuffer buf;

	notify_chat._packet_length = 8 + message.size();
	notify_chat._guid = guid;
	notify_chat._message = message;
	buf = notify_chat.serialize();
	get_session()->player()->notify_in_area(buf, GRID_NOTIFY_AREA_WOS);
	notify_player_chat.deliver(message);
}
void ZoneClientInterface::map_enter()
{
	get_session()->player()->on_map_enter();
}
bool ZoneClientInterface::notify_map_properties(zc_map_properties p)
{
	ZC_MAPPROPERTY_R2 pkt(get_session());
	pkt.deliver(p);
	return true;
}
void ZoneClientInterface::whisper_message(const char *name, int32_t name_length, const char *message, int32_t message_length)
{
	// validate name
	// only restriction is that the name must be zero-terminated
	if (name[name_length] != '\0') {
		HLog(warning) << "ZoneClientInterface::whisper_message: Player '" << get_session()->player()->name() << "' sent an unterminated name!\n";
		return;
	}
	// validate name
	// only restriction is that the name must be zero-terminated
	if (message[message_length] != '\0') {
		HLog(warning) << "ZoneClientInterface::whisper_message: Player '" << get_session()->player()->name() << "' sent an unterminated message!\n";
		return;
	}

	HLog(debug) << name << " : " << message;

	// @TODO
	//std::shared_ptr<Horizon::Zone::Units::Player> player = sZone->get_component_of_type<GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC)->find_player(name);

	//ZC_ACK_WHISPER02 pkt(get_session());
	//if (player != nullptr)
	//	pkt.deliver(WRT_SUCCESS, player->character()._character_id);
	//else
	//	pkt.deliver(WRT_RECIPIENT_OFFLINE, 0);
//
	//ZC_WHISPER pkt2(player->get_session());
	//pkt2.deliver(get_session()->player()->name(), message, player->account()._group_id >= 99 ? true : false);
}
void ZoneClientInterface::use_item(int16_t inventory_index, int32_t guid)
{
	get_session()->player()->inventory()->use_item(inventory_index, guid);
}
void ZoneClientInterface::equip_item(int16_t inventory_index, int16_t equip_location_mask)
{
	get_session()->player()->inventory()->equip_item(inventory_index, equip_location_mask);
}
void ZoneClientInterface::unequip_item(int16_t inventory_index)
{
	get_session()->player()->inventory()->unequip_item(inventory_index);
}
void ZoneClientInterface::pickup_item(int guid)
{
	get_session()->player()->pickup_item(guid);
}
void ZoneClientInterface::throw_item(int16_t inventory_index, int16_t amount)
{
	std::shared_ptr<item_entry_data> item = get_session()->player()->inventory()->get_item(inventory_index);

	if (item == nullptr)
		return;

	if (item->amount < amount)
		return;
	
	get_session()->player()->throw_item(item, amount);
}
void ZoneClientInterface::move_item_from_inventory_to_cart(int16_t inventory_index, int amount)
{

}
void ZoneClientInterface::move_item_from_inventory_to_storage(int16_t inventory_index, int amount)
{
	int32_t current_storage_id = get_session()->player()->get_current_storage_id();
	std::shared_ptr<Assets::Storage> storage = get_session()->player()->get_storage(current_storage_id);
	storage->add_item_from_inventory(inventory_index, amount);
}
void ZoneClientInterface::move_item_from_cart_to_inventory(int16_t inventory_index, int amount)
{

}
void ZoneClientInterface::move_item_from_cart_to_storage(int16_t inventory_index, int amount)
{

}
void ZoneClientInterface::move_item_from_storage_to_inventory(int16_t storage_index, int amount)
{
	int32_t current_storage_id = get_session()->player()->get_current_storage_id();
	std::shared_ptr<Assets::Storage> storage = get_session()->player()->get_storage(current_storage_id);
	storage->add_item_to_inventory(storage_index, amount);
}
void ZoneClientInterface::move_item_from_storage_to_cart(int16_t inventory_index, int amount)
{

}
void ZoneClientInterface::display_item_card_composition(int card_index, int equip_index)
{

}
void ZoneClientInterface::identify_item(int inventory_index)
{
	
}
void ZoneClientInterface::repair_item(int inventory_index, int item_id, int refine, int card1, int card2, int card3, int card4)
{

}
bool ZoneClientInterface::notify_pickup_item(std::shared_ptr<item_entry_data> item, int16_t amount, item_inventory_addition_notif_type result)
{
	ZC_ITEM_PICKUP_ACK_V7 pkt(get_session());
	pkt.deliver(*item, amount, result);
	return true;
}
bool ZoneClientInterface::notify_normal_item_list(std::vector<std::shared_ptr<const item_entry_data>> const &items)
{
#if (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180829) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180919) || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION >= 20181002)
	ZC_STORE_ITEMLIST_NORMAL_V6 pkt(get_session());
	pkt.deliver(INVTYPE_INVENTORY, items);
#else
	ZC_INVENTORY_ITEMLIST_NORMAL_V5 pkt(get_session());
	pkt.deliver(items);
#endif
	return true;
}
bool ZoneClientInterface::notify_equipment_item_list(std::vector<std::shared_ptr<const item_entry_data>> const &items)
{
#if (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180829) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180919) || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION >= 20181002)
	ZC_STORE_ITEMLIST_EQUIP_V6 pkt(get_session());
	pkt.deliver(INVTYPE_INVENTORY, items);
#else
	ZC_INVENTORY_ITEMLIST_EQUIP_V6 pkt(get_session());
	pkt.deliver(items);
#endif
	return true;
}
bool ZoneClientInterface::notify_throw_item(int16_t inventory_index, int16_t amount)
{
	ZC_ITEM_THROW_ACK pkt(get_session());
	pkt.deliver(inventory_index, amount);
	return true;
}
bool ZoneClientInterface::notify_inventory_move_failed(int16_t inventory_index, bool silent)
{
	ZC_INVENTORY_MOVE_FAILED pkt(get_session());
	pkt.deliver(inventory_index, silent);
	return true;
}
bool ZoneClientInterface::notify_delete_item(int16_t inventory_index, int16_t amount, item_deletion_reason_type reason)
{
	ZC_DELETE_ITEM_FROM_BODY pkt(get_session());
	pkt.deliver(inventory_index, amount, reason);
	return true;
}
bool ZoneClientInterface::notify_bind_on_equip(int16_t inventory_index)
{
	ZC_NOTIFY_BIND_ON_EQUIP pkt(get_session());
	pkt.deliver(inventory_index);
	return true;
}
bool ZoneClientInterface::notify_use_item(std::shared_ptr<item_entry_data> inv_item, bool success)
{
	ZC_USE_ITEM_ACK pkt(get_session());
	pkt.deliver(inv_item->index.inventory, inv_item->amount, success);
	return true;
}
bool ZoneClientInterface::notify_equip_item(std::shared_ptr<const item_entry_data> item, item_equip_result_type result)
{
	ZC_ACK_WEAR_EQUIP_V5 pkt(get_session());
	pkt.deliver(item->index.inventory, item->current_equip_location_mask, item->sprite_id, result);
	return true;
}
bool ZoneClientInterface::notify_unequip_item(std::shared_ptr<const item_entry_data> item, item_unequip_result_type result)
{
	ZC_ACK_TAKEOFF_EQUIP_V5 pkt(get_session());
	pkt.deliver(item->index.inventory, item->current_equip_location_mask, result);
	return true;
}
bool ZoneClientInterface::notify_equip_arrow(std::shared_ptr<const item_entry_data> item)
{
	ZC_EQUIP_ARROW pkt(get_session());
	pkt.deliver(item->index.inventory);
	return true;
}
bool ZoneClientInterface::notify_action_failure(int16_t message_type)
{
	ZC_ACTION_FAILURE pkt(get_session());
	pkt.deliver(message_type);
	return true;
}
bool ZoneClientInterface::notify_item_composition(int16_t item_inventory_index, int16_t card_inventory_index, zc_ack_item_composition_result_type result)
{
	ZC_ACK_ITEMCOMPOSITION pkt(get_session());
	pkt.deliver(item_inventory_index, card_inventory_index, result);
	return true;
}
bool ZoneClientInterface::notify_identified_item(int16_t inventory_index, zc_ack_item_identify_result_type result)
{
	ZC_ACK_ITEMIDENTIFY pkt(get_session());
	pkt.deliver(inventory_index, result);
	return true;
}
bool ZoneClientInterface::notify_item_refining(zc_ack_itemrefining_result_type result, int16_t inventory_index, int16_t refine_lv)
{
	ZC_ACK_ITEMREFINING pkt(get_session());
	pkt.deliver(result, inventory_index, refine_lv);
	return true;
}
bool ZoneClientInterface::notify_item_repair(int inventory_index, zc_ack_itemrepair_result_type result)
{
	ZC_ACK_ITEMREPAIR pkt(get_session());
	pkt.deliver(inventory_index, result);
	return true;
}
bool ZoneClientInterface::notify_item_merge(int inventory_index, int amount, zc_ack_merge_item_reason_type reason)
{
	ZC_ACK_MERGE_ITEM pkt(get_session());
	pkt.deliver(inventory_index, amount, reason);
	return true;
}
bool ZoneClientInterface::notify_item_drop(int guid, int item_id, int type, int identified, int x, int y, int x_area, int y_area, int amount, int show_drop_effect, int drop_effect_mode)
{
	ZC_ITEM_FALL_ENTRY pkt(get_session());
	pkt.deliver(guid, item_id, type, identified, x, y, x_area, y_area, amount, show_drop_effect, drop_effect_mode);
	return true;
}
bool ZoneClientInterface::notify_item_removal_from_floor(int32_t guid)
{
	ZC_ITEM_DISAPPEAR pkt(get_session());
	pkt.deliver(guid);
	return true;
}

bool ZoneClientInterface::notify_inventory_start(inventory_type type, std::string name)
{
#if (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180829) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180919) || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION >= 20181002)
	ZC_INVENTORY_START pkt(get_session());
	pkt.deliver(type, name);
#endif
	return true;
}
bool ZoneClientInterface::notify_inventory_end(inventory_type type)
{
#if (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180829) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180919) || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION >= 20181002)
	ZC_INVENTORY_END pkt(get_session());
	pkt.deliver(type);
#endif
	return true;
}

void ZoneClientInterface::storage_close()
{
	notify_storage_close();
	get_session()->player()->set_current_storage_id(0);
}
bool ZoneClientInterface::notify_storage_size(int16_t total_size, int16_t max_size)
{
	ZC_NOTIFY_STOREITEM_COUNTINFO pkt(get_session());
	pkt.deliver(total_size, max_size);
	return true;
}
bool ZoneClientInterface::notify_storage_close()
{
	ZC_CLOSE_STORE pkt(get_session());
	pkt.deliver();
	return true;
}
bool ZoneClientInterface::notify_storage_add_item(std::shared_ptr<const item_entry_data> entry, int amount)
{
	ZC_ADD_ITEM_TO_STORE3 pkt(get_session());
	pkt.deliver(entry, amount);
	return true;
}
bool ZoneClientInterface::notify_storage_remove_item(int16_t storage_index, int amount)
{
	ZC_DELETE_ITEM_FROM_STORE pkt(get_session());
	pkt.deliver(storage_index, amount);
	return true;
}
bool ZoneClientInterface::notify_storage_normal_items(std::string name, std::vector<std::shared_ptr<const item_entry_data>> const &items)
{
#if (PACKET_VERSION >= 20120925 && \
	((CLIENT_TYPE == 'R' && PACKET_VERSION < 20180829) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION < 20180919) || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION < 20181002)))
	ZC_STORE_ITEMLIST_NORMAL_V5 pkt(get_session());
	pkt.deliver(name, items);
#elif (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20180829) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20180919) || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION >= 20181002)
	ZC_STORE_ITEMLIST_NORMAL_V6 pkt(get_session());
	pkt.deliver(INVTYPE_STORAGE, items);
#endif
	return true;
}
bool ZoneClientInterface::notify_storage_equip_items(std::string name, std::vector<std::shared_ptr<const item_entry_data>> const &items)
{
	ZC_STORE_ITEMLIST_EQUIP_V6 pkt(get_session());
#if (PACKET_VERSION >= 20120925 && \
	((CLIENT_TYPE == 'R' && PACKET_VERSION < 20180829) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION < 20180919) || \
	(CLIENT_TYPE == 'M' && PACKET_VERSION < 20181002)))
	pkt.deliver(name, items);
#else
	pkt.deliver(INVTYPE_STORAGE, items);
#endif
	return true;
}

void ZoneClientInterface::upgrade_skill_level(int16_t skill_id)
{
	ZC_SKILLINFO_UPDATE pkt(get_session());
	
	std::shared_ptr<skill_learnt_info> ls = get_session()->player()->get_learnt_skill(skill_id);

	if (ls == nullptr) {
		skill_learnt_info s;
		s.skill_id = skill_id;
		s.level = 1;
		s.learn_type = SKILL_LEARN_PERMANENT;
		ls = std::make_shared<skill_learnt_info>(s);
		get_session()->player()->add_learnt_skill(ls);
	} else {
		ls->level += 1;
	}

	std::shared_ptr<const skill_config_data> skd = SkillDB->get_skill_by_id(ls->skill_id);

	bool upgradeable = false;

	job_class_type job_id = (job_class_type) get_session()->player()->job_id();
	std::shared_ptr<const skill_tree_config> stc = SkillDB->get_skill_tree_skill_id_by_job_id(job_id, ls->skill_id);

	if (stc == nullptr) {
		HLog(error) << "Skill ID " << ls->skill_id << " not found for Job ID " << (int32_t) job_id << ".";
		return;
	}

	upgradeable = (ls->level < stc->max_level) ? true : false;

	get_session()->player()->status()->skill_point()->sub_base(1);

	pkt.deliver(skill_id, ls->level, skd->sp_cost[ls->level], skd->use_range[ls->level], upgradeable);
}
bool ZoneClientInterface::notify_learnt_skill_list()
{
	const std::map<uint16_t, std::shared_ptr<skill_learnt_info>> &learnt_skills = get_session()->player()->get_learnt_skills();
	std::vector<zc_skill_info_data> sidv;

	for (auto sp : learnt_skills) {
		const std::shared_ptr<skill_learnt_info> ls = sp.second;

		if (ls == nullptr) {
			HLog(error) << "Learnt skill was a nullptr.";
			continue;
		}

		zc_skill_info_data si;

		std::shared_ptr<const skill_config_data> skd = SkillDB->get_skill_by_id(ls->skill_id);
		
		if (skd == nullptr) {
			HLog(error) << "Tried to send data for unknown skill with ID " << ls->skill_id;
			continue;
		}

		si.skill_id = ls->skill_id;
		si.skill_type = skd->primary_type;
		si.level = ls->level;

		si.sp_cost = ls->level ? skd->sp_cost[ls->level] : 0;
		si.range = ls->level ? skd->use_range[ls->level] : 0;
		
#if (CLIENT_VERSION == 'R' && PACKET_VERSION >= 20190807) || \
	(CLIENT_VERSION == 'Z' && PACKET_VERSION >= 20190918)
		si.level2 = ls->level;
#else
		strncpy(si.name, skd->name.c_str(), MAX_SKILL_NAME_LENGTH);
#endif

		if (ls->learn_type == SKILL_LEARN_PERMANENT) {
			job_class_type job_id = (job_class_type) get_session()->player()->job_id();
			std::shared_ptr<const skill_tree_config> stc = SkillDB->get_skill_tree_skill_id_by_job_id(job_id, ls->skill_id);

			if (stc == nullptr) {
				HLog(error) << "Skill ID " << ls->skill_id << " not found for Job ID " << (int32_t) job_id << ".";
				continue;
			}

			si.upgradeable = (ls->level < stc->max_level) ? 1 : 0;
		}

		sidv.push_back(si);
	}

	ZC_SKILLINFO_LIST pkt(get_session());
	pkt.deliver(sidv);

	return true;
}

void ZoneClientInterface::use_skill_on_target(int16_t skill_lv, int16_t skill_id, int target_guid)
{
	std::shared_ptr<Unit> target = get_session()->player()->get_nearby_unit(target_guid);
	
	if (target == nullptr)
		return;
	
	std::shared_ptr<const struct skill_config_data> skd = SkillDB->get_skill_by_id(skill_id);

	if (skd == nullptr) {
		HLog(error) << "Skill ID " << skill_id << " not found.";
		return;
	}

	CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config config;

	config.skill_id = skill_id;
	config.skill_lv = skill_lv;
	config.skd = skd;
	config.skill_execution = std::make_shared<SkillExecution>(get_session()->player(), skill_id, skill_lv);

	CombatRegistry::SkillExecutionOperation::SkillExecutionOperand *operand = new CombatRegistry::SkillExecutionOperation::SkillExecutionOperand(get_session()->player(), target, config);
	CombatRegistry::SkillExecutionOperation *operation = new CombatRegistry::SkillExecutionOperation(operand, CombatRegistry::SkillExecutionOperation::skill_execution_operation_type::SKILL_EXECUTION_OPERATION_TARGET);

	int time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	CombatRegistry::CombatStage *stage = get_session()->player()->combat_registry()->create_combat_stage(time);
	stage->add_operation(operation);
	get_session()->player()->combat_registry()->queue_combat_stage(stage);
	
	return;
}

void ZoneClientInterface::use_skill_on_ground(int16_t skill_lv, int16_t skill_id, int16_t pos_x, int16_t pos_y)
{
	auto ske = std::make_shared<SkillExecution>(get_session()->player(), skill_id, skill_lv);
	ske->execute(pos_x, pos_y);
}
void ZoneClientInterface::use_skill_on_ground(int16_t skill_lv, int16_t skill_id, int16_t pos_x, int16_t pos_y, std::string contents)
{
	auto ske = std::make_shared<SkillExecution>(get_session()->player(), skill_id, skill_lv);
	ske->execute(pos_x, pos_y, contents);
}

void ZoneClientInterface::notify_skill_cast(uint16_t skill_id, uint32_t src_guid, uint32_t target_guid, uint16_t target_x, uint16_t target_y, uint32_t element, int cast_time)
{
#if (CLIENT_TYPE == 'R' && PACKET_VERSION >= 20190807) || \
	(CLIENT_TYPE == 'Z' && PACKET_VERSION >= 20190918)
	ZC_USESKILL_ACK3 pkt(get_session());
	pkt.deliver(skill_id, src_guid, target_guid, target_x, target_y, element, cast_time);
#elif (CLIENT_TYPE == 'M' && PACKET_VERSION >= 20090406) || \
	(CLIENT_TYPE == 'S' && PACKET_VERSION >= 20080618) || \
	(CLIENT_TYPE == 'R' && PACKET_VERSION >= 20080827)
	ZC_USESKILL_ACK pkt(get_session());
	pkt.deliver(src_guid, target_guid, target_x, target_y, skill_id, element, cast_time);
#endif
}

void ZoneClientInterface::notify_safe_skill_use(int skill_id, int heal_amount, int target_guid, zc_use_skill2_result_type result)
{
	ZC_USE_SKILL2 pkt(get_session());
	pkt._skill_id = skill_id;
	pkt._heal_amount = heal_amount;
	pkt._target_guid = target_guid;
	pkt._result = (int) result;
	get_session()->player()->notify_in_area(pkt.serialize(), GRID_NOTIFY_AREA);
}

void ZoneClientInterface::notify_hostile_skill_use(int16_t skill_id, int32_t src_guid, int32_t target_guid, int32_t start_time, int32_t attack_motion, int32_t delay_motion, int32_t damage, int16_t level, int16_t number_of_hits, int8_t action)
{
	ZC_NOTIFY_SKILL2 pkt(get_session());

	pkt._skill_id = skill_id;
	pkt._source_id = src_guid;
	pkt._start_time = start_time;
	pkt._target_id = target_guid;
	pkt._level = level;
	pkt._attack_motion = attack_motion;
	pkt._attacked_motion = delay_motion;
	pkt._damage = damage;
	pkt._count = number_of_hits;
	pkt._action = action;
	
	get_session()->player()->notify_in_area(pkt.serialize(), GRID_NOTIFY_AREA);
}

void ZoneClientInterface::action_request(int32_t target_guid, player_action_type action)
{
	bool continuous = false;
	switch(action)
	{
		case PLAYER_ACT_SIT:
		case PLAYER_ACT_STAND:
		{
			get_session()->player()->on_action_request(action);
			break;
		}
		case PLAYER_ACT_ATTACK_REPEAT:
		{
			continuous = true;
		}
		case PLAYER_ACT_ATTACK:
		{
			std::shared_ptr<Unit> target = get_session()->player()->get_nearby_unit(target_guid);
			
			CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::s_melee_execution_operation_config config;
			config.continuous = continuous;
			CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand *operand = new CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand(get_session()->player(), target, config);
			CombatRegistry::MeleeExecutionOperation *operation = new CombatRegistry::MeleeExecutionOperation(operand, CombatRegistry::MeleeExecutionOperation::melee_execution_operation_type::MELEE_EXECUTION_OPERATION_TARGET);

			int time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			
			CombatRegistry::CombatStage *stage = get_session()->player()->combat_registry()->create_combat_stage(time);
			
			stage->add_operation(operation);

			get_session()->player()->combat_registry()->queue_combat_stage(stage);
		}
		default:
			break;
	};
	
}
bool ZoneClientInterface::notify_action(int32_t guid, player_action_type action)
{
	ZC_NOTIFY_ACT na(get_session());
	na.deliver(guid, (int8_t) action);
	return true;
}
bool ZoneClientInterface::notify_action(player_action_type action)
{
	ZC_NOTIFY_ACT na(get_session());
	na.deliver((int8_t) action);
	return true;
}
bool ZoneClientInterface::notify_status_change(int16_t si_type, int32_t guid, int8_t state, int32_t time_remaining, int32_t val1, int32_t val2, int32_t val3)
{
	ZC_MSG_STATE_CHANGE2 sc(get_session());

	if (time_remaining < 0)
		time_remaining = 9999;

	sc.deliver(si_type, guid, state, time_remaining, val1, val2, val3);
	return true;
}
bool ZoneClientInterface::notify_status_change_end(int16_t status_index, int32_t guid, int8_t state)
{
	ZC_MSG_STATE_CHANGE sce(get_session());
	sce.deliver(status_index, guid, state);
	return true;
}
bool ZoneClientInterface::notify_skill_fail(int16_t skill_id, int32_t message_type, int32_t item_id, skill_use_fail_cause_type cause)
{
	ZC_ACK_TOUSESKILL pkt(get_session());
	pkt.deliver(skill_id, message_type, item_id, cause);
	return true;
}
bool ZoneClientInterface::notify_damage(int guid, int target_guid, int start_time, int delay_skill, int delay_damage, int damage, bool is_sp_damaged, int number_of_hits, int8_t action_type, int left_damage)
{
	ZC_NOTIFY_ACT3 pkt(get_session());
	pkt.deliver(guid, target_guid, start_time, delay_skill, delay_damage, damage, is_sp_damaged, number_of_hits, action_type, left_damage);
	return true;
}
void ZoneClientInterface::open_vend_shop(int account_id)
{

}
void ZoneClientInterface::close_vending()
{
	
}
void ZoneClientInterface::open_buying_store(int account_id)
{

}
void ZoneClientInterface::close_buying_store()
{
	
}
void ZoneClientInterface::start_vending(std::string shop_name, std::vector<cz_req_openstore_itemlist> items)
{

}
void ZoneClientInterface::start_buying_store(std::string store_name, int zeny_limit, std::vector<cz_req_open_buying_store_itemlist> items)
{

}
void ZoneClientInterface::sell_to_buying_store(int account_id, int store_id, std::vector<cz_req_trade_buying_store_itemlist> items)
{

}
bool ZoneClientInterface::notify_buying_store_itemlist(int account_id, int store_id, int zeny_limit, std::vector<zc_ack_itemlist_buying_store> items)
{
	ZC_ACK_ITEMLIST_BUYING_STORE pkt(get_session());
	pkt.deliver(account_id, store_id, zeny_limit, items);
	return true;
}
bool ZoneClientInterface::notify_open_vending(zc_ack_openstore2_result_type result)
{
	ZC_ACK_OPENSTORE2 pkt(get_session());
	pkt.deliver(result);
	return true;
}
/**
 * Misc
 */
void ZoneClientInterface::emotion(int type)
{

}
void ZoneClientInterface::user_count()
{

}
void ZoneClientInterface::ignore_list()
{
	
}

/**
 * Class Specific 
 */

void ZoneClientInterface::star_gladiator_feel_save(cz_agree_starplace_type type)
{

}
void ZoneClientInterface::novice_explosion_spirits()
{

}
void ZoneClientInterface::novice_doridori()
{

}
void ZoneClientInterface::remember_warppoint()
{

}
void ZoneClientInterface::produce_item(int item_id, std::vector<int16_t> material_ids)
{

}
void ZoneClientInterface::remove_cart()
{

}
void ZoneClientInterface::change_cart(int16_t num)
{

}
void ZoneClientInterface::make_arrow(int16_t item_id)
{

}
void ZoneClientInterface::make_item(cz_req_makingitem_type type, int16_t item_id)
{

}

/**
 * Ranking 
 */
void ZoneClientInterface::ranking(cz_req_ranking_type type)
{

}
void ZoneClientInterface::ranking_alchemist()
{

}
void ZoneClientInterface::ranking_blacksmith()
{

}
void ZoneClientInterface::ranking_pk()
{

}
bool ZoneClientInterface::notify_ranking(cz_req_ranking_type type, std::vector<zc_ack_ranking_info> info, int ranking_points)
{
	ZC_ACK_RANKING pkt(get_session());
	pkt.deliver(type, info, ranking_points);
	return true;
}
bool ZoneClientInterface::notify_pvp_points()
{
	//int char_id; int account_id; int win_point; int lose_point; int point;
	//ZC_ACK_PVPPOINT pkt(get_session());
	//pkt.deliver(char_id, account_id, win_point, lose_point, point);
	return true;
}
void ZoneClientInterface::setting_effects(int setting)
{

}
/**
 * Battleground 
 */
void ZoneClientInterface::message(std::string message)
{

}
void ZoneClientInterface::blocking_play_cancel()
{
	ZC_ALT_PING pkt(get_session());
	pkt.deliver();
	// TODO: should also send ZC_SPECIALPOPUP for clients >= 20221005 based on mapflag.
}
void ZoneClientInterface::client_version(int version)
{

}
void ZoneClientInterface::change_direction(int head_direction, int body_direction)
{

}
void ZoneClientInterface::change_effect_state(int effect_state)
{

}
void ZoneClientInterface::change_map_type(int x, int y, bool walkable)
{

}
void ZoneClientInterface::bargain_sale_tool_close()
{

}
void ZoneClientInterface::searchstore_close()
{

}
void ZoneClientInterface::command_mercenary(int type, cz_command_mercenary_command_type command)
{

}
void ZoneClientInterface::auto_revive()
{
	
}
void ZoneClientInterface::command_pet(cz_command_pet_type command)
{

}
void ZoneClientInterface::pet_act_emotion(cz_pet_act_emotion_type emotion)
{

}
void ZoneClientInterface::pet_evolve(int evolved_pet_egg_id, std::vector<cz_pet_evolution_itemlist> items)
{
	
}
void ZoneClientInterface::view_equipment(int account_id)
{
	
}
void ZoneClientInterface::set_config(cz_config_type config, bool setting)
{
	switch(config)
	{
	case CZ_CONFIG_OPEN_EQUIPMENT_WINDOW:
		break;
	case CZ_CONFIG_CALL:
		break;
	case CZ_CONFIG_PET_AUTOFEED:
		break;
	case CZ_CONFIG_HOMUNCULUS_AUTOFEED:
		break;
	default:
		break;
	}
}
void ZoneClientInterface::rename_homunculus(std::string name)
{

}
void ZoneClientInterface::rename_pet(std::string name)
{

}
void ZoneClientInterface::request_action(int guid, int target_id, int action)
{

}
void ZoneClientInterface::move_homunculus_to_coordinates(int guid, int x, int y, int dir)
{

}
void ZoneClientInterface::move_homunculus_to_master(int guid)
{
	
}


/**
 * Administration
 */
void ZoneClientInterface::disconnect_all_players()
{

}
void ZoneClientInterface::disconnect_account(int account_id)
{

}
void ZoneClientInterface::create(std::string create)
{

}
void ZoneClientInterface::move_to_map(std::string map_name, int16_t x, int16_t y)
{

}
void ZoneClientInterface::recall(std::string username)
{

}
void ZoneClientInterface::recall(int account_id)
{

}


void ZoneClientInterface::summon(std::string char_name)
{

}
void ZoneClientInterface::warp_to(int account_id)
{

}
void ZoneClientInterface::request_username(int account_id)
{

}
void ZoneClientInterface::adjust_manner_by_name(std::string name)
{

}
void ZoneClientInterface::give_manner_point(int account_id, cz_req_give_manner_point_type type, int value)
{

}
void ZoneClientInterface::check_status(std::string name)
{

}
bool ZoneClientInterface::notify_give_manner_point(zc_ack_give_manner_point_result_type result)
{
	ZC_ACK_GIVE_MANNER_POINT pkt(get_session());
	pkt.deliver(result);
	return true;
}

/**
 * Instancing 
 */
void ZoneClientInterface::memorial_dungeon_command(cz_memorial_dungeon_command_type command)
{

}

/**
 * Cash Point Store 
 */
void ZoneClientInterface::cash_point_purchase(int kafra_points, std::vector<cz_pc_buy_cash_point_item> items)
{

}

/*
 * Private Airship
 */
void ZoneClientInterface::private_airship_request(std::string map_name, int item_id)
{

}