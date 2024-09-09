/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
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

#include "ItemComponent.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Game/StaticDB/ItemDB.hpp"
#include "Server/Zone/Game/Map/Grid/Notifiers/GridNotifiers.hpp"

#include "ItemDefinitions.hpp"

using namespace Horizon::Zone;

void ItemComponent::sync_definitions(std::shared_ptr<sol::state> state)
{
	sol::table tbl_1 = state->create_named_table("RefineType");
	tbl_1["Armor"] = (int) REFINE_TYPE_ARMOR; 
	tbl_1["Weapon1"] = (int) REFINE_TYPE_WEAPON1; 
	tbl_1["Weapon2"] = (int) REFINE_TYPE_WEAPON2; 
	tbl_1["Weapon3"] = (int) REFINE_TYPE_WEAPON3; 
	tbl_1["Weapon4"] = (int) REFINE_TYPE_WEAPON4; 
	tbl_1["Max"] = (int) REFINE_TYPE_MAX;

	sol::table tbl_2 = state->create_named_table("RefineChanceType");
	tbl_2["Normal"] = (int) REFINE_CHANCE_TYPE_NORMAL; 
	tbl_2["Enriched"] = (int) REFINE_CHANCE_TYPE_ENRICHED; 
	tbl_2["EventNormal"] = (int) REFINE_CHANCE_TYPE_E_NORMAL; 
	tbl_2["EventEnriched"] = (int) REFINE_CHANCE_TYPE_E_ENRICHED; 
	tbl_2["Max"] = (int) REFINE_CHANCE_TYPE_MAX;

	sol::table tbl_3 = state->create_named_table("WeaponType");
	tbl_3["Fist"] = (int) IT_WT_FIST; 
	tbl_3["Dagger"] = (int) IT_WT_DAGGER; 
	tbl_3["OneHandedSword"] = (int) IT_WT_1HSWORD; 
	tbl_3["TwoHandedSword"] = (int) IT_WT_2HSWORD; 
	tbl_3["OneHandedSpear"] = (int) IT_WT_1HSPEAR; 
	tbl_3["TwoHandedSpear"] = (int) IT_WT_2HSPEAR; 
	tbl_3["OneHandedAxe"] = (int) IT_WT_1HAXE; 
	tbl_3["TwoHandedAxe"] = (int) IT_WT_2HAXE; 
	tbl_3["OneHandedMace"] = (int) IT_WT_1HMACE; 
	tbl_3["TwoHandedMace"] = (int) IT_WT_2HMACE; 
	tbl_3["OneHandedStaff"] = (int) IT_WT_STAFF; 
	tbl_3["Bow"] = (int) IT_WT_BOW; 
	tbl_3["Knuckle"] = (int) IT_WT_KNUCKLE; 
	tbl_3["MusicalInstrument"] = (int) IT_WT_MUSICAL; 
	tbl_3["Whip"] = (int) IT_WT_WHIP; 
	tbl_3["Book"] = (int) IT_WT_BOOK; 
	tbl_3["Katar"] = (int) IT_WT_KATAR; 
	tbl_3["Revolver"] = (int) IT_WT_REVOLVER; 
	tbl_3["Rifle"] = (int) IT_WT_RIFLE; 
	tbl_3["GatlingGun"] = (int) IT_WT_GATLING; 
	tbl_3["Shotgun"] = (int) IT_WT_SHOTGUN; 
	tbl_3["GrenadeLauncher"] = (int) IT_WT_GRENADE; 
	tbl_3["FuumaShuriken"] = (int) IT_WT_HUUMA; 
	tbl_3["TwoHandedStaff"] = (int) IT_WT_2HSTAFF; 
	tbl_3["DWDaggers"] = (int) IT_WT_DOUBLE_DAGGERS;  ///< 2 daggers
	tbl_3["DWSwords"] = (int) IT_WT_DOUBLE_SWORDS;  ///< 2 swords
	tbl_3["DWAxes"] = (int) IT_WT_DOUBLE_AXES;  ///< 2 axes
	tbl_3["DWDaggerSword"] = (int) IT_WT_DOUBLE_DAGGER_SWORD;  ///< dagger + sword
	tbl_3["DWDaggerAxe"] = (int) IT_WT_DOUBLE_DAGGER_AXE;  ///< dagger + axe
	tbl_3["DWSwordAxe"] = (int) IT_WT_DOUBLE_SWORD_AXE; ///< sword + axe

	sol::table tbl_4 = state->create_named_table("WeaponTypeMask");
	tbl_4["Ranged"] = (int) IT_WTM_RANGED; 
	tbl_4["Melee"] = (int) IT_WTM_MELEE;

	sol::table tbl_5 = state->create_named_table("ItemLevelType");
	tbl_5["Armor"] = (int) IT_LVL_ARMOR; 
	tbl_5["Weapon1"] = (int) IT_LVL_WEAPON1; 
	tbl_5["Weapon2"] = (int) IT_LVL_WEAPON2; 
	tbl_5["Weapon3"] = (int) IT_LVL_WEAPON3; 
	tbl_5["Weapon4"] = (int) IT_LVL_WEAPON4; 
	tbl_5["Max"] = (int) IT_LVL_MAX;

	sol::table tbl_6 = state->create_named_table("ItemGenderType");
	tbl_6["Female"] = (int) IT_GENDER_FEMALE; 
	tbl_6["Male"] = (int) IT_GENDER_MALE; 
	tbl_6["Any"] = (int) IT_GENDER_ANY;

	sol::table tbl_7 = state->create_named_table("EquipLocationMask");
	tbl_7["None"] = (int) IT_EQPM_NONE; 
	tbl_7["LowerHead"] = (int) IT_EQPM_HEAD_LOW; 
	tbl_7["RightHand"] = (int) IT_EQPM_HAND_R; 
	tbl_7["Garment"] = (int) IT_EQPM_GARMENT; 
	tbl_7["LeftAccessory"] = (int) IT_EQPM_ACC_L; 
	tbl_7["Armor"] = (int) IT_EQPM_ARMOR; 
	tbl_7["LeftHand"] = (int) IT_EQPM_HAND_L; 
	tbl_7["Shoes"] = (int) IT_EQPM_SHOES; 
	tbl_7["RightAccessory"] = (int) IT_EQPM_ACC_R; 
	tbl_7["TopHead"] = (int) IT_EQPM_HEAD_TOP; 
	tbl_7["MidHead"] = (int) IT_EQPM_HEAD_MID; 
	tbl_7["CostumeTopHead"] = (int) IT_EQPM_COSTUME_HEAD_TOP; 
	tbl_7["CostumeMidHead"] = (int) IT_EQPM_COSTUME_HEAD_MID; 
	tbl_7["CostumeLowerHead"] = (int) IT_EQPM_COSTUME_HEAD_LOW; 
	tbl_7["CostumeGarment"] = (int) IT_EQPM_COSTUME_GARMENT; 
	tbl_7["Ammunition"] = (int) IT_EQPM_AMMO; 
	tbl_7["ShadowArmor"] = (int) IT_EQPM_SHADOW_ARMOR; 
	tbl_7["ShadowWeapon"] = (int) IT_EQPM_SHADOW_WEAPON; 
	tbl_7["ShadowShield"] = (int) IT_EQPM_SHADOW_SHIELD; 
	tbl_7["ShadowShoes"] = (int) IT_EQPM_SHADOW_SHOES; 
	tbl_7["ShadowRightAccessory"] = (int) IT_EQPM_SHADOW_ACC_R; 
	tbl_7["ShadowLeftAccessory"] = (int) IT_EQPM_SHADOW_ACC_L; 
	tbl_7["Weapon"] = (int) IT_EQPM_WEAPON; 
	tbl_7["Shield"] = (int) IT_EQPM_SHIELD; 
	tbl_7["BothArms"] = (int) IT_EQPM_ARMS; 
	tbl_7["AllHeadgear"] = (int) IT_EQPM_HELM; 
	tbl_7["AllAccessory"] = (int) IT_EQPM_ACC; 
	tbl_7["AllCostume"] = (int) IT_EQPM_COSTUME; 
	tbl_7["ShadowAccessory"] = (int) IT_EQPM_SHADOW_ACC; 
	tbl_7["ShadowArms"] = (int) IT_EQPM_SHADOW_ARMS;

	sol::table tbl_8 = state->create_named_table("EquipLocationIndex");
	tbl_8["LeftAccessory"] = (int) IT_EQPI_ACC_L; 
	tbl_8["RightAccessory"] = (int) IT_EQPI_ACC_R; 
	tbl_8["Shoes"] = (int) IT_EQPI_SHOES; 
	tbl_8["Garment"] = (int) IT_EQPI_GARMENT; 
	tbl_8["LowerHead"] = (int) IT_EQPI_HEAD_LOW; 
	tbl_8["MidHead"] = (int) IT_EQPI_HEAD_MID; 
	tbl_8["TopHead"] = (int) IT_EQPI_HEAD_TOP; 
	tbl_8["Armor"] = (int) IT_EQPI_ARMOR; 
	tbl_8["LeftHand"] = (int) IT_EQPI_HAND_L; 
	tbl_8["RightHand"] = (int) IT_EQPI_HAND_R; 
	tbl_8["CostumeTopHead"] = (int) IT_EQPI_COSTUME_TOP; 
	tbl_8["CostumeMidHead"] = (int) IT_EQPI_COSTUME_MID; 
	tbl_8["CostumeLowHead"] = (int) IT_EQPI_COSTUME_LOW; 
	tbl_8["CostumeGarment"] = (int) IT_EQPI_COSTUME_GARMENT; 
	tbl_8["Ammunition"] = (int) IT_EQPI_AMMO; 
	tbl_8["ShadowArmor"] = (int) IT_EQPI_SHADOW_ARMOR; 
	tbl_8["ShadowWeapon"] = (int) IT_EQPI_SHADOW_WEAPON; 
	tbl_8["ShadowShield"] = (int) IT_EQPI_SHADOW_SHIELD; 
	tbl_8["ShadowShoes"] = (int) IT_EQPI_SHADOW_SHOES; 
	tbl_8["ShadowRightAccessory"] = (int) IT_EQPI_SHADOW_ACC_R; 
	tbl_8["ShadowLeftAccessory"] = (int) IT_EQPI_SHADOW_ACC_L; 
	tbl_8["Max"] = (int) IT_EQPI_MAX;

	sol::table tbl_9 = state->create_named_table("ItemTradeMask");
	tbl_9["None"] = (int) ITRMASK_NONE; 
	tbl_9["Drop"] = (int) ITRMASK_DROP; 
	tbl_9["Trade"] = (int) ITRMASK_TRADE; 
	tbl_9["TradeWithPartner"] = (int) ITRMASK_TRADEWPARTNER; 
	tbl_9["SellToNPC"] = (int) ITRMASK_SELL_TO_NPC; 
	tbl_9["StoreInCart"] = (int) ITRMASK_CART; 
	tbl_9["StoreInStorage"] = (int) ITRMASK_STORAGE; 
	tbl_9["StoreInGuildStorage"] = (int) ITRMASK_GSTORAGE; 
	tbl_9["Mail"] = (int) ITRMASK_MAIL; 
	tbl_9["Auction"] = (int) ITRMASK_AUCTION; 
	tbl_9["SellInBuyingStore"] = (int) ITRMASK_BUYING_STORE; 
	tbl_9["AllowAll"] = (int) ITRMASK_ALL;

	sol::table tbl_10 = state->create_named_table("ItemUsageMask");
	tbl_10["Sitting"] = (int) IURMASK_SITTING; 
	tbl_10["All"] = (int) IURMASK_ALL;

	sol::table tbl_11 = state->create_named_table("ItemType");
	tbl_11["Healing"] = (int) IT_TYPE_HEALING; 
	tbl_11["Unknown"] = (int) IT_TYPE_UNKNOWN; 
	tbl_11["Usable"] = (int) IT_TYPE_USABLE; 
	tbl_11["Etc"] = (int) IT_TYPE_ETC; 
	tbl_11["Weapon"] = (int) IT_TYPE_WEAPON; 
	tbl_11["Armor"] = (int) IT_TYPE_ARMOR; 
	tbl_11["Card"] = (int) IT_TYPE_CARD; 
	tbl_11["PetEgg"] = (int) IT_TYPE_PET_EGG; 
	tbl_11["PetArmor"] = (int) IT_TYPE_PET_ARMOR; 
	tbl_11["Unknown2"] = (int) IT_TYPE_UNKNOWN2; 
	tbl_11["Ammunition"] = (int) IT_TYPE_AMMO; 
	tbl_11["ConsumptionDelay"] = (int) IT_TYPE_CONSUMPTION_DELAY; 
	tbl_11["Cash"] = (int) IT_TYPE_CASH; 
	tbl_11["Max"] = (int) IT_TYPE_MAX;

	sol::table tbl_12 = state->create_named_table("AmmoType");
	tbl_12["None"] = (int) IT_AT_NONE; 
	tbl_12["Arrow"] = (int) IT_AT_ARROW; 
	tbl_12["ThrowableDagger"] = (int) IT_AT_DAGGER; 
	tbl_12["Bullet"] = (int) IT_AT_BULLET; 
	tbl_12["Shell"] = (int) IT_AT_SHELL; 
	tbl_12["Grenade"] = (int) IT_AT_GRENADE; 
	tbl_12["Shuriken"] = (int) IT_AT_SHURIKEN; 
	tbl_12["Kunai"] = (int) IT_AT_KUNAI; 
	tbl_12["CannonBall"] = (int) IT_AT_CANNONBALL; 
	tbl_12["ThrowableItem"] = (int) IT_AT_THROWABLE_ITEM; 
	tbl_12["Max"] = (int) IT_AT_MAX;

	sol::table tbl_13 = state->create_named_table("ItemBindType");
	tbl_13["None"] = (int) IT_BIND_NONE; 
	tbl_13["Account"] = (int) IT_BIND_ACCOUNT; 
	tbl_13["Guild"] = (int) IT_BIND_GUILD; 
	tbl_13["Party"] = (int) IT_BIND_PARTY; 
	tbl_13["Character"] = (int) IT_BIND_CHARACTER; 
	tbl_13["Max"] = (int) IT_BIND_MAX;
}

