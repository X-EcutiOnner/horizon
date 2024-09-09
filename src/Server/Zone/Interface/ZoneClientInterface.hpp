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

#ifndef HORIZON_ZONECLIENTINTERFACE_HPP
#define HORIZON_ZONECLIENTINTERFACE_HPP

#include "Server/Common/Base/NetworkPacket.hpp"
#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Definitions/SkillDefinitions.hpp"
#include "Server/Zone/Definitions/PlayerDefinitions.hpp"
#include "Server/Common/Interfaces/ClientInterface.hpp"
#include "Server/Zone/Game/Map/Grid/GridDefinitions.hpp"
#include "Server/Zone/Packets/TransmittedPackets.hpp"
#include "Server/Zone/Interface/UI/UserInterface.hpp"

namespace Horizon
{
namespace Zone
{
namespace Units
{
	namespace Traits
	{
		class Status;
	}
	class Item;
}
class Unit;
class ZoneSession;
class ZoneClientInterface : public ClientInterface<ZoneSession>
{
public:
	ZoneClientInterface(std::shared_ptr<ZoneSession> s);
	~ZoneClientInterface();
	
	UI::Chatroom &chatroom() { return _chat_room; }
	UI::Trade &trade() { return _trade; }
	UI::Party &party() { return _party; }
	UI::PartyBooking &party_booking() { return _party_booking; }
	UI::Guild &guild() { return _guild; }
	UI::Friend &friend_() { return _friend; }
	UI::Quest &quest() { return _quest; }
	UI::Auction &auction() { return _auction; }
	UI::Mail &mail() { return _mail; }
	UI::Clan &clan() { return _clan; }

	bool login(uint32_t account_id, uint32_t char_id, uint32_t auth_code, uint32_t client_time, uint8_t gender);
	bool restart(uint8_t type);
	bool disconnect(int8_t type);
	bool update_session(int32_t account_id);
	
	bool walk_to_coordinates(uint16_t x, uint16_t y, uint8_t dir);
	
	bool notify_time();
	bool notify_unit_name(uint32_t guid);
	bool stop_attack();

	/**
	 * Character
	 */
	void request_name_by_char_id(int char_id);
	void invite_baby(int account_id);
	void pvpinfo(int character_id, int account_id);
	/**
	 * Ranking 
	 */
	void ranking(cz_req_ranking_type type);
	void ranking_alchemist();
	void ranking_blacksmith();
	void ranking_pk();
	bool notify_pvp_points();
	bool notify_ranking(cz_req_ranking_type type, std::vector<zc_ack_ranking_info> info, int ranking_points);
	/**
	 * Config
	*/
	void set_config(cz_config_type config, bool setting);
	void view_equipment(int account_id);

	/* Movement & Viewport*/
	bool notify_player_movement(MapCoords from, MapCoords to);
	bool notify_movement_stop(int32_t guid, int16_t x, int16_t y);
	bool notify_unit_move(int32_t guid, MapCoords from, MapCoords to);

	item_viewport_entry create_viewport_item_entry(std::shared_ptr<Units::Item> item);
	unit_viewport_entry create_viewport_entry(std::shared_ptr<Unit> unit);

	bool notify_viewport_add_unit(unit_viewport_entry entry);
	bool notify_viewport_remove_unit(std::shared_ptr<Unit> unit, unit_viewport_notification_type type);
	bool notify_viewport_moving_unit(unit_viewport_entry entry);
	bool notify_viewport_spawn_unit(unit_viewport_entry entry);
	bool notify_viewport_item_entry(item_viewport_entry entry);
	
