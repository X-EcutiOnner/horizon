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

#ifndef HORIZON_ZONE_PERSISTENCE_PERSISTENCEMANAGER
#define HORIZON_ZONE_PERSISTENCE_PERSISTENCEMANAGER
#include "Server/Common/System.hpp"
#include "Server/Common/Server.hpp"


namespace Horizon
{
namespace Zone
{
	namespace Units
	{
		class Player;
	}
class PersistenceManager : public KernelComponent
{
public:
	PersistenceManager();
    void initialize(int segment_number = 1) override;
    void finalize() override;
    void start();
    void update(uint64_t diff);

	bool is_initialized() override { return _is_initialized.load(); }
	bool is_finalized() override { return _is_finalized.load(); }
    
private:
    std::thread _thread;
	std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};

protected:
	using PrimaryResource = SharedPriorityResourceMedium<s_segment_storage<uint64_t, std::shared_ptr<Units::Player>>>;
	using ResourceManager = SharedPriorityResourceManager<PrimaryResource>;
	ResourceManager _resource_manager;
public:
	ResourceManager& get_resource_manager() { return _resource_manager; }
};
}
}
#endif /* HORIZON_ZONE_PERSISTENCE_PERSISTENCEMANAGER */