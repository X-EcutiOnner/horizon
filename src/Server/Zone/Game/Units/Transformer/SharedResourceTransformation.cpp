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

#include "SharedResourceTransformation.hpp"

#include "Server/Zone/Game/Units/Mob/Companion/Pet.hpp"
#include "Server/Zone/Game/Units/Mob/Companion/Mercenary.hpp"
#include "Server/Zone/Game/Units/Mob/Companion/Elemental.hpp"
#include "Server/Zone/Game/Units/Mob/Companion/Homunculus.hpp"
#include "Server/Zone/Game/Units/Mob/Hostile/Monster.hpp"
#include "Server/Zone/Game/Units/Item/Item.hpp"
#include "Server/Zone/Game/Units/Mob/Mob.hpp"
#include "Server/Zone/Game/Units/NPC/NPC.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Game/Units/Skill/Skill.hpp"
#include "Server/Zone/Game/Units/Unit.hpp"

using namespace Horizon::Zone;
using namespace Horizon::Zone::Game;

template <typename T> SharedResourceTransformation<T>::SharedResourceTransformation(std::shared_ptr<T> before_resource, std::shared_ptr<T> after_resource) 
: _before_resource(before_resource), _after_resource(after_resource) 
{ }

template <typename T> SharedResourceTransformation<T>::SharedResourceTransformation(std::shared_ptr<T> before_resource) 
: _before_resource(before_resource) 
{ }

template <typename T>
PreparatoryTo<T>::PreparatoryTo(std::shared_ptr<T> before_resource) 
: SharedResourceTransformation<T>(before_resource) 
{ }

template <typename T>
SubsequentTo<T>::SubsequentTo(std::shared_ptr<T> before_resource, std::shared_ptr<T> after_resource) 
: SharedResourceTransformation<T>(before_resource, after_resource) 
{ }

template <typename T>
TransformResource<T>::TransformResource(PreparatoryTo<T> prep_to, SubsequentTo<T> sub_to) 
: _prep_to(prep_to), _sub_to(sub_to)
{

}

PreparatoryToTransformUnit::PreparatoryToTransformUnit(std::shared_ptr<Unit> unit) : PreparatoryTo<Unit>(unit)
{

}

bool PreparatoryToTransformUnit::prepare()
{
    
    return true;
}

bool PreparatoryToTransformUnit::validate()
{
    return true;
}

bool PreparatoryToTransformUnit::transform()
{
    return true;
}

SubsequentToTransformUnit::SubsequentToTransformUnit(std::shared_ptr<Unit> unit) : SubsequentTo<Unit>(unit, nullptr)
{

}

bool SubsequentToTransformUnit::prepare()
{
    return true;
}

bool SubsequentToTransformUnit::validate()
{
    return true;
}

bool SubsequentToTransformUnit::response()
{
    return true;
}

TransformUnit::TransformUnit(std::shared_ptr<Horizon::Zone::Unit> unit) 
: TransformResource(PreparatoryToTransformUnit(unit), SubsequentToTransformUnit(unit))
{

}

bool TransformUnit::prepare()
{
    return true;
}

bool TransformUnit::validate()
{
    return true;
}

bool TransformUnit::transform()
{
    return true;
}

PreparatoryToTransformPlayer::PreparatoryToTransformPlayer(std::shared_ptr<Horizon::Zone::Units::Player> player) : PreparatoryToTransformUnit(std::dynamic_pointer_cast<Unit>(player))
{

}

bool PreparatoryToTransformPlayer::prepare()
{
    return true;
}

bool PreparatoryToTransformPlayer::validate()
{
    return true;
}

bool PreparatoryToTransformPlayer::transform()
{
    return true;
}

SubsequentToTransformPlayer::SubsequentToTransformPlayer(std::shared_ptr<Horizon::Zone::Units::Player> player) : SubsequentToTransformUnit(std::dynamic_pointer_cast<Unit>(player))
{

}

bool SubsequentToTransformPlayer::prepare()
{
    return true;
}

bool SubsequentToTransformPlayer::validate()
{
    return true;
}

bool SubsequentToTransformPlayer::response()
{
    return true;
}

TransformPlayer::TransformPlayer(std::shared_ptr<Horizon::Zone::Units::Player> player) : TransformUnit(player)
{

}

bool TransformPlayer::prepare()
{
    return true;
}

bool TransformPlayer::validate()
{
    return true;
}

bool TransformPlayer::transform()
{
    return true;
}

// NPC



PreparatoryToTransformNPC::PreparatoryToTransformNPC(std::shared_ptr<Horizon::Zone::Units::NPC> npc) : PreparatoryToTransformUnit(std::dynamic_pointer_cast<Unit>(npc))
{

}

bool PreparatoryToTransformNPC::prepare()
{
    return true;
}

bool PreparatoryToTransformNPC::validate()
{
    return true;
}

bool PreparatoryToTransformNPC::transform()
{
    return true;
}

SubsequentToTransformNPC::SubsequentToTransformNPC(std::shared_ptr<Horizon::Zone::Units::NPC> npc) : SubsequentToTransformUnit(std::dynamic_pointer_cast<Unit>(npc))
{

}

