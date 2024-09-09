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

#ifndef HORIZON_ZONE_GAME_TRAITS_ATTRIBUTES_HPP
#define HORIZON_ZONE_GAME_TRAITS_ATTRIBUTES_HPP

#include "Server/Zone/Definitions/UnitDefinitions.hpp"
#include "Server/Zone/Game/Units/Traits/ObservableStatus.hpp"
#include <memory>

namespace Horizon
{
namespace Zone
{
class Unit;
class Player;
namespace Traits
{	
	class AttributeVariance
	{
	public:
		virtual void add_min(int32_t val) { set_min(_min + val); }
		virtual void sub_min(int32_t val) { set_min(_min - std::min(_min, val)); }
		virtual int32_t get_min() const { return _min; }
		
		virtual void set_min(int32_t val)
		{
			_min = val;
		}

		virtual void add_max(int32_t val) { set_max(_max + val); }
		virtual void sub_max(int32_t val) { set_max(_max - std::min(_max, val)); }
		virtual int32_t get_max() const { return _max; }

		virtual void set_max(int32_t val)
		{
			_max = val;
		}

	protected:
		int32_t _min{ 0 }, _max{ 0 };
	};


	class Attribute
	{
	public:
		Attribute(std::weak_ptr<Unit> unit, status_point_type st_type, int32_t base = 0, int32_t equip = 0, int32_t status = 0)
		: _base_val(base), _status_point_type(st_type), _equip_val(equip), _status_val(status), _unit(unit)
		{
		}
		virtual ~Attribute() { }

		std::shared_ptr<Unit> unit() { return _unit.lock(); }
		void unit(std::shared_ptr<Unit> e) { _unit = e; }

		virtual void set_base(int32_t val)
		{
			_base_val = val;
			notify();
		}
		virtual void add_base(int32_t val);
		virtual void sub_base(int32_t val);
		virtual int32_t get_base() const { return _base_val; }

		virtual void set_equip(int32_t val)
		{
			_equip_val = val;
			notify();
		}
		virtual void add_equip(int32_t val);
		virtual void sub_equip(int32_t val);
		virtual int32_t get_equip() const { return _equip_val; }

		virtual void set_status(int32_t val)
		{
			_status_val = val;
			notify();
		}
		virtual void add_status(int32_t val);
		virtual void sub_status(int32_t val);
		virtual int32_t get_status() const { return _status_val; }

		virtual int32_t total() const { return _base_val + _equip_val + _status_val; }

		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		TT operator + (TT right) { return total() + right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		TT operator / (TT right) { return total() / right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		TT operator * (TT right) { return total() * right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		TT operator - (TT right) { return total() - right; }

		int operator + (Attribute const &right) const { return total() + right.total() ; }
		double operator / (Attribute const &right) { return right.total() / total(); }
		double operator * (Attribute const &right) { return right.total() * total(); }
		int operator - (Attribute const &right) { return right.total() - total(); }
		
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		bool operator == (TT right) { return total() == right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		bool operator != (TT right) { return total() != right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		bool operator > (TT right) { return total() > right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		bool operator >= (TT right) { return total() >= right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		bool operator < (TT right) { return total() < right; }
		template <typename TT, typename std::enable_if<std::is_integral<TT>::value>::type* = nullptr>
		bool operator <= (TT right) { return total() <= right; }

		bool operator == (Attribute const &right) { return total() == right.total(); }
		bool operator != (Attribute const &right) { return total() != right.total(); }
		bool operator > (Attribute const &right) { return total() > right.total(); }
		bool operator >= (Attribute const &right) { return total() >= right.total(); }
		bool operator < (Attribute const &right) { return total() < right.total(); }
		bool operator <= (Attribute const &right) { return total() <= right.total(); }

		Attribute operator = (Attribute &right)
		{
			unit(right.unit());
			set_base(right.get_base());
			set_equip(right.get_equip());
			set_status(right.get_status());

			return *this;
		}
		
		void notify();

		status_point_type get_type() const { return _status_point_type; }

	protected:
		status_point_type _status_point_type{status_point_type::STATUS_POINT_INVALID};
		int32_t _base_val{0};
		int32_t _equip_val{0};
		int32_t _status_val{0};
	private:
		std::weak_ptr<Unit> _unit;
	};
}
}
}

#endif /* HORIZON_ZONE_GAME_STATUS_ATTRIBUTES_HPP */
