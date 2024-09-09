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

#ifndef HORIZON_ZONE_GAME_ENTITIES_BATTLE_COMBAT_HPP
#define HORIZON_ZONE_GAME_ENTITIES_BATTLE_COMBAT_HPP

#include "Server/Zone/Definitions/CombatDefinitions.hpp"
#include "Server/Zone/Game/Units/Unit.hpp"
#include "Core/Multithreading/ThreadSafeQueue.hpp"
#include "Server/Zone/Game/Units/Traits/Attribute.hpp"
#include "Server/Zone/Game/SkillSystem/SkillExecution.hpp"

namespace Horizon
{
namespace Zone
{
class CombatRegistry
{
public:
    enum combat_operation_value_type
    {
        COMBAT_OPERATION_VALUE_INTEGER,
        COMBAT_OPERATION_VALUE_DAMAGE,
        COMBAT_OPERATION_VALUE_HEALING,
        COMBAT_OPERATION_VALUE_STATUS,
        COMBAT_OPERATION_VALUE_NONE
    };
    
    class CombatOperationValue
    {
    public:
        CombatOperationValue() { }
        explicit CombatOperationValue(combat_operation_value_type type)
        : _type(type) { }
        virtual ~CombatOperationValue() = default;
        
        virtual combat_operation_value_type get_type() { return _type; }

        void operator=(const CombatOperationValue &value)
        {
            _type = value._type;
        }
    private:
        combat_operation_value_type _type;
    };

    class CombatValueInteger : public CombatOperationValue
    {
    public:
        CombatValueInteger() = delete;

        explicit CombatValueInteger(int value) 
        : CombatOperationValue(COMBAT_OPERATION_VALUE_INTEGER), _value(value) { }
        
        ~CombatValueInteger() { }

        int get_value() { return _value; }
    private:
        int _value{ 0 };
    };

    class CombatValueDamage : public CombatOperationValue
    {
    public:
        CombatValueDamage() = delete;
        explicit CombatValueDamage(combat_damage damage) 
        : CombatOperationValue(COMBAT_OPERATION_VALUE_DAMAGE), _damage(damage) { }
        ~CombatValueDamage() { }

        combat_damage get_damage() { return _damage; }

    private:
        combat_damage _damage{ 0 };
    };

    class CombatValueHealing : public CombatOperationValue
    {
    public:
        CombatValueHealing() = delete;
        explicit CombatValueHealing(combat_healing healing) 
        : CombatOperationValue(COMBAT_OPERATION_VALUE_HEALING), _healing(healing) { }

        ~CombatValueHealing() { }

        combat_healing get_healing() { return _healing; }

    private:
        combat_healing _healing{ 0 };
    };

    class CombatOperand
    {
    public:
        enum combat_operand_type
        {
            COMBAT_OPERAND_ATTRIBUTE = 0,
            COMBAT_OPERAND_APPEARANCE = 1,
            COMBAT_OPERAND_STATUS = 2,
            COMBAT_OPERAND_SKILL_EXECUTION = 3,
            COMBAT_OPERAND_MELEE_EXECUTION = 4,
            COMBAT_OPERAND_SKILL_RESULT = 5,
            COMBAT_OPERAND_MELEE_RESULT = 6
        };
        virtual ~CombatOperand() = default;

        virtual combat_operand_type get_type() { return _type; }

        std::shared_ptr<Unit> get_source() { return _source; }
        std::shared_ptr<Unit> get_target() { return _target; }

        void set_source(std::shared_ptr<Unit> source) { _source = source; }
        void set_target(std::shared_ptr<Unit> target) { _target = target; }

protected:
        CombatOperand() = delete;
        CombatOperand(CombatOperand &operand) = delete;
        CombatOperand(CombatOperand &&operand) = delete;
        
        explicit CombatOperand(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, combat_operand_type type) 
        : _source(source), _target(target), _type(type) { }

        void operator=(const CombatOperand &operand) = delete;
        void operator=(CombatOperand &&operand) = delete;

    private:
        combat_operand_type _type;
        std::shared_ptr<Unit> _source, _target;
    };

    class CombatOperation
    {
    public:
        enum combat_operation_type
        {
            COMBAT_OPERATION_ATTRIBUTE,
            COMBAT_OPERATION_APPEARANCE,
            COMBAT_OPERATION_STATUS,
            COMBAT_OPERATION_SKILL_EXECUTION,
            COMBAT_OPERATION_MELEE_EXECUTION,
            COMBAT_OPERATION_SKILL_RESULT,
            COMBAT_OPERATION_MELEE_RESULT,
            COMBAT_OPERATION_MAX
        };

