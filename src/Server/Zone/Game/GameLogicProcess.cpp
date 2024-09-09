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
 * Base Author - Sxyz <sagunxp@gmail.com>
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

#include "GameLogicProcess.hpp"

#include "Server/Zone/Game/StaticDB/ExpDB.hpp"
#include "Server/Zone/Game/StaticDB/JobDB.hpp"
#include "Server/Zone/Game/StaticDB/ItemDB.hpp"
#include "Server/Zone/Game/StaticDB/MonsterDB.hpp"
#include "Server/Zone/Game/StaticDB/SkillDB.hpp"
#include "Server/Zone/Game/StaticDB/StatusEffectDB.hpp"
#include "Server/Zone/Game/StaticDB/StorageDB.hpp"

#include "Server/Zone/Game/Units/Mob/Hostile/Monster.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/Units/Player/Player.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Zone.hpp"

#if WIN32
	#include <windows.h>
#elif __linux__
	#include <sched.h>
#endif

using namespace Horizon::Zone;

static std::atomic<bool> static_db_loaded = false;

GameLogicProcess::GameLogicProcess()
: KernelComponent(sZone, Horizon::System::RUNTIME_GAMELOGIC),
_resource_manager(
	PrimaryResource(RESOURCE_PRIORITY_PRIMARY, std::make_shared<s_segment_storage<std::string, std::shared_ptr<Map>>>()),
	SecondaryResource(RESOURCE_PRIORITY_SECONDARY, std::make_shared<s_segment_storage<uint64_t, std::shared_ptr<Units::Player>>>()),
	TertiaryResource(RESOURCE_PRIORITY_TERTIARY, std::make_shared<s_segment_storage<uint64_t, std::shared_ptr<Unit>>>())
)
{
}

void GameLogicProcess::initialize(int segment_number)
{
	set_segment_number(segment_number);

	/**
	 * Static Databases
	 */
	if (static_db_loaded == false) {
		ExpDB->load();
		ExpDB->load_status_point_table();
		JobDB->load();
		ItemDB->load();
		ItemDB->load_refine_db();
		ItemDB->load_weapon_target_size_modifiers_db();
		ItemDB->load_weapon_attribute_modifiers_db();
		StatusEffectDB->load();
		SkillDB->load();
		MonsterDB->load();
		StorageDB->load();
		static_db_loaded = true;
	}

	load_map_cache();
	start_internal();

	get_monster_spawn_agent().initialize(std::static_pointer_cast<GameLogicProcess>(shared_from_this()));

	_is_initialized.exchange(true);
}

void GameLogicProcess::finalize()
{
	this->get_resource_manager().clear<RESOURCE_PRIORITY_PRIMARY>();
	this->get_resource_manager().clear<RESOURCE_PRIORITY_SECONDARY>();
	this->get_resource_manager().clear<RESOURCE_PRIORITY_TERTIARY>();

	get_monster_spawn_agent().clear_monster_spawn_info();

	if (_thread.joinable()) {
		try {
			_thread.join();
		} catch(std::exception &e) {
			HLog(error) << "GameLogicProcess::finalize: " << e.what();
		}
	}

	_is_finalized.exchange(true);
}

bool GameLogicProcess::load_map_cache()
{
	Horizon::Libraries::MapCache m;
	std::string db_path = sZone->config().get_static_db_path().string();

	m.setMapListPath(db_path + "map_list.lua");
	m.setMapCachePath(sZone->config().get_mapcache_path().string());

	if (m.ReadMapListConfig() != MCACHE_CONFIG_OK) {
		HLog(error) <<"Could not read map config file '" << m.getMapListPath().string() << "'.";
		return false;
	}

	switch (m.ImportFromCacheFile())
	{
		default:
			break;
		case MCACHE_IMPORT_NONEXISTENT_FILE:
			HLog(error) << "Could not open file '" << m.getMapCachePath().string() << "'.";
			return false;
		case MCACHE_IMPORT_READ_ERROR:
			HLog(error) << "Could not read file '" << m.getMapCachePath().string() << "', rebuilding...";
			return false;
		case MCACHE_IMPORT_INVALID_CHECKSUM:
			HLog(error) << "File cache file '" << m.getMapCachePath().string() << "' is corrupted (invalid checksum), rebuilding...";
			return false;
		case MCACHE_IMPORT_DECOMPRESS_ERROR:
			HLog(error) << "File cache file '" << m.getMapCachePath().string() << "' could not be decompressed, rebuilding...";
			return false;
		case MCACHE_IMPORT_MAPINFO_ERROR:
			HLog(error) << "Could not read map information for a map while importing file '" << m.getMapCachePath().string() << "', rebuilding...";
			return false;
		case MCACHE_IMPORT_CELLINFO_ERROR:
			HLog(error) << "Could not read cell information for a map while importing file '" << m.getMapCachePath().string() << "', rebuilding...";
			return false;
	}

	_maps = m.getMCache()->maps;
	
	return true;
}

