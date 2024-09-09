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

#include "GridNotifiers.hpp"

#include "Server/Zone/Definitions/NPCDefinitions.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Traits/Status.hpp"
#include "Server/Zone/Game/Units/NPC/NPC.hpp"
#include "Server/Zone/Game/Units/Mob/Companion/Pet.hpp"
#include "Server/Zone/Game/Units/Mob/Companion/Homunculus.hpp"
#include "Server/Zone/Game/Units/Mob/Companion/Mercenary.hpp"
#include "Server/Zone/Game/Units/Mob/Companion/Elemental.hpp"
#include "Server/Zone/Game/Units/Mob/Hostile/Monster.hpp"
#include "Server/Zone/Game/Units/Item/Item.hpp"
#include "Server/Zone/Game/Units/Skill/Skill.hpp"
#include "Server/Zone/Game/Map/Grid/GridRefManager.hpp"
#include "Server/Zone/Game/Map/Path/AStar.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone::Units;

void GridPlayerNotifier::notify(GridRefManager<Horizon::Zone::Units::Player> &m)
{
	using namespace Horizon::Zone::Units;

	std::shared_ptr<Player> pl = _unit.lock()->template downcast<Player>();

	for (typename GridRefManager<Player>::iterator iter = m.begin(); iter != typename GridRefManager<Player>::iterator(nullptr); ++iter) {
		if (iter->source() == nullptr)
			continue;

		switch (_type)
		{
			case GRID_NOTIFY_AREA_WOS:
				if (pl != nullptr && iter->source()->guid() == pl->guid())
					continue;
			default:
				break;
		}
		iter->source()->get_session()->transmit_buffer(_buf, _buf.active_length());
	}
}

void GridPlayerNotifier::Visit(GridRefManager<Player> &m) { notify(m); }
template<> void GridPlayerNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template<> void GridPlayerNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template<> void GridPlayerNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template<> void GridPlayerNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template<> void GridPlayerNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template<> void GridPlayerNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template<> void GridPlayerNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template<> void GridPlayerNotifier::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridViewPortUpdater::update(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (_unit.expired())
        return;

    std::shared_ptr<Player> pl = _unit.lock()->template downcast<Player>();

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr || iter->source()->guid() == pl->guid())
            continue;

        if (pl->get_session() == nullptr || pl->get_session()->clif() == nullptr)
            continue;


            std::shared_ptr<Horizon::Zone::Unit> vp_e = iter->source()->shared_from_this();
    
            if (pl->is_in_range_of(vp_e, MAX_VIEW_RANGE) && !vp_e->is_walking())
                pl->add_unit_to_viewport(vp_e);
            else if (!pl->is_in_range_of(vp_e, MAX_VIEW_RANGE))
                pl->remove_unit_from_viewport(vp_e, EVP_NOTIFY_OUT_OF_SIGHT);
    }
}

void GridViewPortUpdater::Visit(GridRefManager<Player> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<NPC> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<Elemental> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<Homunculus> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<Mercenary> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<Pet> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<Monster> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<Skill> &m) { update(m); }
void GridViewPortUpdater::Visit(GridRefManager<Item> &m) { update(m); }

template <class T>
void GridUnitExistenceNotifier::notify(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (!m.get_size())
        return;

    std::shared_ptr<Horizon::Zone::Unit> src_unit = _unit.lock();

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Player> tpl = iter->source()->template downcast<Player>();

        if (src_unit == nullptr || src_unit->guid() == tpl->guid())
            continue;

        if (tpl->get_session() == nullptr || tpl->get_session()->clif() == nullptr)
            continue;

        bool is_in_range = tpl->is_in_range_of(src_unit);

        if (_notif_type == EVP_NOTIFY_IN_SIGHT && is_in_range) {
            if (tpl->unit_is_in_viewport(src_unit))
                continue;
            // Target player realizes new unit in viewport.
            // Source unit doesn't need to realize target as update_viewport() is called when needed/
            tpl->add_unit_to_viewport(src_unit);
        } else if (_notif_type == EVP_NOTIFY_OUT_OF_SIGHT && !is_in_range) {
            if (!tpl->unit_is_in_viewport(src_unit))
                continue;
            
            tpl->remove_unit_from_viewport(src_unit, EVP_NOTIFY_OUT_OF_SIGHT);
        }
        else if (_notif_type > EVP_NOTIFY_OUT_OF_SIGHT) {
            if (!tpl->unit_is_in_viewport(src_unit))
                continue;

            tpl->remove_unit_from_viewport(src_unit, _notif_type);
        }
    }
}

