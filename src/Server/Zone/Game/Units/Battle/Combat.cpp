/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 *
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

#include "Combat.hpp"
#include "Server/Zone/Definitions/ItemDefinitions.hpp"
#include "Server/Zone/Definitions/ClientDefinitions.hpp"
#include "Server/Zone/Game/GameLogicProcess.hpp"
#include "Server/Zone/Game/Map/Grid/Notifiers/GridNotifiers.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Game/Units/Player/Assets/Inventory.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Mob/Hostile/Monster.hpp"
#include "Server/Zone/Game/StaticDB/ItemDB.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Interface/ZoneClientInterface.hpp"

using namespace Horizon::Zone;

CombatRegistry::CombatRegistry(std::shared_ptr<Unit> unit)
: _unit(unit)
{

}

CombatRegistry::~CombatRegistry() 
{

}

Combat::Combat(std::shared_ptr<Unit> unit, std::shared_ptr<Unit> target)
: _unit(unit), _target(target), _start_time(std::time(nullptr))
{
}

Combat::~Combat()
{
}

combat_retaliate_type Combat::weapon_attack()
{
    combat_damage dmg;
    
    if (unit()->type() == UNIT_PLAYER) {
        EquipmentListType const& equipments = unit()->downcast<Horizon::Zone::Units::Player>()->inventory()->equipments();
        std::shared_ptr<const item_entry_data> weapon = nullptr;

        // Calculate element damage ratio and damage.
        int64_t batk = unit()->status()->base_attack()->total();

        if (equipments[IT_EQPI_HAND_R].second.expired() == false) {
            batk = deduce_weapon_element_attack(batk, ELE_NEUTRAL, IT_EQPI_HAND_R);
            batk = deduce_damage_size_modifier(batk, IT_EQPI_HAND_R);
            dmg.right_damage = (batk << 1);
        }

        if (equipments[IT_EQPI_HAND_L].second.expired() == false) {
            batk = deduce_weapon_element_attack(batk, ELE_NEUTRAL, IT_EQPI_HAND_L);
            batk = deduce_damage_size_modifier(batk, IT_EQPI_HAND_L);
            dmg.left_damage = batk;
        }
    } else {
        dmg.right_damage = (std::rand() % unit()->status()->creature_attack_damage()->get_min()) + (unit()->status()->creature_attack_damage()->get_max() - unit()->status()->creature_attack_damage()->get_min());
    }
    
    dmg.number_of_hits = 1;

    CombatRegistry::MeleeResultOperation::MeleeResultOperand *melee_operand = new CombatRegistry::MeleeResultOperation::MeleeResultOperand(unit(), target());
    CombatRegistry::CombatValueDamage *melee_value = new CombatRegistry::CombatValueDamage(dmg);
    CombatRegistry::MeleeResultOperation *melee_operation = new CombatRegistry::MeleeResultOperation(melee_operand, CombatRegistry::MeleeResultOperation::melee_result_operation_type::MELEE_RESULT_OPERATION_DAMAGE, melee_value);

    CombatRegistry::AttributeOperation::AttributeOperand *attr_operand = new CombatRegistry::AttributeOperation::AttributeOperand(unit(), target(), target()->status()->current_hp());
    CombatRegistry::CombatValueInteger *attr_value = new CombatRegistry::CombatValueInteger(dmg.left_damage + dmg.right_damage);
    CombatRegistry::AttributeOperation *attr_operation = new CombatRegistry::AttributeOperation(attr_operand, CombatRegistry::AttributeOperation::attribute_operation_type::ATTRIBUTE_OPERATION_SUBTRACT_FROM_BASE, attr_value);
    
    int time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	CombatRegistry::CombatStage *stage = new CombatRegistry::CombatStage(time);
    stage->add_operation(melee_operation);
    stage->add_operation(attr_operation);
    unit()->combat_registry()->queue_combat_stage(stage);

    return CBT_RET_NONE;
}

int64_t Combat::calculate_weapon_attack(int64_t damage)
{
    return damage;
}

int64_t Combat::calculate_magic_attack(int64_t damage)
{
    return damage;
}

int64_t Combat::calculate_misc_attack(int64_t damage)
{
    return damage;
}

