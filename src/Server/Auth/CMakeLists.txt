###################################################
#       _   _            _                        #
#      | | | |          (_)                       #
#      | |_| | ___  _ __ _ _______  _ __          #
#      |  _  |/ _ \| '__| |_  / _ \| '_  \        #
#      | | | | (_) | |  | |/ / (_) | | | |        #
#      \_| |_/\___/|_|  |_/___\___/|_| |_|        #
###################################################
# This file is part of Horizon (c).
#
# Copyright (c) 2019 Sagun K. (sagunxp@gmail.com).
# Copyright (c) 2019 Horizon Dev Team.
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

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE_SOURCES
)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

set(AUTH_TARGET_SOURCES 
	${PRIVATE_SOURCES}
	${COMMON_SOURCES}
	${COMMON_HEADERS}
	${CORE_SOURCES}
	${UTILITY_SOURCES})

add_executable(auth
	${AUTH_TARGET_SOURCES}
)

set(AUTH_TARGET_SOURCES ${AUTH_TARGET_SOURCES} PARENT_SCOPE)

target_link_libraries(auth
	PUBLIC
		${Boost_LIBRARIES}
    	networking
		${LUA_LIBRARIES}
		OpenSSL::SSL
		${QUADMATH_LIBRARY}
)

target_precompile_headers(auth
	PUBLIC
		../pch.hpp)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src
)

CollectIncludeDirectories(
	${INCLUDE_DIRS}
	PUBLIC_INCLUDES
)

# LuaJit linker flags for macOS to avoid crashes.
if (APPLE)
set_target_properties(auth
	PROPERTIES
	LINK_FLAGS "-pagezero_size 10000 -image_base 100000000"
	)
endif()

target_include_directories(auth
	PUBLIC
		${Boost_INCLUDE_DIRS}
		${PUBLIC_INCLUDES}
		${LUA_INCLUDE_DIR}
		${SOL2_INCLUDE_DIR}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
)

# Enable CMP0091 for MSVC Builds
if (MSVC)
set_property(TARGET auth PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(MSVC)

install(TARGETS auth
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE})

install(FILES ${PROJECT_SOURCE_DIR}/config/auth-server.lua.dist DESTINATION ${CONF_DIR})