	/**
	 * NPC
	 */
	void npc_weapon_refine(int32_t inventory_index);
	void npc_next_dialog(int32_t npc_guid);
	void npc_contact(int32_t npc_guid);
	void npc_close(int32_t npc_guid);
	void notify_npc_dialog(uint32_t npc_guid, std::string dialog);
	void notify_npc_next_dialog(uint32_t npc_guid);
	void notify_npc_close_dialog(uint32_t npc_guid);
	void notify_npc_menu_list(uint32_t npc_guid, std::string const &menu);
	uint32_t get_npc_contact_guid() { return _npc_contact_guid; }
	void set_npc_contact_guid(uint32_t guid) { _npc_contact_guid = guid; }
	void npc_select_deal_type(int guid, cz_ack_select_dealtype deal_type);
	void npc_select_menu(int guid, int choice);
	void npc_input(int guid, int value);
	void npc_input(int guid, std::string value);
	void npc_purchase_items(std::vector<cz_pc_purchase_itemlist> items);
	void npc_sell_items(std::vector<cz_pc_sell_itemlist> items);
	void vending_purchase_items(int account_id, std::vector<cz_pc_purchase_itemlist> items);
	void vending_purchase_items(int account_id, int unique_id, std::vector<cz_pc_purchase_itemlist> items);
	void progress_bar_completed();
	
	/**
	 * Status
	 */
	bool notify_initial_status();
	bool notify_appearance_update(unit_appearance_type type, int32_t value, int32_t value2);
	bool notify_compound_attribute_update(status_point_type type, int32_t value);
	bool notify_status_attribute_update(status_point_type type, int32_t value, bool success);
	bool notify_required_attribute_update(status_point_type type, int32_t value);
	bool notify_attribute_update(status_point_type type, int32_t value, int32_t value2);
	bool notify_cart_weight_update();
	bool notify_attack_range_update(int32_t value);
	bool notify_experience_update(status_point_type type, int32_t value);
	bool notify_zeny_update();
	bool increase_status_point(status_point_type type, uint8_t amount);
	void register_baby(int account_id, int character_id, cz_join_baby_reply_type response);
	void auto_revive();
	/**
	 * Map
	 */
	bool notify_move_to_map(std::string map_name, int16_t x, int16_t y);
	void map_enter();
	bool notify_map_properties(zc_map_properties properties);

	/**
	 * Chat
	 */
	bool notify_chat(std::string message);
	void parse_chat_message(std::string message);
	void whisper_message(const char *name, int32_t name_length, const char *message, int32_t message_length);

	/**
	 * Items
	 */
	void use_item(int16_t inventory_index, int32_t guid);
	void equip_item(int16_t inventory_index, int16_t equip_location_mask);
	void pickup_item(int guid);
	void unequip_item(int16_t inventory_index);
	void throw_item(int16_t inventory_index, int16_t amount);
	void move_item_from_inventory_to_cart(int16_t inventory_index, int amount);
	void move_item_from_inventory_to_storage(int16_t inventory_index, int amount);
	void move_item_from_cart_to_inventory(int16_t inventory_index, int amount);
	void move_item_from_cart_to_storage(int16_t inventory_index, int amount);
	void move_item_from_storage_to_inventory(int16_t storage_index, int amount);
	void move_item_from_storage_to_cart(int16_t inventory_index, int amount);
	void display_item_card_composition(int card_index, int equip_index = 0);
	void repair_item(int inventory_index, int item_id, int refine, int card1, int card2, int card3, int card4);
	void identify_item(int inventory_index);
	bool notify_pickup_item(std::shared_ptr<item_entry_data> item, int16_t amount, item_inventory_addition_notif_type result);
	bool notify_normal_item_list(std::vector<std::shared_ptr<const item_entry_data>> const &items);
	bool notify_equipment_item_list(std::vector<std::shared_ptr<const item_entry_data>> const &items);
	bool notify_throw_item(int16_t inventory_index, int16_t amount);
	bool notify_inventory_move_failed(int16_t inventory_index, bool silent);
	bool notify_delete_item(int16_t inventory_index, int16_t amount, item_deletion_reason_type reason);
	bool notify_bind_on_equip(int16_t inventory_index);
	bool notify_use_item(std::shared_ptr<item_entry_data> inv_item, bool success);
	bool notify_equip_item(std::shared_ptr<const item_entry_data> item, item_equip_result_type result);
	bool notify_unequip_item(std::shared_ptr<const item_entry_data> item, item_unequip_result_type result);
	bool notify_equip_arrow(std::shared_ptr<const item_entry_data> item);
	bool notify_action_failure(int16_t message_type);
	bool notify_item_composition(int16_t item_inventory_index, int16_t card_inventory_index, zc_ack_item_composition_result_type result);
	bool notify_identified_item(int16_t inventory_index, zc_ack_item_identify_result_type result);
	bool notify_item_refining(zc_ack_itemrefining_result_type result, int16_t inventory_index, int16_t refine_lv);
	bool notify_item_repair(int inventory_index, zc_ack_itemrepair_result_type result);
	bool notify_item_merge(int inventory_index, int amount, zc_ack_merge_item_reason_type reason);
	bool notify_item_drop(int guid, int item_id, int type, int identified, int x, int y, int x_area, int y_area, int amount, int show_drop_effect = 0, int drop_effect_mode = 0);
	bool notify_item_removal_from_floor(int guid);
	