int64_t Combat::deduce_weapon_element_attack(int64_t damage, element_type def_ele, item_equip_location_index loc)
{
    if (unit()->type() == UNIT_PLAYER) {
        EquipmentListType const &equipments = unit()->downcast<Horizon::Zone::Units::Player>()->inventory()->equipments();
        std::shared_ptr<const item_entry_data> weapon = nullptr;

        if (equipments[loc].second.expired())
            return damage;

        weapon = equipments[loc].second.lock();

        if (loc != IT_EQPI_HAND_R || loc != IT_EQPI_HAND_L)
            return damage; // not a weapon.

        std::shared_ptr<const item_config_data> weapond = ItemDB->get_item_by_id(weapon->item_id);

        if (weapond == nullptr) {
            HLog(warning) << "Combat::deduce_weapon_element_attack: could not find item config of right hand weapon ID: " << weapon->item_id << ". ignoring..." ;
            return damage;
        }

        int32_t attribute_damage_ratio = ItemDB->get_weapon_attribute_modifier(weapond->level.weapon, ELE_NEUTRAL, def_ele);

        damage += (attribute_damage_ratio < 100 ? -(damage * (attribute_damage_ratio - 100) / 100) : (damage * (attribute_damage_ratio - 100) / 100));
    }

    return damage;
}

int64_t Combat::deduce_damage_size_modifier(int64_t damage, item_equip_location_index loc)
{
    if (unit()->type() == UNIT_PLAYER) {
        EquipmentListType const &equipments = unit()->downcast<Horizon::Zone::Units::Player>()->inventory()->equipments();
        std::shared_ptr<const item_entry_data> weapon = nullptr;

        if (equipments[loc].second.expired())
            return damage;

        weapon = equipments[loc].second.lock();

        if (loc == IT_EQPI_HAND_R || loc == IT_EQPI_HAND_L) 
            return damage; // not a weapon.

        std::shared_ptr<const item_config_data> weapond = ItemDB->get_item_by_id(weapon->item_id);

        if (weapond == nullptr) {
            HLog(warning) << "Combat::deduce_weapon_element_attack: could not find item config of right hand weapon ID: " << weapon->item_id << ". ignoring..." ;
            return damage;
        }

        int32_t size_damage_ratio = ItemDB->get_weapon_target_size_modifier(weapond->sub_type.weapon_t, (unit_size_type) unit()->status()->size()->get_base());

        damage += (size_damage_ratio < 100 ? -(damage * (size_damage_ratio - 100) / 100) : (damage * (size_damage_ratio - 100) / 100));
    }

    return damage;
}

