###################################################
#       _   _            _                        #
#      | | | |          (_)                       #
#      | |_| | ___  _ __ _ _______  _ __          #
#      |  _  |/ _ \| '__| |_  / _ \| '_  \        #
#      | | | | (_) | |  | |/ / (_) | | | |        #
#      \_| |_/\___/|_|  |_/___\___/|_| |_|        #
###################################################
# This file is part of Horizon (c).
# Copyright (c) 2018 Horizon Dev Team.
#
# Base Author - Sagun K. (sagunxp@gmail.com)
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this library.  If not, see <http://www.gnu.org/licenses/>.
###################################################

find_library(QUADMATH_LIBRARY
	NAMES quadmath
	PATH_SUFFIXES 10 11 12 13 14 15 16
	PATHS
		/usr/lib/gcc/x86_64-linux-gnu/
	)

if (QUADMATH_LIBRARY)
	message(STATUS "Found QuadMath Library: ${QUADMATH_LIBRARY} (Required for Boost.CharConv / Boost.MySQL)")
else()
	message(FATAL_ERROR "Could not find QuadMath Library (Required for Boost.CharConv / Boost.MySQL)")
endif()