void GameLogicProcess::start_internal()
{
	int total_maps = 0;
	int mcache_size = _maps.size();
	int max_maps_per_thread = std::ceil((double) mcache_size / sZone->config().max_game_logic_threads());
	int container_max = get_segment_number() * max_maps_per_thread;
	int container_min = (get_segment_number() - 1) * max_maps_per_thread;

	auto map_i = _maps.begin();
	std::advance(map_i, container_min);
	for (int i = container_min; i < container_max; i++)
	{
		if (map_i == _maps.end())
			break;
			
		std::shared_ptr<Map> map = std::make_shared<Map>(std::static_pointer_cast<GameLogicProcess>(shared_from_this()), map_i->second.name(), map_i->second.width(), map_i->second.height(), map_i->second.getCells());
		
		this->get_resource_manager().add<RESOURCE_PRIORITY_PRIMARY>(map_i->second.name(), map);

		total_maps++;
		map_i++;
	}
	
	HLog(info) << "Started map container " << get_segment_number() << " (" << container_min + 1 << " to " << container_max << " maps) for a total of " << container_max - container_min << " out of "  << mcache_size << " maps.";

	_maps.clear();

	_thread = std::thread([this]() {
		while (!sZone->general_conf().is_test_run_minimal() && get_shutdown_stage() == SHUTDOWN_NOT_STARTED) {
			this->update(std::time(nullptr));
			std::this_thread::sleep_for(std::chrono::microseconds(MAX_CORE_UPDATE_INTERVAL));
		};
	});
}

void GameLogicProcess::update(uint64_t diff)
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	// Update the entities.
	for (auto unit_it : this->get_resource_manager().get_medium<RESOURCE_PRIORITY_TERTIARY>().get_map())
		unit_it.second->update(diff);

	getScheduler().Update();

	get_system_routine_manager().process_queue();
	
#if WIN32
	DWORD cpu = GetCurrentProcessorNumber();
	if (get_thread_cpu_id() != (int) cpu) 
		set_thread_cpu_id(cpu);
#elif __linux__
	int cpu = sched_getcpu();
	if (get_thread_cpu_id() != cpu)
		set_thread_cpu_id(cpu);
#endif
	calculate_and_set_cpu_load();
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	set_total_execution_time(time_span.count());
}

GameLogicProcess::MonsterSpawnAgent::~MonsterSpawnAgent()
{

}

void GameLogicProcess::MonsterSpawnAgent::reschedule_single_monster_spawn(std::shared_ptr<Horizon::Zone::Units::Monster> monster) 
{
	uint8_t type = 0;
	uint32_t guid = 0;
	uint16_t spawn_dataset_id = 0;
	uint8_t spawn_id = 0;

	sZone->from_uuid(monster->uuid(), type, guid, spawn_dataset_id, spawn_id);

	std::shared_ptr<monster_spawn_data> msd = get_monster_spawn_info(spawn_dataset_id);

	if (msd == nullptr) {
		HLog(error) << "Couldn't find monster spawn data for monster " << monster->uuid() << ", spawn_dataset_id " << spawn_dataset_id << ", monster_id " << monster->job_id() << " and spawn_id " << spawn_id << ".";
		return;
	}

	std::shared_ptr<Map> map = monster->map();

	msd->dead_amount++;
	// Set the spawn time cache. This is used to calculate the time to spawn the monster.
	monster_spawn_data::s_monster_spawn_time_cache spawn_time_cache;
	spawn_time_cache.dead_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	spawn_time_cache.spawn_time = msd->spawn_delay_base + std::rand() % (msd->spawn_delay_variance + 1);
	msd->dead_spawn_time_list.emplace(monster->uuid(), spawn_time_cache);

	get_container()->getScheduler().Schedule(
		Milliseconds(spawn_time_cache.spawn_time), 
		monster->get_scheduler_task_id(UNIT_SCHEDULE_MONSTER_RESPAWN),
		[this, monster, map, msd](TaskContext /*&context*/) {
			if (map->get_user_count() == 0)
				return;

			this->spawn_monster_internal(map, msd->spawn_dataset_id, msd->monster_id, 1, msd->x, msd->y, msd->x_area, msd->y_area);
			msd->dead_spawn_time_list.erase(monster->uuid());
			msd->dead_amount--;
		});
}