CombatRegistry::AttributeOperation *CombatRegistry::CombatStage::push_attribute_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, std::shared_ptr<Traits::Attribute> attribute, CombatRegistry::AttributeOperation::attribute_operation_type operation_type, int value)
{
    CombatRegistry::AttributeOperation::AttributeOperand *operand = new CombatRegistry::AttributeOperation::AttributeOperand(source, target, attribute);
    CombatRegistry::CombatValueInteger *combat_value = new CombatRegistry::CombatValueInteger(value);
    CombatRegistry::AttributeOperation *operation = new CombatRegistry::AttributeOperation(operand, operation_type, combat_value);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::StatusOperation *CombatRegistry::CombatStage::push_status_add_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config config)
{
    CombatRegistry::StatusOperation::StatusOperand *operand = new CombatRegistry::StatusOperation::StatusOperand(source, target, config);
    CombatRegistry::StatusOperation *operation = new CombatRegistry::StatusOperation(operand, CombatRegistry::StatusOperation::status_operation_type::STATUS_OPERATION_ADD);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::StatusOperation *CombatRegistry::CombatStage::push_status_remove_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config config)
{
    CombatRegistry::StatusOperation::StatusOperand *operand = new CombatRegistry::StatusOperation::StatusOperand(source, target, config);
    CombatRegistry::StatusOperation *operation = new CombatRegistry::StatusOperation(operand, CombatRegistry::StatusOperation::status_operation_type::STATUS_OPERATION_REMOVE);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::SkillExecutionOperation *CombatRegistry::CombatStage::push_skill_execution_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config config, CombatRegistry::SkillExecutionOperation::skill_execution_operation_type operation_type)
{
    CombatRegistry::SkillExecutionOperation::SkillExecutionOperand *operand = new CombatRegistry::SkillExecutionOperation::SkillExecutionOperand(source, target, config);
    CombatRegistry::SkillExecutionOperation *operation = new CombatRegistry::SkillExecutionOperation(operand, operation_type);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::SkillResultOperation *CombatRegistry::CombatStage::push_skill_result_damage_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config config, combat_damage value_config)
{
    CombatRegistry::SkillResultOperation::SkillResultOperand *operand = new CombatRegistry::SkillResultOperation::SkillResultOperand(source, target, config);
    CombatRegistry::CombatValueDamage *combat_value = new CombatRegistry::CombatValueDamage(value_config);
    CombatRegistry::SkillResultOperation *operation = new CombatRegistry::SkillResultOperation(operand, CombatRegistry::SkillResultOperation::skill_result_operation_type::SKILL_RESULT_OPERATION_DAMAGE, combat_value);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::SkillResultOperation *CombatRegistry::CombatStage::push_skill_result_healing_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config config, combat_healing value_config)
{
    CombatRegistry::SkillResultOperation::SkillResultOperand *operand = new CombatRegistry::SkillResultOperation::SkillResultOperand(source, target, config);
    CombatRegistry::CombatValueHealing *combat_value = new CombatRegistry::CombatValueHealing(value_config);
    CombatRegistry::SkillResultOperation *operation = new CombatRegistry::SkillResultOperation(operand, CombatRegistry::SkillResultOperation::skill_result_operation_type::SKILL_RESULT_OPERATION_HEALING, combat_value);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::MeleeExecutionOperation *CombatRegistry::CombatStage::push_melee_execution_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::s_melee_execution_operation_config config, CombatRegistry::MeleeExecutionOperation::melee_execution_operation_type operation_type)
{
    CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand *operand = new CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand(source, target, config);
    CombatRegistry::MeleeExecutionOperation *operation = new CombatRegistry::MeleeExecutionOperation(operand, operation_type);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::MeleeResultOperation *CombatRegistry::CombatStage::push_melee_result_damage_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, combat_damage value_config)
{
    CombatRegistry::MeleeResultOperation::MeleeResultOperand *operand = new CombatRegistry::MeleeResultOperation::MeleeResultOperand(source, target);
    CombatRegistry::CombatValueDamage *combat_value = new CombatRegistry::CombatValueDamage(value_config);
    CombatRegistry::MeleeResultOperation *operation = new CombatRegistry::MeleeResultOperation(operand, CombatRegistry::MeleeResultOperation::melee_result_operation_type::MELEE_RESULT_OPERATION_DAMAGE, combat_value);
    this->add_operation(operation);
    return operation;
}

CombatRegistry::MeleeResultOperation *CombatRegistry::CombatStage::push_melee_result_healing_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, combat_healing value_config)
{
    CombatRegistry::MeleeResultOperation::MeleeResultOperand *operand = new CombatRegistry::MeleeResultOperation::MeleeResultOperand(source, target);
    CombatRegistry::CombatValueHealing *combat_value = new CombatRegistry::CombatValueHealing(value_config);
    CombatRegistry::MeleeResultOperation *operation = new CombatRegistry::MeleeResultOperation(operand, CombatRegistry::MeleeResultOperation::melee_result_operation_type::MELEE_RESULT_OPERATION_HEALING, combat_value);
    this->add_operation(operation);
    return operation;
}

void CombatRegistry::AttributeOperation::execute() const
{
    CombatRegistry::AttributeOperation::AttributeOperand *operand = dynamic_cast<CombatRegistry::AttributeOperation::AttributeOperand *>(get_operand());
    CombatRegistry::CombatValueInteger *integer = dynamic_cast<CombatRegistry::CombatValueInteger*>(get_operation_value());
    int value = integer->get_value();
    switch (get_operation_sub_type())
    {
        case ATTRIBUTE_OPERATION_ADD_TO_BASE:
        {
            operand->get_attribute()->add_base(value);
        }
            break;
        case ATTRIBUTE_OPERATION_SUBTRACT_FROM_BASE:
        {
            operand->get_attribute()->sub_base(value);
        }
            break;
        case ATTRIBUTE_OPERATION_ADD_TO_EQUIP:
        {
            operand->get_attribute()->add_equip(value);
        }
            break;
        case ATTRIBUTE_OPERATION_SUBTRACT_FROM_EQUIP:
        {
            operand->get_attribute()->sub_equip(value);
        }
            break;
        case ATTRIBUTE_OPERATION_ADD_TO_STATUS:
        {
            operand->get_attribute()->add_status(value);
        }
            break;
        case ATTRIBUTE_OPERATION_SUBTRACT_FROM_STATUS:
        {
            operand->get_attribute()->sub_status(value);
        }
            break;
    }
}