void GridUnitExistenceNotifier::Visit(GridRefManager<Player> &m) { notify(m); }
template <> void GridUnitExistenceNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridUnitExistenceNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridUnitExistenceNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridUnitExistenceNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridUnitExistenceNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridUnitExistenceNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridUnitExistenceNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridUnitExistenceNotifier::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridUnitSpawnNotifier::notify(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (!m.get_size())
        return;

    std::shared_ptr<Horizon::Zone::Unit> src_unit = _unit.lock();

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Player> tpl = iter->source()->template downcast<Player>();

        if (src_unit == nullptr || src_unit->guid() == tpl->guid())
            continue;

        if (tpl->get_session() == nullptr || tpl->get_session()->clif() == nullptr)
            continue;

        tpl->spawn_unit_in_viewport(src_unit);
    }
}

void GridUnitSpawnNotifier::Visit(GridRefManager<Player> &m) { notify<Player>(m); }
template <> void GridUnitSpawnNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridUnitSpawnNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridUnitSpawnNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridUnitSpawnNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridUnitSpawnNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridUnitSpawnNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridUnitSpawnNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridUnitSpawnNotifier::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridUnitMovementNotifier::notify(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (m.get_size() == 0)
        return;

    std::shared_ptr<Horizon::Zone::Unit> src_unit = _unit.lock();

    if (src_unit == nullptr)
        return;

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); iter++) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Player> tpl = iter->source()->template downcast<Player>();

        if (src_unit->guid() == tpl->guid())
            continue;

        if (tpl->get_session() == nullptr || tpl->get_session()->clif() == nullptr)
            continue;

        if (_new_entry == true)
            tpl->realize_unit_movement_entry(src_unit);
        else
            tpl->realize_unit_movement(src_unit);
    }
}

void GridUnitMovementNotifier::Visit(GridRefManager<Player> &m) { notify(m); }

template <> void GridUnitMovementNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridUnitMovementNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridUnitMovementNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridUnitMovementNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridUnitMovementNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridUnitMovementNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridUnitMovementNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridUnitMovementNotifier::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridUnitSearcher::search(GridRefManager<T> &m)
{
    // Found check.
    if (!_result.expired())
        return;

    using namespace Horizon::Zone;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::weak_ptr<Unit> unit = iter->source()->shared_from_this();
        if (!unit.expired() && _predicate(unit)) {
            _result = unit;
            return;
        }
    }
}

std::shared_ptr<Horizon::Zone::Unit> GridUnitSearcher::get_result() const { return _result.lock(); }

void GridUnitSearcher::Visit(GridRefManager<Player> &m) { search<Player>(m); }
void GridUnitSearcher::Visit(GridRefManager<NPC> &m) { search<NPC>(m); }
void GridUnitSearcher::Visit(GridRefManager<Elemental> &m) { search<Elemental>(m); }
void GridUnitSearcher::Visit(GridRefManager<Homunculus> &m) { search<Homunculus>(m); }
void GridUnitSearcher::Visit(GridRefManager<Mercenary> &m) { search<Mercenary>(m); }
void GridUnitSearcher::Visit(GridRefManager<Pet> &m) { search<Pet>(m); }
void GridUnitSearcher::Visit(GridRefManager<Monster> &m) { search<Monster>(m); }
void GridUnitSearcher::Visit(GridRefManager<Skill> &m) { search<Skill>(m); }
void GridUnitSearcher::Visit(GridRefManager<Item> &m) { search<Item>(m); }

template <class T>
void GridMonsterActiveAIExecutor::perform(GridRefManager<T> &m)
{
    if (_player.expired())
        return;

    using namespace Horizon::Zone;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Monster> monster = iter->source()->template downcast<Monster>();

        if (monster == nullptr)
            continue;

        monster->behavior_active(_player.lock());
    }
}

