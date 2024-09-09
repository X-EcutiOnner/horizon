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

#ifndef HORIZON_ZONE_GAME_GAMELOGICPROCESS_HPP
#define HORIZON_ZONE_GAME_GAMELOGICPROCESS_HPP

#include "Server/Common/System.hpp"
#include "Utility/TaskScheduler.hpp"
#include "Libraries/MapCache/MapCache.hpp"
#include "Libraries/Networking/Buffer/ByteBuffer.hpp"
#include "Server/Zone/Zone.hpp"
namespace Horizon
{
namespace Zone
{
	namespace Units
	{
		class Monster;
	}
	class Map;
class GameLogicProcess : public KernelComponent
{
	
class MonsterSpawnAgent
{
public:
	~MonsterSpawnAgent();

	void initialize(std::shared_ptr<GameLogicProcess> game_logic_process)
	{
		_game_logic_process = game_logic_process;
	}
	
	void finalize();

	std::shared_ptr<GameLogicProcess> get_container() { return _game_logic_process.lock(); }

    void spawn_monsters(std::string map_name);

	void reschedule_single_monster_spawn(std::shared_ptr<Horizon::Zone::Units::Monster> monster);
	
	void spawn_monster_internal(std::shared_ptr<Map> map, int spawn_dataset_id, int monster_id, int16_t amount, int16_t x, int16_t y, int16_t x_area, int16_t y_area);
	void despawn_monsters(std::string map_name);
    
	void register_monster_spawn_info(uint32_t id, std::shared_ptr<monster_spawn_data> data) { _monster_spawn_db.insert(id, data); }
    std::shared_ptr<monster_spawn_data> get_monster_spawn_info(uint32_t id) { return _monster_spawn_db.at(id); }
	void clear_monster_spawn_info() { _monster_spawn_db.clear(); }
	
    int32_t _last_monster_spawn_id{0};
private:
	std::weak_ptr<GameLogicProcess> _game_logic_process;
    LockedLookupTable<uint32_t, std::shared_ptr<monster_spawn_data>> _monster_spawn_db;
};

public:
	GameLogicProcess();
	
    void initialize(int segment_number = 1) override;
    void finalize() override;
	void update(uint64_t diff);

	bool is_initialized() override { return _is_initialized.load(); }
	bool is_finalized() override { return _is_finalized.load(); }

	/* Map / Maps */
	bool load_map_cache();
	void start_internal();
	
	TaskScheduler &getScheduler() { return _scheduler; }

	void on_map_update(int64_t diff);
	
	MonsterSpawnAgent &get_monster_spawn_agent() { return _monster_spawn_agent; }

protected:
    std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};
	
	using PrimaryResource = SharedPriorityResourceMedium<s_segment_storage<std::string /* Map Name */, std::shared_ptr<Map>>>;
	using SecondaryResource = SharedPriorityResourceMedium<s_segment_storage<uint64_t, std::shared_ptr<Units::Player>>>;
	using TertiaryResource = SharedPriorityResourceMedium<s_segment_storage<uint64_t, std::shared_ptr<Unit>>>;
	using ResourceManager = SharedPriorityResourceManager<PrimaryResource, SecondaryResource, TertiaryResource>;
	ResourceManager _resource_manager;
public:
	ResourceManager &get_resource_manager() { return _resource_manager; }

private:
	MonsterSpawnAgent _monster_spawn_agent;
	TaskScheduler _scheduler;
	std::thread _thread;
	std::map<std::string, map_data> _maps; // Temporary map cache asset holder until maps are loaded.
};
}
}

#endif /* HORIZON_ZONE_GAME_GAMELOGICPROCESS_HPP */