void CombatRegistry::StatusOperation::execute() const
{
    CombatRegistry::StatusOperation::StatusOperand *operand = dynamic_cast<CombatRegistry::StatusOperation::StatusOperand *>(get_operand());
    CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config config = operand->get_config();
    std::shared_ptr<Unit> target = operand->get_target();

    if (get_operation_sub_type() == STATUS_OPERATION_REMOVE)
        target->status_effect_end(config.type);
    else
        target->status_effect_start(config.type, config.total_time, config.val1, config.val2, config.val3, config.val4);
}

void CombatRegistry::SkillExecutionOperation::execute() const
{
    CombatRegistry::SkillExecutionOperation::SkillExecutionOperand *operand = static_cast<CombatRegistry::SkillExecutionOperation::SkillExecutionOperand*>(get_operand());
    CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config &config = operand->get_config();
    std::shared_ptr<Unit> source = operand->get_source();
    std::shared_ptr<Unit> target = operand->get_target();

    switch(get_operation_sub_type())
    {
        case SKILL_EXECUTION_OPERATION_CAST:
        {
            s_unit_skill_use_notifier_config notifier_config;
            notifier_config.skill_id = config.skill_id;
            notifier_config.cast_time = config.cast_time;
            notifier_config.element = config.element;
            notifier_config.source_guid = source->guid();
            notifier_config.target_guid = target->guid();
            notifier_config.target_x = target->map_coords().x();
            notifier_config.target_y = target->map_coords().y();
            source->notify_nearby_players_of_skill_use(grid_unit_skill_use_notification_type::GRID_UNIT_SKILL_USE_NOTIFY_CASTTIME, notifier_config);
            
            if (source->is_walking())
                source->stop_walking(true, true);

            HLog(debug) << "Casting skill: " << config.skd->name << " on target: " << target->guid() << "." << std::endl;
            source->map()->container()->getScheduler().Schedule(
			    Milliseconds(config.cast_time), 
			    source->get_scheduler_task_id(UNIT_SCHEDULE_SKILL_CAST),
			    [this, config, target] (TaskContext context)
			    {
                    config.cast_end_function(config.skill_cast_data, config.skd);
                    HLog(debug) << "Cast complete for skill: " << config.skd->name << " on target: " << target->guid() << "." << std::endl;
                }
            );
        }
            break;
        case SKILL_EXECUTION_OPERATION_TARGET:
        {
            HLog(debug) << "Started skill execution on target: " << target->guid() << " with skill: " << config.skd->name << "." << std::endl;
            config.skill_execution->execute(target->guid());
            HLog(debug) << "Finished skill execution on target: " << target->guid() << " with skill: " << config.skd->name << "." << std::endl;
        }
            break;
        case SKILL_EXECUTION_OPERATION_GROUND:
        {
            if (config.contents.size() > 0)
                config.skill_execution->execute(config.pos_x, config.pos_y, config.contents);
            else
                config.skill_execution->execute(config.pos_x, config.pos_y);
        }
            break;
    }
}

