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

#ifndef HORIZON_ZONE_GAME_MAP_COORDINATES_HPP
#define HORIZON_ZONE_GAME_MAP_COORDINATES_HPP

#include <cinttypes>
#include <cassert>
#include <cmath>

template <int16_t MAX_COORDINATES>
class Coordinates
{
public:
	Coordinates(int16_t x = 0, int16_t y = 0)
	: _x(x), _y(y), _move_cost(0)
	{
		//
	}

	Coordinates(const Coordinates<MAX_COORDINATES> &obj)
	: _x(obj._x), _y(obj._y), _move_cost(obj._move_cost)
	{
		//
	}

	Coordinates<MAX_COORDINATES> operator = (const Coordinates<MAX_COORDINATES> &right)
	{
		_x = right._x;
		_y = right._y;
		_move_cost = right._move_cost;
		return *this;
	}

	bool operator == (Coordinates<MAX_COORDINATES> const &right) const
	{
		return (_x == right._x && _y == right._y);
	}

	bool operator != (Coordinates<MAX_COORDINATES> const &right) const
	{
		return !(*this == right);
	}

    Coordinates<MAX_COORDINATES> operator + (Coordinates<MAX_COORDINATES> const &right) const
    {
        return Coordinates<MAX_COORDINATES>(_x + right._x, _y + right._y);
    }

    Coordinates<MAX_COORDINATES> operator - (Coordinates<MAX_COORDINATES> const &right) const
    {
        return Coordinates<MAX_COORDINATES>(_x - right._x, _y - right._y);
    }

	template <int16_t BOUNDS>
	bool is_within_range(Coordinates<BOUNDS> const &bounds, int range) const
	{
		int x_diff = _x - bounds.x();
		int y_diff = _y - bounds.y();

		return abs(x_diff) <= range && abs(y_diff) <= range;
	}

	template<int16_t BOUNDS>
	Coordinates<BOUNDS> at_range(int range) const
	{
		int x = std::max(0, std::min((_x + range), (int) BOUNDS));
		int y = std::max(0, std::min((_y + range), (int) BOUNDS));

		return Coordinates<BOUNDS>(x, y);
	}

	// d=√((x2 – x1)² + (y2 – y1)²)
	template<int16_t BOUNDS>
	int distance_from(Coordinates<BOUNDS> const &bounds) const
	{
		int distance = std::sqrt(std::pow(bounds.x() - _x, 2) + std::pow(bounds.y() - _y, 2));
		return distance;
	}
	
	template<int16_t BLOCK_SIZE, int16_t BLOCK_COUNT>
	Coordinates<BLOCK_COUNT> scale() const
	{
		int x = _x / BLOCK_SIZE;
		int y = _y / BLOCK_SIZE;

		assert(x < BLOCK_COUNT && y < BLOCK_COUNT);
		assert(x >= 0 && y >= 0);

		return Coordinates<BLOCK_COUNT>(x, y);
	}

	int16_t x() const { return _x; }
	int16_t y() const { return _y; }
	int16_t move_cost() const { return _move_cost; }
	void set_move_cost(int16_t move_cost) { _move_cost = move_cost; }

	void inc_x(int16_t val)
	{
		if (_x + val < MAX_COORDINATES)
			_x += val;
		else
			_x = MAX_COORDINATES - 1;
	}

	void dec_x(int16_t val)
	{
		if (_x > val)
			_x -= val;
		else
			_x = 0;
	}

	void inc_y(int16_t val)
	{
		if (_y + val < MAX_COORDINATES)
			_y += val;
		else
			_y = MAX_COORDINATES - 1;
	}

	void dec_y(int16_t val)
	{
		if (_y > val)
			_y -= val;
		else
			_y = 0;
	}

	bool valid() const
	{
		return _x < MAX_COORDINATES && _y < MAX_COORDINATES;
	}

private:
	int16_t _x{0};
	int16_t _y{0};
	int16_t _move_cost{0};
};

#endif /* HORIZON_ZONE_GAME_MAP_COORDINATES_HPP */