void ItemComponent::sync_data_types(std::shared_ptr<sol::state> state)
{

	
	state->new_usertype<item_entry_data::options>("item_options",
		"index", sol::property(&item_entry_data::options::get_index, &item_entry_data::options::set_index),
		"value", sol::property(&item_entry_data::options::get_value, &item_entry_data::options::set_value)
	);

	sol::usertype<item_entry_data> config_1 = state->new_usertype<item_entry_data>("item_entry_data");
	config_1["is_equipment"] = &item_entry_data::is_equipment;
	config_1["is_stackable"] = &item_entry_data::is_stackable;
	config_1["is_same_as"] = &item_entry_data::operator ==;
	config_1["storage_type"] = &item_entry_data::storage_type;
	config_1["storage_index"] = [] (item_entry_data &item) {
		return std::ref(item.index.storage);
	};
	config_1["inventory_index"] = [] (item_entry_data &item) {
		return std::ref(item.index.inventory);
	};
	config_1["item_id"] = sol::readonly(&item_entry_data::item_id);
	config_1["type"] = sol::readonly(&item_entry_data::type);
	config_1["amount"] = sol::readonly(&item_entry_data::amount);
	config_1["current_equip_location_mask"] = sol::readonly(&item_entry_data::current_equip_location_mask);
	config_1["actual_equip_location_mask"] = sol::readonly(&item_entry_data::actual_equip_location_mask);
	config_1["refine_level"] = sol::property(&item_entry_data::get_refine_level, &item_entry_data::set_refine_level);
	config_1["slot_item_id"] = sol::property([] (item_entry_data &item) {
		return std::ref(item.slot_item_id);
	});
	config_1["hire_expire_date"] = sol::property(&item_entry_data::get_hire_expire_date, &item_entry_data::set_hire_expire_date);
	config_1["bind_type"] = sol::property(&item_entry_data::get_bind_type, &item_entry_data::set_bind_type);
	config_1["sprite_id"] = sol::readonly(&item_entry_data::sprite_id);
	config_1["ele_type"] = sol::readonly(&item_entry_data::ele_type);
	config_1["option_count"] = sol::property(&item_entry_data::get_option_count, &item_entry_data::set_option_count);
	config_1["options_data"] = sol::property([] (item_entry_data &item) {
		return std::ref(item.option_data);
	});
	config_1["unique_id"] = sol::readonly(&item_entry_data::unique_id);

	sol::usertype<item_config_data> config_2 = state->new_usertype<item_config_data>("item_config_data");
	config_2["item_id"] = sol::readonly(&item_config_data::item_id);
	config_2["name"] = sol::readonly(&item_config_data::name);
	config_2["key_name"] = sol::readonly(&item_config_data::key_name);
	config_2["value_buy"] = sol::readonly(&item_config_data::value_buy);
	config_2["value_sell"] = sol::readonly(&item_config_data::value_sell);
	config_2["type"] = sol::readonly(&item_config_data::type);
	config_2["item_sub_type"] = sol::readonly_property([] (item_config_data &item) {
		return item.type == IT_TYPE_WEAPON ? item.sub_type.weapon_t : item.sub_type.ammo_t;
	});
	config_2["equip_location_mask"] = sol::readonly(&item_config_data::equip_location_mask);
	config_2["weight"] = sol::readonly(&item_config_data::weight);
	config_2["attack"] = sol::readonly(&item_config_data::attack);
	config_2["defense"] = sol::readonly(&item_config_data::defense);
	config_2["attack_range"] = sol::readonly(&item_config_data::attack_range);
	config_2["card_slot_count"] = sol::readonly(&item_config_data::card_slot_count);
	config_2["sprite_id"] = sol::readonly(&item_config_data::sprite_id);
	config_2["level"] = sol::readonly_property([] (item_config_data &item) {
		return item.type == IT_TYPE_WEAPON ? (uint8_t) item.level.weapon : (uint8_t) item.level.armor;
	});
	config_2["magic_atk"] = sol::readonly(&item_config_data::magic_atk);
	config_2["req_min_lv"] = sol::readonly_property([] (item_config_data &item) {
		return item.requirements.min_lv;
	});
	config_2["req_max_lv"] = sol::readonly_property([] (item_config_data &item) {
		return item.requirements.max_lv;
	});
	config_2["req_job_ids"] = sol::readonly_property([] (item_config_data &item) {
		return item.requirements.job_ids;
	});
	config_2["req_gender"] = sol::readonly_property([] (item_config_data &item) {
		return item.requirements.gender;
	});
	config_2["default_script"] = sol::readonly(&item_config_data::default_script);
	config_2["equip_script"] = sol::readonly(&item_config_data::equip_script);
	config_2["unequip_script"] = sol::readonly(&item_config_data::unequip_script);
	config_2["available"] = sol::readonly_property([] (item_config_data &item) {
		return item.config.available ? 1 : 0;
	});
	config_2["refineable"] = sol::readonly_property([] (item_config_data &item) {
		return item.config.refineable ? 1 : 0;
	});
	config_2["trade_restriction_mask"] = sol::readonly_property([] (item_config_data &item) {
		return (int) item.trade_restriction_mask;
	});
	config_2["trade_restriction_group_override_id"] = sol::readonly_property([] (item_config_data &item) {
		return (int) item.trade_restriction_group_override_id;
	});
	config_2["trade_restriction_partner_override"] = sol::readonly_property([] (item_config_data &item) {
		return (bool) item.trade_restriction_partner_override;
	});
	config_2["usage_restriction_mask"] = sol::readonly_property([] (item_config_data &item) {
		return (int) item.usage_restriction_mask;
	});
	config_2["usage_restriction_group_override_id"] = sol::readonly_property([] (item_config_data &item) {
		return (int) item.usage_restriction_group_override_id;
	});
	config_2["allow_item_options"] = sol::readonly_property([] (item_config_data &item) {
		return item.config.allow_item_options ? 1 : 0;
	});
	config_2["bind_on_equip"] = sol::readonly_property([] (item_config_data &item) {
		return item.config.bind_on_equip ? 1 : 0;
	});
	config_2["drop_announce"] = sol::readonly_property([] (item_config_data &item) {
		return item.config.drop_announce ? 1 :0;
	});

	sol::usertype<s_grid_notify_item_drop_entry> item_drop = state->new_usertype<s_grid_notify_item_drop_entry>("s_grid_notify_item_drop_entry");
	item_drop["guid"] = &s_grid_notify_item_drop_entry::guid;
	item_drop["item_id"] = &s_grid_notify_item_drop_entry::item_id;
	item_drop["type"] = &s_grid_notify_item_drop_entry::type;
	item_drop["is_identified"] = &s_grid_notify_item_drop_entry::is_identified;
	item_drop["x"] = &s_grid_notify_item_drop_entry::x;
	item_drop["y"] = &s_grid_notify_item_drop_entry::y;
	item_drop["x_area"] = &s_grid_notify_item_drop_entry::x_area;
	item_drop["y_area"] = &s_grid_notify_item_drop_entry::y_area;
	item_drop["amount"] = &s_grid_notify_item_drop_entry::amount;
	item_drop["show_drop_effect"] = &s_grid_notify_item_drop_entry::show_drop_effect;
	item_drop["drop_effect_mode"] = &s_grid_notify_item_drop_entry::drop_effect_mode;
}

void ItemComponent::sync_functions(std::shared_ptr<sol::state> state)
{
	state->set_function("get_item_info",
					[] (uint32_t item_id) {
						return ItemDB->get_item_by_id(item_id);
					});
}