bool SubsequentToTransformNPC::prepare()
{
    return true;
}

bool SubsequentToTransformNPC::validate()
{
    return true;
}

bool SubsequentToTransformNPC::response()
{
    return true;
}

TransformNPC::TransformNPC(std::shared_ptr<Horizon::Zone::Units::NPC> npc) : TransformUnit(npc)
{

}

bool TransformNPC::prepare()
{
    return true;
}

bool TransformNPC::validate()
{
    return true;
}

bool TransformNPC::transform()
{
    return true;
}

// Item

PreparatoryToTransformItem::PreparatoryToTransformItem(std::shared_ptr<Horizon::Zone::Units::Item> item) : PreparatoryToTransformUnit(std::dynamic_pointer_cast<Unit>(item))
{

}

bool PreparatoryToTransformItem::prepare()
{
    return true;
}

bool PreparatoryToTransformItem::validate()
{
    return true;
}

bool PreparatoryToTransformItem::transform()
{
    return true;
}

SubsequentToTransformItem::SubsequentToTransformItem(std::shared_ptr<Horizon::Zone::Units::Item> item) : SubsequentToTransformUnit(std::dynamic_pointer_cast<Unit>(item))
{

}

bool SubsequentToTransformItem::prepare()
{
    return true;
}

bool SubsequentToTransformItem::validate()
{
    return true;
}

bool SubsequentToTransformItem::response()
{
    return true;
}

TransformItem::TransformItem(std::shared_ptr<Horizon::Zone::Units::Item> item) : TransformUnit(item)
{

}

bool TransformItem::prepare()
{
    return true;
}

bool TransformItem::validate()
{
    return true;
}

bool TransformItem::transform()
{
    return true;
}

// Skill

PreparatoryToTransformSkill::PreparatoryToTransformSkill(std::shared_ptr<Horizon::Zone::Units::Skill> skill) : PreparatoryToTransformUnit(std::dynamic_pointer_cast<Unit>(skill))
{

}

bool PreparatoryToTransformSkill::prepare()
{
    return true;
}

bool PreparatoryToTransformSkill::validate()
{
    return true;
}

bool PreparatoryToTransformSkill::transform()
{
    return true;
}

SubsequentToTransformSkill::SubsequentToTransformSkill(std::shared_ptr<Horizon::Zone::Units::Skill> skill) : SubsequentToTransformUnit(std::dynamic_pointer_cast<Unit>(skill))
{

}

bool SubsequentToTransformSkill::prepare()
{
    return true;
}

bool SubsequentToTransformSkill::validate()
{
    return true;
}

bool SubsequentToTransformSkill::response()
{
    return true;
}

TransformSkill::TransformSkill(std::shared_ptr<Horizon::Zone::Units::Skill> skill) : TransformUnit(skill)
{

}

bool TransformSkill::prepare()
{
    return true;
}

bool TransformSkill::validate()
{
    return true;
}

bool TransformSkill::transform()
{
    return true;
}

// Mob


PreparatoryToTransformMob::PreparatoryToTransformMob(std::shared_ptr<Horizon::Zone::Units::Mob> mob) : PreparatoryToTransformUnit(std::dynamic_pointer_cast<Unit>(mob))
{

}

bool PreparatoryToTransformMob::prepare()
{
    return true;
}

bool PreparatoryToTransformMob::validate()
{
    return true;
}

bool PreparatoryToTransformMob::transform()
{
    return true;
}

SubsequentToTransformMob::SubsequentToTransformMob(std::shared_ptr<Horizon::Zone::Units::Mob> mob) : SubsequentToTransformUnit(std::dynamic_pointer_cast<Unit>(mob))
{

}

bool SubsequentToTransformMob::prepare()
{
    return true;
}

bool SubsequentToTransformMob::validate()
{
    return true;
}

bool SubsequentToTransformMob::response()
{
    return true;
}

TransformMob::TransformMob(std::shared_ptr<Horizon::Zone::Units::Mob> mob) : TransformUnit(mob)
{

}

bool TransformMob::prepare()
{
    return true;
}

bool TransformMob::validate()
{
    return true;
}

bool TransformMob::transform()
{
    return true;
}

// Monster


PreparatoryToTransformMonster::PreparatoryToTransformMonster(std::shared_ptr<Horizon::Zone::Units::Monster> monster) : PreparatoryToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(monster))
{

}

bool PreparatoryToTransformMonster::prepare()
{
    return true;
}

bool PreparatoryToTransformMonster::validate()
{
    return true;
}

bool PreparatoryToTransformMonster::transform()
{
    return true;
}

SubsequentToTransformMonster::SubsequentToTransformMonster(std::shared_ptr<Horizon::Zone::Units::Monster> monster) : SubsequentToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(monster))
{

}

bool SubsequentToTransformMonster::prepare()
{
    return true;
}

bool SubsequentToTransformMonster::validate()
{
    return true;
}

bool SubsequentToTransformMonster::response()
{
    return true;
}

