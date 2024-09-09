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

#ifndef HORIZON_ZONE_GAME_ENTITIES_MONSTER_HPP
#define HORIZON_ZONE_GAME_ENTITIES_MONSTER_HPP

#include "Server/Zone/Game/GridObject.hpp"
#include "Server/Zone/Game/Units/Mob/Mob.hpp"
#include "Server/Zone/Game/StaticDB/MonsterDB.hpp"


namespace Horizon
{
namespace Zone
{
class Map;
namespace Units
{
class Player;
class Monster : public Mob, public GridObject<Monster>
{
public:
	explicit Monster(int16_t spawn_dataset_id, int8_t spawn_id, std::shared_ptr<Map> map, MapCoords mcoords,
		std::shared_ptr<const monster_config_data> md,
		std::shared_ptr<std::vector<std::shared_ptr<const monster_skill_config_data>>> mskd);
	~Monster() override;

	bool initialize();
	virtual bool finalize() override;

	virtual void stop_movement() override;
	virtual void on_pathfinding_failure() override;
	virtual void on_movement_begin() override;
	virtual void on_movement_step() override;
	virtual void on_movement_end() override;

	void on_damage_received(std::shared_ptr<Unit> damage_dealer, int damage) override;
	void on_killed(std::shared_ptr<Unit> killer, bool with_drops = false, bool with_exp = false) override;

	void set_spotted(bool spotted) { _was_spotted_once = spotted; }
	bool was_spotted_once() { return _was_spotted_once; }

	std::shared_ptr<const monster_config_data> monster_config() { return _wmd_data.lock(); }
	void set_monster_config(std::shared_ptr<const monster_config_data> md) { _wmd_data = md; }

    /**
     * Status Effect
     */
    void on_status_effect_start(std::shared_ptr<status_change_entry> sce) override;
    void on_status_effect_end(std::shared_ptr<status_change_entry> sce) override;
    void on_status_effect_change(std::shared_ptr<status_change_entry> sce) override;

    void behavior_passive();
    void behavior_active(std::shared_ptr<Player> pl);

    void set_next_walk_time(int walk_time) { _next_walk_time = walk_time; }
    int next_walk_time() { return _next_walk_time; }

    void set_target(std::shared_ptr<Unit> &target) { _target = target; }
    std::shared_ptr<Unit> target() { return _target; }
    void unlock_target() { _target = nullptr; }

private:
	bool _was_spotted_once{false};
	int _next_walk_time{0}, _last_spotted_time{0}, _last_think_time{0};
	std::weak_ptr<const monster_config_data> _wmd_data;
	std::weak_ptr<std::vector<std::shared_ptr<const monster_skill_config_data>>> _wms_data;

    std::shared_ptr<Unit> _target{nullptr};

};
}
}
}

#endif /* HORIZON_ZONE_GAME_ENTITIES_MONSTER_HPP */
