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
# Common Sources
add_subdirectory(Common)
set(COMMON_HEADERS ${COMMON_HEADERS} PARENT_SCOPE)

# Targets
add_subdirectory(Auth)
list(REMOVE_ITEM AUTH_TARGET_SOURCES REGEX "${CMAKE_CURRENT_SOURCE_DIR}/Auth/Main.cpp")
set(AUTH_TESTING_SOURCES ${AUTH_TARGET_SOURCES} PARENT_SCOPE)

add_subdirectory(Char)
list(REMOVE_ITEM CHAR_TARGET_SOURCES REGEX "${CMAKE_CURRENT_SOURCE_DIR}/Char/Main.cpp")
set(CHAR_TESTING_SOURCES ${CHAR_TARGET_SOURCES} PARENT_SCOPE)

add_subdirectory(Zone)
list(REMOVE_ITEM ZONE_TARGET_SOURCES REGEX "${CMAKE_CURRENT_SOURCE_DIR}/Zone/Main.cpp")
set(ZONE_TESTING_SOURCES ${ZONE_TARGET_SOURCES} PARENT_SCOPE)