void GameLogicProcess::MonsterSpawnAgent::spawn_monsters(std::string map_name)
{
	auto monster_spawn_map = _monster_spawn_db.get_map();

	for (auto i = monster_spawn_map.begin(); i != monster_spawn_map.end(); i++) {
		std::shared_ptr<monster_spawn_data> msd = (*i).second;
		if (map_name.compare(msd->map_name) == 0) {
			if (msd->dead_amount > 0) {
				for (auto dead_it = msd->dead_spawn_time_list.begin(); dead_it != msd->dead_spawn_time_list.end();) {
					int64_t dead_monster_spawn_uuid = dead_it->first;
					monster_spawn_data::s_monster_spawn_time_cache dead_stc = dead_it->second;
					
					uint8_t type = 0;
					uint32_t guid = 0;
					uint16_t spawn_dataset_id = 0;
					uint8_t spawn_id = 0;

					sZone->from_uuid(dead_monster_spawn_uuid, type, guid, spawn_dataset_id, spawn_id);

					int64_t since_death_ms = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - dead_stc.dead_time);

					int64_t time_to_spawn_ms = dead_stc.spawn_time - since_death_ms;

					if (time_to_spawn_ms < 0)
						time_to_spawn_ms = 0;

					this->get_container()->getScheduler().Schedule(
						Milliseconds(time_to_spawn_ms),
						((uint64_t) guid << 32) + (int) UNIT_SCHEDULE_MONSTER_RESPAWN,
						[this, map_name, msd](TaskContext /*&context*/) {
							int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, map_name, nullptr);

							if (segment_number == 0)
								return;

							std::shared_ptr<Horizon::Zone::GameLogicProcess> container = sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number);

							std::shared_ptr<Map> map = container->get_resource_manager().template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(map_name, nullptr);

							if (map->get_user_count() == 0)
								return;

							this->spawn_monster_internal(map, msd->spawn_dataset_id, msd->monster_id, 1, msd->x, msd->y, msd->x_area, msd->y_area);
						});

					dead_it = msd->dead_spawn_time_list.erase(dead_it);
					msd->dead_amount--;
				}
			}

			int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, map_name, nullptr);

			if (segment_number == 0)
				return;

			std::shared_ptr<Horizon::Zone::GameLogicProcess> container = sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number);

			std::shared_ptr<Map> map = container->get_resource_manager().template get_resource<RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(map_name, nullptr);

			this->spawn_monster_internal(map, msd->spawn_dataset_id, msd->monster_id, msd->amount - msd->dead_amount, msd->x, msd->y, msd->x_area, msd->y_area);
		}
	}
}

void GameLogicProcess::MonsterSpawnAgent::despawn_monsters(std::string map_name)
{
	int segment_number = sZone->get_segment_number_for_resource<Horizon::Zone::GameLogicProcess, RESOURCE_PRIORITY_PRIMARY, std::string, std::shared_ptr<Map>>(Horizon::System::RUNTIME_GAMELOGIC, map_name, nullptr);

	if (segment_number == 0)
		return;

	std::shared_ptr<Horizon::Zone::GameLogicProcess> container = sZone->get_component_of_type<Horizon::Zone::GameLogicProcess>(Horizon::System::RUNTIME_GAMELOGIC, segment_number);

	if (container == nullptr)
		return;

	auto monster_spawned_map = container->get_resource_manager().get_medium<RESOURCE_PRIORITY_TERTIARY>().get_map();

	for (auto i = monster_spawned_map.begin(); i != monster_spawned_map.end();)
		if ((*i).second->map()->get_name() == map_name && (*i).second->type() == UNIT_MONSTER) {
			(*i).second->finalize();
			container->get_resource_manager().remove<RESOURCE_PRIORITY_TERTIARY>((*i).second->uuid());
			i = monster_spawned_map.erase(i);
		} else {
			i++;
		}
}

void GameLogicProcess::MonsterSpawnAgent::spawn_monster_internal(std::shared_ptr<Map> map, int spawn_dataset_id, int monster_id, int16_t amount, int16_t x, int16_t y, int16_t x_area, int16_t y_area)
{
	std::shared_ptr<const monster_config_data> md = MonsterDB->get_monster_by_id(monster_id);
	
	if (md == nullptr) {
		HLog(warning) << "Monster " << monster_id << " set for spawn in " << map->get_name() << " does not exist in the database.";
		return;
	}

	std::shared_ptr<std::vector<std::shared_ptr<const monster_skill_config_data>>> mskd = MonsterDB->get_monster_skill_by_id(monster_id);
	
	for (int i = 0; i < amount; i++) {
		MapCoords mcoords = MapCoords(x, y);
		if (mcoords == MapCoords(0, 0))
			mcoords = map->get_random_accessible_coordinates();
		else if (x_area && y_area) {
			if ((mcoords = map->get_random_coordinates_in_walkable_area(x, y, x_area, y_area)) == MapCoords(0, 0)) {
				HLog(warning) << "Couldn't spawn monster " << md->name << " in area, spawning it on random co-ordinates.";
				mcoords = map->get_random_accessible_coordinates();
			}
		}
		std::shared_ptr<Horizon::Zone::Units::Monster> monster = std::make_shared<Horizon::Zone::Units::Monster>(spawn_dataset_id, i, map, mcoords, md, mskd);
		
		monster->initialize();
		map->container()->get_resource_manager().add<RESOURCE_PRIORITY_TERTIARY>(monster->guid(), monster);
	}

	return;
}