void CombatRegistry::SkillResultOperation::execute() const
{
    std::shared_ptr<Unit> source = get_operand()->get_source();
    std::shared_ptr<Unit> target = get_operand()->get_target();

    switch(get_operation_sub_type())
    {
        case SKILL_RESULT_OPERATION_DAMAGE:
        {
            CombatRegistry::SkillResultOperation::SkillResultOperand *operand = dynamic_cast<CombatRegistry::SkillResultOperation::SkillResultOperand *>(get_operand());
            CombatRegistry::CombatValueDamage *damage = dynamic_cast<CombatRegistry::CombatValueDamage *>(get_operation_value());
            HLog(debug) << "Started skill result damage operation for skill: " << operand->get_config().skill_id << "." << std::endl;
            s_unit_skill_use_notifier_config notifier_config;
            notifier_config.skill_id = operand->get_config().skill_id;
            notifier_config.skill_lv = operand->get_config().skill_lv;
            notifier_config.source_guid = source->guid();
            notifier_config.target_guid = target->guid();
            notifier_config.start_time = get_sys_time();
            notifier_config.attack_motion = operand->get_config().attack_motion;
            notifier_config.delay_motion = operand->get_config().delay_motion;
            notifier_config.damage_value = damage->get_damage().left_damage + damage->get_damage().right_damage;
            notifier_config.number_of_hits = damage->get_damage().number_of_hits;
            notifier_config.action_type = operand->get_config().action_type;
            source->notify_nearby_players_of_skill_use(GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_DAMAGE, notifier_config);
            HLog(debug) << "Finished skill result damage operation for skill: " << operand->get_config().skill_id << "." << std::endl;
            target->on_damage_received(source, damage->get_damage().left_damage + damage->get_damage().right_damage);
        }
                break;
        case SKILL_RESULT_OPERATION_HEALING:
        {
            CombatRegistry::SkillResultOperation::SkillResultOperand *operand = dynamic_cast<CombatRegistry::SkillResultOperation::SkillResultOperand *>(get_operand());
            CombatRegistry::CombatValueHealing *healing = dynamic_cast<CombatRegistry::CombatValueHealing *>(get_operation_value());
            HLog(debug) << "Started skill result damage operation for skill: " << operand->get_config().skill_id << "." << std::endl;
            s_unit_skill_use_notifier_config notifier_config;
            notifier_config.skill_id = operand->get_config().skill_id;
            notifier_config.skill_lv = operand->get_config().skill_lv;
            notifier_config.source_guid = source->guid();
            notifier_config.display_value = healing->get_healing().heal_amount;
            notifier_config.target_guid = operand->get_target()->guid();
            source->notify_nearby_players_of_skill_use(GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_NO_DAMAGE, notifier_config);
            HLog(debug) << "Finished skill result damage operation for skill: " << operand->get_config().skill_id << "." << std::endl;
        }
            break;
        default:
            HLog(warning) << "CombatRegistry::SkillOperation::execute(): unknown skill operation type: " << get_operation_sub_type();
            break;
    }
}

void CombatRegistry::MeleeExecutionOperation::execute() const
{
    std::shared_ptr<Unit> source = get_operand()->get_source();
    std::shared_ptr<Unit> target = get_operand()->get_target();
    CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand *operand = dynamic_cast<CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand*>(get_operand());
    CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::s_melee_execution_operation_config config = operand->get_config();
    
    switch(get_operation_sub_type())
    {
        case MELEE_EXECUTION_OPERATION_TARGET:
        {
			source->attack(target, config.continuous);
        }
            break;
    }
}

void CombatRegistry::MeleeResultOperation::execute() const
{
    std::shared_ptr<Unit> source = get_operand()->get_source();
    std::shared_ptr<Unit> target = get_operand()->get_target();

    switch(get_operation_sub_type())
    {
        case MELEE_RESULT_OPERATION_DAMAGE:
        {
            CombatRegistry::CombatValueDamage *value = dynamic_cast<CombatRegistry::CombatValueDamage *>(get_operation_value());
            s_grid_unit_basic_attack_config config;
            config.guid = source->guid();
            config.target_guid = target->guid();
            config.start_time = get_sys_time();
            config.delay_skill = source->status()->attack_motion()->total();
            config.delay_damage = source->status()->attack_delay()->total();
            config.damage = value->get_damage().right_damage;
            config.is_sp_damaged = 0;
            config.number_of_hits = value->get_damage().number_of_hits;
            config.action_type = ZCNA3_DAMAGE;
            config.left_damage = value->get_damage().left_damage;
            source->notify_nearby_players_of_basic_attack(config);
            target->on_damage_received(source, value->get_damage().left_damage + value->get_damage().right_damage);
        }
            break;
        case MELEE_RESULT_OPERATION_HEALING:
        {
            // Could be a possibility?
        }
            break;
    }
}

void CombatRegistry::process_queue()
{
	while (!_combat_stage_queue.empty()) {
		CombatStage *combat_stage = _combat_stage_queue.top();
        HLog(debug) << "Started combat stage: " << (void *) combat_stage << std::endl;
        _combat_stage_queue.pop();
        while (combat_stage->has_operations()) {
            const CombatOperation *operation = combat_stage->get_next_operation();
            combat_stage->pop_queue();
            operation->execute();
            delete operation;
        }
        HLog(debug) << "Ended combat stage: " << (void *) combat_stage << std::endl;
        delete combat_stage;
	};
}