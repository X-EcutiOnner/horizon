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

#ifndef HORIZON_ZONE_SCRIPTMANAGER
#define HORIZON_ZONE_SCRIPTMANAGER

#include "Server/Zone/Definitions/NPCDefinitions.hpp"
#include "Server/Zone/Definitions/MonsterDefinitions.hpp"

#include "Utility/TaskScheduler.hpp"

#include "Server/Zone/Script/Components/StatusEffectComponent.hpp"
#include "Server/Zone/Script/Components/SkillComponent.hpp"
#include "Server/Zone/Script/Components/CombatComponent.hpp"
#include "Server/Zone/Script/Components/UnitComponent.hpp"
#include "Server/Zone/Script/Components/ItemComponent.hpp"
#include "Server/Zone/Script/Components/MapComponent.hpp"
#include "Server/Zone/Script/Components/MonsterComponent.hpp"
#include "Server/Zone/Script/Components/NPCComponent.hpp"
#include "Server/Zone/Script/Components/PlayerComponent.hpp"
#include "Server/Common/Server.hpp"

namespace Horizon
{
namespace Zone
{
namespace Units
{
	class NPC;
	class Player;
	class Monster;
}
class ScriptManager : public KernelComponent
{
public:
	ScriptManager();
	~ScriptManager();

	void initialize_basic_state(std::shared_ptr<sol::state> state);
	void initialize_player_state(std::shared_ptr<sol::state> state);
	void initialize_npc_state(std::shared_ptr<sol::state> state);
	void initialize_monster_state(std::shared_ptr<sol::state> state);

	std::shared_ptr<PlayerComponent> player() { return _player_component; }
	std::shared_ptr<NPCComponent> npc() { return _npc_component; }
	std::shared_ptr<MonsterComponent> monster() { return _monster_component; }
	std::shared_ptr<MapComponent> map() { return _map_component; }
	std::shared_ptr<ItemComponent> item() { return _item_component; }
	std::shared_ptr<UnitComponent> unit() { return _unit_component; }
	std::shared_ptr<SkillComponent> skill() { return _skill_component; }
	std::shared_ptr<StatusEffectComponent> status_effect() { return _status_effect_component; }
	std::shared_ptr<CombatComponent> combat() { return _combat_component; }

	std::shared_ptr<sol::state> lua_state() { return _lua_state; }

	bool is_initialized() override { return _is_initialized.load(); }
	bool is_finalized() override { return _is_finalized.load(); }

	void initialize(int segment_number = 1) override;
	void finalize() override;
	void prepare_lua_state(std::shared_ptr<sol::state> &lua);

	void start();
	void update(uint64_t diff);

	TaskScheduler &getScheduler() { return _scheduler; }
	
private:
	void load_constants();
	void load_scripts();
	void load_scripts_internal();

	std::vector<std::string> _script_files;
	std::shared_ptr<sol::state> _lua_state;

	std::shared_ptr<PlayerComponent> _player_component;
	std::shared_ptr<NPCComponent> _npc_component;
	std::shared_ptr<MonsterComponent> _monster_component;
	std::shared_ptr<MapComponent> _map_component;
	std::shared_ptr<ItemComponent> _item_component;
	std::shared_ptr<UnitComponent> _unit_component;
	std::shared_ptr<SkillComponent> _skill_component;
	std::shared_ptr<StatusEffectComponent> _status_effect_component;
	std::shared_ptr<CombatComponent> _combat_component;

	std::thread _thread;
protected:
	std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};

	using PrimaryResource = SharedPriorityResourceMedium<s_segment_storage<uint64_t /*NPC GUID*/, std::shared_ptr<Units::NPC>>>;
	using ResourceManager = SharedPriorityResourceManager<PrimaryResource>;
	ResourceManager _resource_manager;
	TaskScheduler _scheduler;
public:
	ResourceManager &get_resource_manager() { return _resource_manager; }
};
}
}

#endif /* HORIZON_ZONE_SCRIPTMANAGER */