        CombatOperation() = delete;
        CombatOperation(const CombatOperation &right) = delete;
        CombatOperation(CombatOperation &&right) = delete;

        explicit CombatOperation(CombatOperand *operand, int operation_type, int operation_sub_type, CombatOperationValue *value)
        : _operand(operand), _operation_type(operation_type), _operation_sub_type(operation_sub_type),  _operation_value(value) { }
        
        explicit CombatOperation(CombatOperand *operand, int operation_type, int operation_sub_type)
        : _operand(operand), _operation_type(operation_type), _operation_sub_type(operation_sub_type) { }

        virtual ~CombatOperation() 
        {
            if (_operand != nullptr) 
                delete _operand; 
            if (_operation_value != nullptr) 
                delete _operation_value; 
        }

        virtual CombatOperand *get_operand() const { return _operand; }
        int get_operation_type() const { return _operation_type; }
        int get_operation_sub_type() const { return _operation_sub_type; }
        virtual CombatOperationValue *get_operation_value() const { return _operation_value; }

        int get_priority() { return _priority; }
        void set_priority(int priority) { _priority = priority; }

        virtual void execute() const = 0;

        void operator=(const CombatOperation &operation) = delete;

    protected:
        CombatOperand *_operand{nullptr};
        int _operation_type{ 0 };
        int _operation_sub_type{ 0 };
        CombatOperationValue *_operation_value{nullptr};
        int _priority{ 0 };
    };

    class AttributeOperation : public CombatOperation
    {
    public:
        enum attribute_operation_type
        {
            ATTRIBUTE_OPERATION_ADD_TO_BASE = 0,
            ATTRIBUTE_OPERATION_SUBTRACT_FROM_BASE,
            ATTRIBUTE_OPERATION_ADD_TO_EQUIP,
            ATTRIBUTE_OPERATION_SUBTRACT_FROM_EQUIP,
            ATTRIBUTE_OPERATION_ADD_TO_STATUS,
            ATTRIBUTE_OPERATION_SUBTRACT_FROM_STATUS
        };
        class AttributeOperand : public CombatOperand
        {
        public:
            AttributeOperand() = delete;
            explicit AttributeOperand(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, std::shared_ptr<Traits::Attribute> attribute) 
            : _attribute(attribute), CombatOperand(source, target, COMBAT_OPERAND_ATTRIBUTE) { }
            ~AttributeOperand() { }

            std::shared_ptr<Traits::Attribute> get_attribute() { return _attribute; }
            
            void operator=(const AttributeOperand &operand)
            {
                _attribute = operand._attribute;
            }
        private:
            std::shared_ptr<Traits::Attribute> _attribute;
        };

        explicit AttributeOperation(AttributeOperand *operand, attribute_operation_type operation_type, CombatValueInteger *value)
        : CombatOperation(operand, COMBAT_OPERATION_ATTRIBUTE, (int) operation_type, value) { }
        ~AttributeOperation() { }

        void execute() const override;
    };

    class StatusOperation : public CombatOperation
    {
    public:
        enum status_operation_type
        {
            STATUS_OPERATION_ADD = 0,
            STATUS_OPERATION_REMOVE
        };

        class StatusOperand : public CombatOperand
        {
        public:
            struct s_combat_status_operation_config
            {
                int type, total_time, val1, val2, val3, val4;
            };
            StatusOperand() = delete;
            StatusOperand(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, s_combat_status_operation_config config)
            : CombatOperand(source, target, COMBAT_OPERAND_STATUS), _config(config) { }
            ~StatusOperand() { }

            s_combat_status_operation_config get_config() { return _config; }

            void operator=(const StatusOperand &operand)
            {
                _config = operand._config;
            }
        private:
            s_combat_status_operation_config _config;
        };

        StatusOperation() = delete;
        
        StatusOperation(StatusOperand *operand, status_operation_type operation_type)
        : CombatOperation(operand, COMBAT_OPERATION_STATUS, (int) operation_type) { }
        
        ~StatusOperation() { }

        void execute() const override;
    };

    class SkillExecutionOperation : public CombatOperation
    {
    public:
        enum skill_execution_operation_type
        {
            SKILL_EXECUTION_OPERATION_CAST = 0,
            SKILL_EXECUTION_OPERATION_TARGET = 1,
            SKILL_EXECUTION_OPERATION_GROUND
        };

        class SkillExecutionOperand : public CombatOperand
        {
        public:
            struct s_skill_execution_operation_config
            {
                int skill_id{ 0 }, skill_lv{ 0 };
                int16_t pos_x{ 0 }; 
                int16_t pos_y{ 0 };
                std::string contents{ "" };
                std::shared_ptr<const skill_config_data> skd{nullptr};
                std::shared_ptr<SkillExecution> skill_execution{nullptr};
                element_type element{ element_type::ELE_NEUTRAL };
                int cast_time;
                sol::function cast_end_function;
                sol::table skill_cast_data;