template <> void GridMonsterActiveAIExecutor::Visit<Player>(GridRefManager<Player> &m);
template <> void GridMonsterActiveAIExecutor::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridMonsterActiveAIExecutor::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridMonsterActiveAIExecutor::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridMonsterActiveAIExecutor::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridMonsterActiveAIExecutor::Visit<Pet>(GridRefManager<Pet> &m);
void GridMonsterActiveAIExecutor::Visit(GridRefManager<Monster> &m) { perform<Monster>(m); }
template <> void GridMonsterActiveAIExecutor::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridMonsterActiveAIExecutor::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridMonsterAIActiveSearchTarget::search(GridRefManager<T> &m)
{
    if (_monster.expired())
        return;

    using namespace Horizon::Zone;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Unit> e = iter->source()->shared_from_this();
        std::shared_ptr<Monster> m = _monster.lock();

        if (m == nullptr || e == nullptr)
            continue;

        if (m->monster_config()->mode & MONSTER_MODE_MASK_TARGETWEAK && e->status()->base_level()->get_base() >= m->monster_config()->level - 5)
            return;

#ifdef ACTIVE_PATH_SEARCH
        // On official servers, monsters will only seek targets that are closer to walk to than their
        // search range. The search range is affected depending on if the monster is walking or not.
        // On some maps there can be a quite long path for just walking two cells in a direction and
        // the client does not support displaying walk paths that are longer than 14 cells, so this
        // option reduces position lag in such situation. But doing a complex search for every possible
        // target, might be CPU intensive.
        // Disable this to make monsters not do any path search when looking for a target (old behavior).
        AStar::CoordinateList wp = m->map()->get_pathfinder().findPath(m->map_coords(), e->map_coords());

        if (wp.size() == 0) 
            continue; // no walk path available.

        //Standing monsters use view range, walking monsters use chase range
        if ((m->is_walking() == false && wp.size() > m->monster_config()->view_range)
            || (m->is_walking() == true && wp.size() > m->monster_config()->chase_range))
            continue;
#endif
        m->set_target(e);

        break;
    }
}

void GridMonsterAIActiveSearchTarget::Visit(GridRefManager<Player> &m) { search<Player>(m); }
template <> void GridMonsterAIActiveSearchTarget::Visit<NPC>(GridRefManager<NPC> &m);
void GridMonsterAIActiveSearchTarget::Visit(GridRefManager<Elemental> &m) { search<Elemental>(m); }
void GridMonsterAIActiveSearchTarget::Visit(GridRefManager<Homunculus> &m) { search<Homunculus>(m); }
void GridMonsterAIActiveSearchTarget::Visit(GridRefManager<Mercenary> &m) { search<Mercenary>(m); }
template <> void GridMonsterAIActiveSearchTarget::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridMonsterAIActiveSearchTarget::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridMonsterAIActiveSearchTarget::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridMonsterAIActiveSearchTarget::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridMonsterAIChangeChaseTarget::search(GridRefManager<T> &m)
{
    if (_monster.expired())
        return;

    using namespace Horizon::Zone;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Unit> e = iter->source()->shared_from_this();
        std::shared_ptr<Monster> m = _monster.lock();

        if (m == nullptr || e == nullptr)
            continue;

        std::shared_ptr<AStar::CoordinateList> wp = m->path_to(e);
        
        if (wp->size() > m->monster_config()->attack_range)
            continue;

        m->set_target(e);

        break;
    }
}

void GridMonsterAIChangeChaseTarget::Visit(GridRefManager<Player> &m) { search<Player>(m); }
template <> void GridMonsterAIChangeChaseTarget::Visit<NPC>(GridRefManager<NPC> &m);
void GridMonsterAIChangeChaseTarget::Visit(GridRefManager<Elemental> &m) { search<Elemental>(m); }
void GridMonsterAIChangeChaseTarget::Visit(GridRefManager<Homunculus> &m) { search<Homunculus>(m); }
void GridMonsterAIChangeChaseTarget::Visit(GridRefManager<Mercenary> &m) { search<Mercenary>(m); }
template <> void GridMonsterAIChangeChaseTarget::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridMonsterAIChangeChaseTarget::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridMonsterAIChangeChaseTarget::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridMonsterAIChangeChaseTarget::Visit<Item>(GridRefManager<Item> &m);


template <class T>
void GridNPCTrigger::check_and_trigger(GridRefManager<T> &m)
{
    if (_source.expired())
        return;

    using namespace Horizon::Zone::Units;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        std::shared_ptr<NPC> npc = iter->source()->template downcast<NPC>();
        if (npc == nullptr)
            continue;


    	//@TODO npc check and trigger script for npc in range
        std::shared_ptr<npc_db_data> const &nd = sZone->get_component_of_type<Horizon::Zone::ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM)->npc()->get_npc_from_db(npc->guid());
        if (nd != nullptr && nd->trigger_range && _predicate(npc, nd->trigger_range)) {
            std::shared_ptr<Player> player = _source.lock()->downcast<Player>();
			sZone->get_component_of_type<Horizon::Zone::ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM)->npc()->contact_npc_for_player(player, npc->guid());
        }
    }
}

