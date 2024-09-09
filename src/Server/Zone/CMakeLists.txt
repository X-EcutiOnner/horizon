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

add_subdirectory(Game)
add_subdirectory(Packets)
add_subdirectory(Script)
add_subdirectory(Persistence)
add_subdirectory(Interface)

set(DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PRIVATE_SOURCES
	${DIR}/Main.cpp
	${DIR}/Zone.hpp
	${DIR}/Zone.cpp
	${DIR}/ZoneSystem.cpp
	${DIR}/ZoneSystem.hpp
	${DIR}/Interface/ZoneClientInterface.cpp
	${DIR}/Interface/ZoneClientInterface.hpp
	${DIR}/Session/ZoneSession.cpp
	${DIR}/Session/ZoneSession.hpp
	${DIR}/Socket/ZoneSocket.cpp
	${DIR}/Socket/ZoneSocket.hpp
	${DIR}/SocketMgr/ClientSocketMgr.hpp
	${DIR}/SocketMgr/ClientSocketMgr.cpp
)

set(ZONE_TARGET_SOURCES
	${PRIVATE_SOURCES}
	${COMMON_SOURCES}
	${COMMON_HEADERS}
	${CORE_SOURCES}
	${UTILITY_SOURCES}
	${PACKET_SOURCES}
	${PACKET_HEADERS}
	${GAME_SOURCES}
	${SCRIPT_SOURCES}
	${PERSISTENCE_SOURCES}
	${INTERFACE_SOURCES}
)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(zone
	${ZONE_TARGET_SOURCES}
)

set(ZONE_TARGET_SOURCES ${ZONE_TARGET_SOURCES} PARENT_SCOPE)

target_precompile_headers(zone
	PUBLIC
		../pch.hpp)

target_link_libraries(zone
	PUBLIC
    	${Boost_LIBRARIES}
		networking
		mcache
		${LUA_LIBRARIES}
		OpenSSL::SSL
		${QUADMATH_LIBRARY}
)

set(INCLUDE_DIRS
	${PROJECT_SOURCE_DIR}/src
)

CollectIncludeDirectories(
	${INCLUDE_DIRS}
	PUBLIC_INCLUDES
)

target_include_directories(zone
	PUBLIC
		${Boost_INCLUDE_DIRS}
		${PUBLIC_INCLUDES}
		${LUA_INCLUDE_DIR}
		${SOL2_INCLUDE_DIR}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
)

# LuaJit linker flags for macOS to avoid crashes.
if (APPLE)
	set(LUAJIT_LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
	set_target_properties(zone
			PROPERTIES
			LINK_FLAGS "${LUAJIT_LINK_FLAGS}"
	)
endif()

# Enable CMP0091 for MSVC Builds
if (MSVC)
set_property(TARGET zone PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(MSVC)

install(TARGETS zone
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	CONFIGURATIONS ${CMAKE_BUILD_TYPE})

install(FILES ${PROJECT_SOURCE_DIR}/config/zone-server.lua.dist DESTINATION ${CONF_DIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/db DESTINATION ${CMAKE_INSTALL_PREFIX})