                void operator=(const s_skill_execution_operation_config &config)
                {
                    skill_id = config.skill_id;
                    skill_lv = config.skill_lv;
                    pos_x = config.pos_x;
                    pos_y = config.pos_y;
                    contents = config.contents;
                    skd = config.skd;
                    skill_execution = config.skill_execution;
                    element = config.element;
                    cast_time = config.cast_time;
                    cast_end_function = config.cast_end_function;
                    skill_cast_data = config.skill_cast_data;
                }
            };
            SkillExecutionOperand() = delete;
            SkillExecutionOperand(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, s_skill_execution_operation_config config)
            : CombatOperand(source, target, COMBAT_OPERAND_SKILL_EXECUTION), _config(config) { }
            ~SkillExecutionOperand() { }

            s_skill_execution_operation_config &get_config() { return _config; }

            void operator=(const SkillExecutionOperand &operand)
            {
                _config = operand._config;
            }
        private:
            s_skill_execution_operation_config _config;
        };
        SkillExecutionOperation() = delete;
        SkillExecutionOperation(SkillExecutionOperand *operand, skill_execution_operation_type type)
        : CombatOperation(operand, COMBAT_OPERATION_SKILL_EXECUTION, (int) type)
        {}

        ~SkillExecutionOperation() { }

        void execute() const override;
    };

    class SkillResultOperation : public CombatOperation
    {
    public:
        enum skill_result_operation_type
        {
            SKILL_RESULT_OPERATION_DAMAGE = 0,
            SKILL_RESULT_OPERATION_HEALING
        };

        class SkillResultOperand : public CombatOperand
        {
        public:
            struct s_skill_result_operation_config
            {
                int skill_id{ 0 }, skill_lv{ 0 };
                int16_t pos_x; 
                int16_t pos_y;
                std::string contents;
                std::shared_ptr<const skill_config_data> skd;
                int delay_motion;
                int attack_motion;
            	zc_notify_act_3_action_types action_type;
            };
            SkillResultOperand() = delete;
            SkillResultOperand(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, s_skill_result_operation_config config)
            : CombatOperand(source, target, COMBAT_OPERAND_SKILL_RESULT), _config(config) { }
            ~SkillResultOperand() { }

            s_skill_result_operation_config get_config() { return _config; }
        private:
            s_skill_result_operation_config _config;
        };

        SkillResultOperation() = delete;
        SkillResultOperation(SkillResultOperand *operand, skill_result_operation_type type, CombatValueDamage *value)
        : CombatOperation(operand, COMBAT_OPERATION_SKILL_RESULT, (int) type, value)
        {}

        SkillResultOperation(SkillResultOperand *operand, skill_result_operation_type type, CombatValueHealing *value)
        : CombatOperation(operand, COMBAT_OPERATION_SKILL_RESULT, (int) type, value)
        {}

        ~SkillResultOperation() { }

        void execute() const override;
    };

    class MeleeExecutionOperation : public CombatOperation
    {
    public:
        enum melee_execution_operation_type
        {
            MELEE_EXECUTION_OPERATION_TARGET = 0
        };

        class MeleeExecutionOperand : public CombatOperand
        {
        public:
            struct s_melee_execution_operation_config
            {
                bool continuous{ false };
            };
            MeleeExecutionOperand() = delete;
            MeleeExecutionOperand(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, s_melee_execution_operation_config config)
            : CombatOperand(source, target, COMBAT_OPERAND_MELEE_EXECUTION), _config(config) { }
            ~MeleeExecutionOperand() { }

            s_melee_execution_operation_config get_config() { return _config; }
        private:
            s_melee_execution_operation_config _config;
        };

        MeleeExecutionOperation(MeleeExecutionOperand *operand, melee_execution_operation_type type)
        : CombatOperation(operand, COMBAT_OPERATION_MELEE_EXECUTION, (int) type) { }

        ~MeleeExecutionOperation() { }

        void execute() const override;
    };

    class MeleeResultOperation : public CombatOperation
    {
    public:
        enum melee_result_operation_type
        {
            MELEE_RESULT_OPERATION_DAMAGE = 0,
            MELEE_RESULT_OPERATION_HEALING
        };

        class MeleeResultOperand : public CombatOperand
        {
        public:
            MeleeResultOperand() = delete;
            MeleeResultOperand(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target)
            : CombatOperand(source, target, COMBAT_OPERAND_MELEE_RESULT) { }
            ~MeleeResultOperand() { }
        };