void GridNPCTrigger::Visit(GridRefManager<NPC> &m) { check_and_trigger(m); }
template<> void GridNPCTrigger::Visit<Player>(GridRefManager<Player> &m);
template<> void GridNPCTrigger::Visit<Elemental>(GridRefManager<Elemental> &m);
template<> void GridNPCTrigger::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template<> void GridNPCTrigger::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template<> void GridNPCTrigger::Visit<Pet>(GridRefManager<Pet> &m);
template<> void GridNPCTrigger::Visit<Monster>(GridRefManager<Monster> &m);
template<> void GridNPCTrigger::Visit<Skill>(GridRefManager<Skill> &m);
template<> void GridNPCTrigger::Visit<Item>(GridRefManager<Item> &m);

// Searches a skillarea for an unit that is within the splash range of the target.
// If found, the status change is applied to the unit.
template <class T>
void GridSCApplyInSkillArea::apply(GridRefManager<T> &m)
{
    if (_source.expired())
        return;

    using namespace Horizon::Zone::Units;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        std::shared_ptr<Horizon::Zone::Unit> unit = iter->source()->shared_from_this();
        
        if (unit == nullptr)
            continue;
        
        // AOE Target Type check.       
        AOETargetTypePredicate aoe_predicate(_aoe_config.aoe_target_mask);

        if (!aoe_predicate(unit))
            continue;

        RangeCheckPredicate range_predicate(unit);
        
        // If the unit is not in range of target's splash range, 
        // ignore.
        if (!range_predicate(_target, _aoe_config.aoe_range))
            continue;
        
        unit->status_effect_start(_sc_config.type, _sc_config.total_time, _sc_config.val1, _sc_config.val2, _sc_config.val3, _sc_config.val4);
    }
}

void GridSCApplyInSkillArea::Visit(GridRefManager<NPC> &m) { apply(m); }
void GridSCApplyInSkillArea::Visit(GridRefManager<Player> &m) { apply(m); }
void GridSCApplyInSkillArea::Visit(GridRefManager<Elemental> &m) { apply(m); }
void GridSCApplyInSkillArea::Visit(GridRefManager<Homunculus> &m) { apply(m); }
void GridSCApplyInSkillArea::Visit(GridRefManager<Mercenary> &m) { apply(m); }
void GridSCApplyInSkillArea::Visit(GridRefManager<Pet> &m) { apply(m); }
void GridSCApplyInSkillArea::Visit(GridRefManager<Monster> &m) { apply(m); }
template <> void GridSCApplyInSkillArea::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridSCApplyInSkillArea::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridSCRemoveInSkillArea::apply(GridRefManager<T> &m)
{
    if (_source.expired())
        return;

    using namespace Horizon::Zone::Units;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        std::shared_ptr<Horizon::Zone::Unit> unit = iter->source()->shared_from_this();
        
        if (unit == nullptr)
            continue;

        // AOE Target Type check.       
        AOETargetTypePredicate aoe_predicate(_aoe_config.aoe_target_mask);

        if (!aoe_predicate(unit))
            continue;

        // @Todo: check map type PvP, GvG, etc.
        RangeCheckPredicate range_predicate(unit);
        
        // If the unit is not in range of target's splash range, 
        // ignore.
        if (!range_predicate(_target, _aoe_config.aoe_range))
            continue;
        
        unit->status_effect_end(_sc_type);
    }
}

