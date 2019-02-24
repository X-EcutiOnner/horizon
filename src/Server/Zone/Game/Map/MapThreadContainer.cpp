/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2019 Horizon Dev Team.
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

#include "MapThreadContainer.hpp"
#include "Server/Zone/Game/Entities/Unit/Player/Player.hpp"
#include "Server/Zone/Session/ZoneSession.hpp"
#include "Server/Zone/Zone.hpp"
#include "Server/Common/Models/GameAccount.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Core/Logging/Logger.hpp"

using namespace Horizon::Zone::Game;

MapThreadContainer::MapThreadContainer()
{

}

MapThreadContainer::~MapThreadContainer()
{
	if (_thread.joinable())
		_thread.join();
}

std::shared_ptr<Map> MapThreadContainer::get_map(std::string name) const
{
	auto map_it = _managed_maps.find(name);

	if (map_it == _managed_maps.end())
		return std::shared_ptr<Map>();

	return map_it->second;
}

void MapThreadContainer::add_map(std::shared_ptr<Map> &&m)
{
	_managed_maps.emplace(m->get_name(), m);
}

void MapThreadContainer::remove_map(std::shared_ptr<Map> &&m)
{
	_managed_maps.erase(m->get_name());
}

void MapThreadContainer::add_player(std::string /*map_name*/, std::shared_ptr<Entities::Player> p)
{
	_player_buffer.push(std::make_pair(true, p));
}

void MapThreadContainer::remove_player(std::shared_ptr<Entities::Player> p)
{
	_player_buffer.push(std::make_pair(false, p));
}

void MapThreadContainer::initialize()
{
	_script_mgr = std::make_shared<ScriptManager>(shared_from_this());

	for (auto mi = _managed_maps.begin(); mi != _managed_maps.end(); mi++) {
		mi->second->ensure_all_grids();
	}
}

void MapThreadContainer::start()
{
	_thread = std::thread(&MapThreadContainer::start_internal, this);

	ZoneLog->info("A map container thread with {} maps has begun working.", _managed_maps.size());
}

void MapThreadContainer::start_internal()
{
	int core_update_interval = ZoneServer->general_conf().get_core_update_interval();

	get_script_manager()->initialize();

	while (!ZoneServer->general_conf().is_test_run() && ZoneServer->get_shutdown_stage() == SHUTDOWN_NOT_STARTED) {
		update(core_update_interval);
		std::this_thread::sleep_for(std::chrono::microseconds(core_update_interval));
	};

	for (auto pi = _managed_players.begin(); pi != _managed_players.end(); pi++) {
		// save and cleanup.
	}

	get_script_manager()->finalize();
	_managed_players.clear();
	_managed_maps.clear();
}

void MapThreadContainer::update(uint32_t diff)
{
	std::shared_ptr<std::pair<bool, std::shared_ptr<Entities::Player>>> pbuf = nullptr;

	// Add any new players / remove anyone else.
	while ((pbuf = _player_buffer.try_pop())) {
		std::shared_ptr<Entities::Player> player = pbuf->second;

		if (player->get_session() == nullptr)
			continue;

		if (pbuf->first) {
			if (!player->is_initialized())
				player->initialize();
			_managed_players.emplace(player->get_guid(), player);
			get_script_manager()->add_player(player);
		} else {
			_managed_players.erase(player->get_guid());
			get_script_manager()->remove_player(player);
		}
	}

	// Update sessions
	for (auto pi = _managed_players.begin(); pi != _managed_players.end(); pi++) {
		std::shared_ptr<ZoneSession> session = pi->second->get_session();

		if (session == nullptr) {
			_managed_players.erase(pi);
			continue;
		}

		// process packets
		pi->second->get_session()->update(diff);
		// update entity.
		pi->second->update(diff);
	}
}