	/**
	 * Inventory Start / End 
	 */
	bool notify_inventory_start(inventory_type type, std::string name);
	bool notify_inventory_end(inventory_type type);
	/**
	 * Storage
	 */
	void storage_close();
	bool notify_storage_size(int16_t total_size, int16_t max_size);
	bool notify_storage_normal_items(std::string name, std::vector<std::shared_ptr<const item_entry_data>> const &items);
	bool notify_storage_equip_items(std::string name, std::vector<std::shared_ptr<const item_entry_data>> const &items);
	bool notify_storage_close();
	bool notify_storage_add_item(std::shared_ptr<const item_entry_data> entry, int amount);
	bool notify_storage_remove_item(int16_t storage_index, int amount);
	
	/**
	 * Skills
	 */
	void upgrade_skill_level(int16_t skill_id);
	bool notify_learnt_skill_list();
	bool notify_skill_fail(int16_t skill_id, int32_t message_type, int32_t item_id, skill_use_fail_cause_type cause);
	void use_skill_on_target(int16_t skill_lv, int16_t skill_id, int target_guid);
	void use_skill_on_ground(int16_t skill_lv, int16_t skill_id, int16_t pos_x, int16_t pos_y);
	void use_skill_on_ground(int16_t skill_lv, int16_t skill_id, int16_t pos_x, int16_t pos_y, std::string contents);
	void notify_skill_cast(uint16_t skill_id, uint32_t src_guid, uint32_t target_guid, uint16_t target_x, uint16_t target_y, uint32_t element, int cast_time);
	void notify_safe_skill_use(int skill_id, int heal_amount, int target_guid, zc_use_skill2_result_type result);
	void notify_hostile_skill_use(int16_t skill_id, int32_t src_guid, int32_t target_guid, int32_t start_time, int32_t attack_motion, int32_t delay_motion, int32_t damage, int16_t level, int16_t count, int8_t action);
	
	/* UI Requests */
	void action_request(int32_t target_guid, player_action_type action);
	bool notify_action(player_action_type action);
	bool notify_action(int32_t target_guid, player_action_type action);

	/**
	 * Status
	 */
	void setting_effects(int setting);
	bool notify_status_change(int16_t si_type, int32_t guid, int8_t state, int32_t time_remaining, int32_t val1, int32_t val2, int32_t val3);
	bool notify_status_change_end(int16_t status_index, int32_t guid, int8_t state);

	/**
	 * Combat
	 */
	bool notify_damage(int guid, int target_guid, int start_time, int delay_skill, int delay_damage, int damage, bool is_sp_damaged, int number_of_hits, int8_t action_type, int left_damage);

