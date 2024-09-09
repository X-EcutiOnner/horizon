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
 * Copyright (c) 2015 Damian Barczynski <daan.net@wp.eu>
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
 *
 * Following tool is licensed under the terms and conditions of the ISC license.
 * License terms - https://github.com/daancode/a-star/blob/master/LICENSE
 **************************************************/

#ifndef HORIZON_ZONE_GAME_MAP_PATH_ASTAR_HPP
#define HORIZON_ZONE_GAME_MAP_PATH_ASTAR_HPP

#include "Server/Zone/Game/Map/Coordinates.hpp"
#include "Server/Zone/Game/Map/Grid/GridDefinitions.hpp"
#include "Server/Common/Configuration/Horizon.hpp"
#include <cmath>
#include <memory>
#include <functional>
#include <vector>

namespace Horizon
{
namespace Zone
{
namespace AStar
{
class Heuristic
{
	static MapCoords getDelta(MapCoords source_, MapCoords target_) { return MapCoords(abs(target_.x() - source_.x()), abs(target_.y() - source_.y())); }

public:
	static uint32_t manhattan(MapCoords source_, MapCoords target_)
	{
		auto delta = getDelta(source_, target_);
		return static_cast<uint32_t>(10 * (delta.x() + delta.y()));
	}

	static uint32_t euclidean(MapCoords source_, MapCoords target_)
	{
		auto delta = getDelta(source_, target_);
		return static_cast<uint32_t>(10 * std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2)));
	}

	static uint32_t octagonal(MapCoords source_, MapCoords target_)
	{
		auto delta = getDelta(source_, target_);
		return 10 * (delta.x() + delta.y()) + (-6) * std::min(delta.x(), delta.y());
	}
};

typedef std::function<uint32_t(MapCoords, MapCoords)> HeuristicFunction;
/// @brief We use a vector because the AStar algorithm is only searching on small datasets.
/// Other data structures such as a priority queue can be used for larger datasets.
typedef std::vector<MapCoords> CoordinateList;
typedef std::function<bool(uint16_t x, uint16_t y)> CollisionDetectionFunction;

struct Node
{
	uint32_t G, H;
	MapCoords coordinates;
	std::shared_ptr<Node> parent;

	explicit Node(MapCoords coord_, std::shared_ptr<Node> parent_ = nullptr)
	{
		parent = parent_;
		coordinates = coord_;
		G = H = 0;
	}

	uint32_t getFScore() const { return G + H; }
};

using NodeSet = std::vector<std::shared_ptr<Node>>;

class Generator
{
	static std::shared_ptr<Node> findNodeOnList(NodeSet& nodes_, MapCoords coordinates_)
	{
		for (auto node : nodes_) {
			if (node->coordinates == coordinates_) {
				return node;
			}
		}
		return nullptr;
	}
	static void releaseNodes(NodeSet nodes_)
	{
		for (auto it = nodes_.begin(); it != nodes_.end();) {
			it = nodes_.erase(it);
		}
	}

public:
	Generator()
	{
		setDiagonalMovement(true);
		setHeuristic(&Heuristic::manhattan);
	}

	Generator(MapCoords worldSize_, CollisionDetectionFunction c, bool diagonal_movement = true, HeuristicFunction h = &Heuristic::manhattan)
	{
		setWorldSize(worldSize_);
		setCollisionDetectionFunction(std::move(c));
		setDiagonalMovement(diagonal_movement);
		setHeuristic(h);
	}

	void setWorldSize(MapCoords worldSize_) { worldSize = worldSize_; }

	void setCollisionDetectionFunction(CollisionDetectionFunction c) { check_collision = c; };

	void setDiagonalMovement(bool enable_) { directions = (enable_ ? 8 : 4); }

	void setHeuristic(const HeuristicFunction& heuristic_) { heuristic = [heuristic_](auto && PH1, auto && PH2) { return heuristic_(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); }; }

	CoordinateList findPath(MapCoords source_, MapCoords target_)
	{
		CoordinateList path;
		std::shared_ptr<Node> current = nullptr;
		NodeSet openSet, closedSet;
		int searchStep = 0;

		openSet.reserve(20);
		closedSet.reserve(20);
		openSet.push_back(std::make_shared<Node>(source_));

		if (check_collision(target_.x(), target_.y()))
			return path;

		while (!openSet.empty() && searchStep < MAX_VIEW_RANGE) {
			auto current_it = openSet.begin();
			
			current = *current_it;

			for (auto it = openSet.begin(); it != openSet.end(); it++) {
				auto node = *it;
				if (node->getFScore() <= current->getFScore()) {
					current = node;
					current_it = it;
				}
			}

			if (current->coordinates == target_)
				break;

			closedSet.push_back(current);
			openSet.erase(current_it);

			for (uint32_t i = 0; i < directions; ++i) {
				MapCoords newCoordinates(current->coordinates + direction[i]);

				if (check_collision(newCoordinates.x(), newCoordinates.y()) 
					|| findNodeOnList(closedSet, newCoordinates)) {
					continue;
				}

				newCoordinates.set_move_cost((i < 4) ? 10 : 14);
				uint32_t totalCost = current->G + newCoordinates.move_cost();

				std::shared_ptr<Node> successor = findNodeOnList(openSet, newCoordinates);
				if (successor == nullptr) {
					successor = std::make_shared<Node>(newCoordinates, current);
					successor->G = totalCost;
					successor->H = heuristic(successor->coordinates, target_);
					openSet.push_back(successor);
				} else if (totalCost < successor->G) {
					successor->parent = current;
					successor->G = totalCost;
				}
			}

			searchStep++;
		}

		while (current != nullptr) {
			path.push_back(current->coordinates);
			current = current->parent;
		}

		releaseNodes(openSet);
		releaseNodes(closedSet);

		return path;
	}

private:
	HeuristicFunction heuristic;
	CollisionDetectionFunction check_collision;
	CoordinateList direction = {
		{ 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
		{ -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
	};
	MapCoords worldSize;
	uint32_t directions{0};
};
}
}
}

#endif /* HORIZON_ZONE_GAME_MAP_PATH_ASTAR_HPP */