void GridSCRemoveInSkillArea::Visit(GridRefManager<NPC> &m) { apply(m); }
void GridSCRemoveInSkillArea::Visit(GridRefManager<Player> &m) { apply(m); }
void GridSCRemoveInSkillArea::Visit(GridRefManager<Elemental> &m) { apply(m); }
void GridSCRemoveInSkillArea::Visit(GridRefManager<Homunculus> &m) { apply(m); }
void GridSCRemoveInSkillArea::Visit(GridRefManager<Mercenary> &m) { apply(m); }
void GridSCRemoveInSkillArea::Visit(GridRefManager<Pet> &m) { apply(m); }
void GridSCRemoveInSkillArea::Visit(GridRefManager<Monster> &m) { apply(m); }
template <> void GridSCRemoveInSkillArea::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridSCRemoveInSkillArea::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridExecuteSkillInArea::apply(GridRefManager<T> &m)
{
    if (_initial_source.expired())
        return;

    using namespace Horizon::Zone::Units;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        std::shared_ptr<Horizon::Zone::Unit> unit = iter->source()->shared_from_this();
        
        if (unit == nullptr)
            continue;

        // AOE Target Type check.       
        AOETargetTypePredicate aoe_predicate(_aoe_config.aoe_target_mask);

        if (!aoe_predicate(unit))
            continue;

        // @Todo: check map type PvP, GvG, etc.
        RangeCheckPredicate range_predicate(unit);
        
        // If the unit is not in range of target's splash range, 
        // ignore.
        if (!range_predicate(_initial_source, _aoe_config.aoe_range))
            continue;
        
        _skill_execution->execute(unit->guid());
    }
}

void GridExecuteSkillInArea::Visit(GridRefManager<NPC> &m) { apply(m); }
void GridExecuteSkillInArea::Visit(GridRefManager<Player> &m) { apply(m); }
void GridExecuteSkillInArea::Visit(GridRefManager<Elemental> &m) { apply(m); }
void GridExecuteSkillInArea::Visit(GridRefManager<Homunculus> &m) { apply(m); }
void GridExecuteSkillInArea::Visit(GridRefManager<Mercenary> &m) { apply(m); }
void GridExecuteSkillInArea::Visit(GridRefManager<Pet> &m) { apply(m); }
void GridExecuteSkillInArea::Visit(GridRefManager<Monster> &m) { apply(m); }
template <> void GridExecuteSkillInArea::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridExecuteSkillInArea::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridExecuteSkillInCell::apply(GridRefManager<T> &m)
{
    if (_initial_source.expired())
        return;

    using namespace Horizon::Zone::Units;
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        std::shared_ptr<Horizon::Zone::Unit> unit = iter->source()->shared_from_this();
        
        if (unit == nullptr)
            continue;

        // AOE Target Type check.       
        AOETargetTypePredicate aoe_predicate(_aoe_config.aoe_target_mask);

        if (!aoe_predicate(unit))
            continue;
        
        CellCheckPredicate cell_predicate(_cell);

        if (!cell_predicate(unit->map_coords()))
            continue;

        _skill_execution->execute(unit->guid());
    }
}

void GridExecuteSkillInCell::Visit(GridRefManager<NPC> &m) { apply(m); }
void GridExecuteSkillInCell::Visit(GridRefManager<Player> &m) { apply(m); }
void GridExecuteSkillInCell::Visit(GridRefManager<Elemental> &m) { apply(m); }
void GridExecuteSkillInCell::Visit(GridRefManager<Homunculus> &m) { apply(m); }
void GridExecuteSkillInCell::Visit(GridRefManager<Mercenary> &m) { apply(m); }
void GridExecuteSkillInCell::Visit(GridRefManager<Pet> &m) { apply(m); }
void GridExecuteSkillInCell::Visit(GridRefManager<Monster> &m) { apply(m); }
template <> void GridExecuteSkillInCell::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridExecuteSkillInCell::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridUnitSkillUseNotifier::notify(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (!m.get_size())
        return;

    std::shared_ptr<Horizon::Zone::Unit> src_unit = _unit.lock();

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Player> tpl = iter->source()->template downcast<Player>();

        if (tpl->get_session() == nullptr || tpl->get_session()->clif() == nullptr)
            continue;

        switch (_notification_type)
        {
            case GRID_UNIT_SKILL_USE_NOTIFY_CASTTIME:
            {
                tpl->get_session()->clif()->notify_skill_cast(_config.skill_id, _config.source_guid, _config.target_guid, _config.target_x, _config.target_y, _config.element, _config.cast_time);
            }
                break;
            case GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_DAMAGE:
            {
                tpl->get_session()->clif()->notify_hostile_skill_use(
                    _config.skill_id, 
                    _config.source_guid, 
                    _config.target_guid, 
                    _config.start_time,
                    _config.attack_motion,
                    _config.delay_motion,
                    _config.damage_value, 
                    _config.display_value,
                    _config.number_of_hits,
                    _config.action_type);
            }
                break;
            case GRID_UNIT_SKILL_USE_NOTIFY_SUCCESS_NO_DAMAGE:
            {
                tpl->get_session()->clif()->notify_safe_skill_use(_config.skill_id, _config.display_value, _config.target_guid, ZC_USESKILL2_SUCCESS);
            }
                break;
            default:
                HLog(warning) << "GridUnitSkillUseNotifier::notify: Unknown notification type: " << _notification_type << "." << std::endl;
                break;
        }
    }
}