	/**
	 * Storage
	 */
	void storage_check_password(std::string password);
	void storage_change_password(std::string password, std::string new_password);

	/**
	 * Vending
	 */
	void open_vend_shop(int account_id);
	void close_vending();
	void open_buying_store(int account_id);
	void close_buying_store();
	void start_vending(std::string shop_name, std::vector<cz_req_openstore_itemlist> items);
	void start_buying_store(std::string store_name, int zeny_limit, std::vector<cz_req_open_buying_store_itemlist> items);
	void sell_to_buying_store(int account_id, int store_id, std::vector<cz_req_trade_buying_store_itemlist> items);
	bool notify_buying_store_itemlist(int account_id, int store_id, int zeny_limit, std::vector<zc_ack_itemlist_buying_store> items);
	bool notify_open_vending(zc_ack_openstore2_result_type result);

	/**
	 * Misc
	 */
	void emotion(int type);
	void user_count();
	void ignore_list();

	/**
	 * Class Specific 
	 */
	void star_gladiator_feel_save(cz_agree_starplace_type type);
	void novice_explosion_spirits(); // Chopokgi.
	void novice_doridori(); // DoriDori
	void remember_warppoint();
	void produce_item(int item_id, std::vector<int16_t> material_ids);
	void remove_cart();
	void change_cart(int16_t num);
	void make_arrow(int16_t item_id);
	void make_item(cz_req_makingitem_type type, int16_t item_id);

	/**
	 * Battlegrounds 
	 */
	void message(std::string message);

	void blocking_play_cancel();
	void client_version(int version);
	void broadcast(std::string message);
	void broadcast_local(std::string message);
	void change_direction(int head_direction, int body_direction);
	void change_effect_state(int effect_state);
	void change_map_type(int x, int y, bool walkable);
	void bargain_sale_tool_close();
	void searchstore_close();

	/**
	 * Pet
	 */
	void command_mercenary(int type, cz_command_mercenary_command_type command);
	void command_pet(cz_command_pet_type command);
	void pet_act_emotion(cz_pet_act_emotion_type emotion);
	void pet_evolve(int evolved_pet_egg_id, std::vector<cz_pet_evolution_itemlist> items);
	void rename_homunculus(std::string name);
	void rename_pet(std::string name);
	void request_action(int guid, int target_id, int action);
	void move_homunculus_to_coordinates(int guid, int x, int y, int dir);
	void move_homunculus_to_master(int guid);
	
	/**
	 * Administration
	 */
	void disconnect_all_players();
	void disconnect_account(int account_id);
	void create(std::string create);
	void move_to_map(std::string map_name, int16_t x, int16_t y);
	void recall(std::string username);
	void recall(int account_id);
	void summon(std::string char_name);
	void warp_to(int account_id);
	void request_username(int account_id);
	void adjust_manner_by_name(std::string name);
	void give_manner_point(int account_id, cz_req_give_manner_point_type type, int value);
	void check_status(std::string name);
	bool notify_give_manner_point(zc_ack_give_manner_point_result_type result);

	/**
	 * Instancing 
	 */
	void memorial_dungeon_command(cz_memorial_dungeon_command_type command);

	/**
	 * Cash Point Store 
	 */
	void cash_point_purchase(int kafra_points, std::vector<cz_pc_buy_cash_point_item> items);

	/*
	 * Private Airship
	 */
	void private_airship_request(std::string map_name, int item_id);
  
protected:
	uint32_t _npc_contact_guid{0};
	UI::Chatroom _chat_room;
	UI::Trade _trade;
	UI::Party _party;
	UI::Guild _guild;
	UI::Friend _friend;
	UI::Quest _quest;
	UI::Auction _auction;
	UI::Mail _mail;
	UI::Clan _clan;
	UI::PartyBooking _party_booking;
};
}
}

#endif /* HORIZON_ZONECLIENTINTERFACE_HPP */