        MeleeResultOperation() = delete;

        MeleeResultOperation(MeleeResultOperand *operand, melee_result_operation_type type, CombatValueDamage *value)
        : CombatOperation(operand, COMBAT_OPERATION_MELEE_RESULT, (int) type, value) { }

        MeleeResultOperation(MeleeResultOperand *operand, melee_result_operation_type type, CombatValueHealing *value)
        : CombatOperation(operand, COMBAT_OPERATION_MELEE_RESULT, (int) type, value) { }

        ~MeleeResultOperation() { }

        void execute() const override;
    };

    // Define a comparison function for the priority queue
    struct CompareCombatOperation {
        bool operator()(CombatOperation* op1, CombatOperation* op2) {
            return op1->get_priority() < op2->get_priority();
        }
    };

    class CombatStage
    {
    public:
        CombatStage(int priority) : _priority(priority) { }
        ~CombatStage() { }

        void add_operation(CombatOperation *operation) { _operation_queue.push(operation); }

        AttributeOperation *push_attribute_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, std::shared_ptr<Traits::Attribute> attribute, CombatRegistry::AttributeOperation::attribute_operation_type operation_type, int value);
        StatusOperation *push_status_add_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config config);
        StatusOperation *push_status_remove_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::StatusOperation::StatusOperand::s_combat_status_operation_config config);
        SkillExecutionOperation *push_skill_execution_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::SkillExecutionOperation::SkillExecutionOperand::s_skill_execution_operation_config config, CombatRegistry::SkillExecutionOperation::skill_execution_operation_type operation_type);
        SkillResultOperation *push_skill_result_damage_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config config, combat_damage value_config);
        SkillResultOperation *push_skill_result_healing_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::SkillResultOperation::SkillResultOperand::s_skill_result_operation_config config, combat_healing value_config);
        MeleeExecutionOperation *push_melee_execution_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, CombatRegistry::MeleeExecutionOperation::MeleeExecutionOperand::s_melee_execution_operation_config config, CombatRegistry::MeleeExecutionOperation::melee_execution_operation_type operation_type);
        MeleeResultOperation *push_melee_result_damage_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, combat_damage value_config);
        MeleeResultOperation *push_melee_result_healing_operation(std::shared_ptr<Unit> source, std::shared_ptr<Unit> target, combat_healing value_config);

        int get_priority() { return _priority; }
        void set_priority(int priority) { _priority = priority; }
        
        bool has_operations() { return !_operation_queue.empty(); }
        bool has_next_operation() { return has_operations(); }
        
        const CombatOperation *get_next_operation() { return _operation_queue.top(); }
        void pop_queue() { _operation_queue.pop(); }
    private:
        int _priority{ 0 };
        std::priority_queue<CombatOperation *, std::vector<CombatOperation *>, CompareCombatOperation> _operation_queue;
    };

    // Define a comparison function for the priority queue
    struct CompareCombatStage {
        bool operator()(CombatStage* op1, CombatStage* op2) {
            return op1->get_priority() < op2->get_priority();
        }
    };

    CombatRegistry(std::shared_ptr<Unit> unit);
    ~CombatRegistry();

    std::shared_ptr<Unit> get_unit() { return _unit.lock(); }

    CombatStage *create_combat_stage(int priority) { return new CombatStage(priority); }

    void queue_combat_stage(CombatStage *stage) { _combat_stage_queue.push(stage); }

    void process_queue();

private:
    std::weak_ptr<Unit> _unit;
    std::priority_queue<CombatStage *, std::vector<CombatStage *>, CompareCombatStage> _combat_stage_queue;
};
class Combat
{
public:
    explicit Combat(std::shared_ptr<Unit> unit, std::shared_ptr<Unit> target);
    ~Combat();

    std::shared_ptr<Unit> unit() const { return _unit.lock(); }
    std::shared_ptr<Unit> target() const { return _target.lock(); }

    combat_retaliate_type weapon_attack();
    int64_t calculate_weapon_attack(int64_t damage);
    int64_t calculate_magic_attack(int64_t damage);
    int64_t calculate_misc_attack(int64_t damage);
    int64_t deduce_weapon_element_attack(int64_t damage, element_type def_ele, item_equip_location_index loc);
    int64_t deduce_damage_size_modifier(int64_t damage, item_equip_location_index loc);
private:
    time_t _start_time{0};
    std::weak_ptr<Unit> _unit, _target;
};
}
}

#endif /* HORIZON_ZONE_GAME_ENTITIES_BATTLE_COMBAT_HPP */