TransformMonster::TransformMonster(std::shared_ptr<Horizon::Zone::Units::Monster> monster) : TransformMob(monster)
{

}

bool TransformMonster::prepare()
{
    return true;
}

bool TransformMonster::validate()
{
    return true;
}

bool TransformMonster::transform()
{
    return true;
}

// Elemental

PreparatoryToTransformElemental::PreparatoryToTransformElemental(std::shared_ptr<Horizon::Zone::Units::Elemental> elemental) : PreparatoryToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(elemental))
{

}

bool PreparatoryToTransformElemental::prepare()
{
    return true;
}

bool PreparatoryToTransformElemental::validate()
{
    return true;
}

bool PreparatoryToTransformElemental::transform()
{
    return true;
}

SubsequentToTransformElemental::SubsequentToTransformElemental(std::shared_ptr<Horizon::Zone::Units::Elemental> elemental) : SubsequentToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(elemental))
{

}

bool SubsequentToTransformElemental::prepare()
{
    return true;
}

bool SubsequentToTransformElemental::validate()
{
    return true;
}

bool SubsequentToTransformElemental::response()
{
    return true;
}

TransformElemental::TransformElemental(std::shared_ptr<Horizon::Zone::Units::Elemental> elemental) : TransformMob(elemental)
{

}

bool TransformElemental::prepare()
{
    return true;
}

bool TransformElemental::validate()
{
    return true;
}

bool TransformElemental::transform()
{
    return true;
}

// Homunculus

PreparatoryToTransformHomunculus::PreparatoryToTransformHomunculus(std::shared_ptr<Horizon::Zone::Units::Homunculus> homunculus) : PreparatoryToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(homunculus))
{

}

bool PreparatoryToTransformHomunculus::prepare()
{
    return true;
}

bool PreparatoryToTransformHomunculus::validate()
{
    return true;
}

bool PreparatoryToTransformHomunculus::transform()
{
    return true;
}

SubsequentToTransformHomunculus::SubsequentToTransformHomunculus(std::shared_ptr<Horizon::Zone::Units::Homunculus> homunculus) : SubsequentToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(homunculus))
{

}

bool SubsequentToTransformHomunculus::prepare()
{
    return true;
}

bool SubsequentToTransformHomunculus::validate()
{
    return true;
}

bool SubsequentToTransformHomunculus::response()
{
    return true;
}

TransformHomunculus::TransformHomunculus(std::shared_ptr<Horizon::Zone::Units::Homunculus> homunculus) 
: TransformMob(homunculus)
{

}

bool TransformHomunculus::prepare()
{
    return true;
}

bool TransformHomunculus::validate()
{
    return true;
}

bool TransformHomunculus::transform()
{
    return true;
}

// Mercenary

PreparatoryToTransformMercenary::PreparatoryToTransformMercenary(std::shared_ptr<Horizon::Zone::Units::Mercenary> mercenary) : PreparatoryToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(mercenary))
{

}

bool PreparatoryToTransformMercenary::prepare()
{
    return true;
}

bool PreparatoryToTransformMercenary::validate()
{
    return true;
}

bool PreparatoryToTransformMercenary::transform()
{
    return true;
}

SubsequentToTransformMercenary::SubsequentToTransformMercenary(std::shared_ptr<Horizon::Zone::Units::Mercenary> mercenary) : SubsequentToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(mercenary))
{

}

bool SubsequentToTransformMercenary::prepare()
{
    return true;
}

bool SubsequentToTransformMercenary::validate()
{
    return true;
}

bool SubsequentToTransformMercenary::response()
{
    return true;
}

TransformMercenary::TransformMercenary(std::shared_ptr<Horizon::Zone::Units::Mercenary> mercenary) 
: TransformMob(mercenary)
{

}

bool TransformMercenary::prepare()
{
    return true;
}

bool TransformMercenary::validate()
{
    return true;
}

bool TransformMercenary::transform()
{
    return true;
}

// Pet


PreparatoryToTransformPet::PreparatoryToTransformPet(std::shared_ptr<Horizon::Zone::Units::Pet> pet) : PreparatoryToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(pet))
{

}

bool PreparatoryToTransformPet::prepare()
{
    return true;
}

bool PreparatoryToTransformPet::validate()
{
    return true;
}

bool PreparatoryToTransformPet::transform()
{
    return true;
}

SubsequentToTransformPet::SubsequentToTransformPet(std::shared_ptr<Horizon::Zone::Units::Pet> pet) : SubsequentToTransformMob(std::dynamic_pointer_cast<Horizon::Zone::Units::Mob>(pet))
{

}

bool SubsequentToTransformPet::prepare()
{
    return true;
}

bool SubsequentToTransformPet::validate()
{
    return true;
}

bool SubsequentToTransformPet::response()
{
    return true;
}

TransformPet::TransformPet(std::shared_ptr<Horizon::Zone::Units::Pet> pet) : TransformMob(pet)
{

}

bool TransformPet::prepare()
{
    return true;
}

bool TransformPet::validate()
{
    return true;
}

bool TransformPet::transform()
{
    return true;
}