void GridUnitSkillUseNotifier::Visit(GridRefManager<Player> &m) { notify<Player>(m); }
template <> void GridUnitSkillUseNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridUnitSkillUseNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridUnitSkillUseNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridUnitSkillUseNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridUnitSkillUseNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridUnitSkillUseNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridUnitSkillUseNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridUnitSkillUseNotifier::Visit<Item>(GridRefManager<Item> &m);


template <class T>
void GridUnitBasicAttackNotifier::notify(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (!m.get_size())
        return;

    std::shared_ptr<Horizon::Zone::Unit> src_unit = _unit.lock();

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Player> tpl = iter->source()->template downcast<Player>();

        if (tpl->get_session() == nullptr || tpl->get_session()->clif() == nullptr)
            continue;

        tpl->get_session()->clif()->notify_damage(_config.guid, _config.target_guid, _config.start_time, _config.delay_skill,
            _config.delay_damage, _config.damage, _config.is_sp_damaged, _config.number_of_hits, _config.action_type, _config.left_damage);
    }
}

void GridUnitBasicAttackNotifier::Visit(GridRefManager<Player> &m) { notify<Player>(m); }
template <> void GridUnitBasicAttackNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridUnitBasicAttackNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridUnitBasicAttackNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridUnitBasicAttackNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridUnitBasicAttackNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridUnitBasicAttackNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridUnitBasicAttackNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridUnitBasicAttackNotifier::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridUnitMovementStopNotifier::notify(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (!m.get_size())
        return;

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Player> tpl = iter->source()->template downcast<Player>();

        if (tpl->get_session() == nullptr || tpl->get_session()->clif() == nullptr)
            continue;
        
        tpl->get_session()->clif()->notify_movement_stop(_unit_guid, _pos_x, _pos_y);
    }
}

void GridUnitMovementStopNotifier::Visit(GridRefManager<Player> &m) { notify<Player>(m); }
template <> void GridUnitMovementStopNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridUnitMovementStopNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridUnitMovementStopNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridUnitMovementStopNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridUnitMovementStopNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridUnitMovementStopNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridUnitMovementStopNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridUnitMovementStopNotifier::Visit<Item>(GridRefManager<Item> &m);

template <class T>
void GridUnitItemDropNotifier::notify(GridRefManager<T> &m)
{
    using namespace Horizon::Zone::Units;

    if (!m.get_size())
        return;

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != typename GridRefManager<T>::iterator(nullptr); ++iter) {
        if (iter->source() == nullptr)
            continue;

        std::shared_ptr<Player> tpl = iter->source()->template downcast<Player>();

        if (tpl->get_session() == nullptr || tpl->get_session()->clif() == nullptr)
            continue;
        
        tpl->get_session()->clif()->notify_item_drop(_entry.guid, 
            _entry.item_id, _entry.type, _entry.is_identified, _entry.x, _entry.y, _entry.x_area, _entry.y_area,
            _entry.amount, _entry.show_drop_effect, _entry.drop_effect_mode);
    }
}

void GridUnitItemDropNotifier::Visit(GridRefManager<Player> &m) { notify<Player>(m); }
template <> void GridUnitItemDropNotifier::Visit<NPC>(GridRefManager<NPC> &m);
template <> void GridUnitItemDropNotifier::Visit<Elemental>(GridRefManager<Elemental> &m);
template <> void GridUnitItemDropNotifier::Visit<Homunculus>(GridRefManager<Homunculus> &m);
template <> void GridUnitItemDropNotifier::Visit<Mercenary>(GridRefManager<Mercenary> &m);
template <> void GridUnitItemDropNotifier::Visit<Pet>(GridRefManager<Pet> &m);
template <> void GridUnitItemDropNotifier::Visit<Monster>(GridRefManager<Monster> &m);
template <> void GridUnitItemDropNotifier::Visit<Skill>(GridRefManager<Skill> &m);
template <> void GridUnitItemDropNotifier::Visit<Item>(GridRefManager<Item> &m);
