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
# Setup CMake to run tests
enable_testing()

CollectSourceFiles(
	${CMAKE_CURRENT_SOURCE_DIR}
	TEST_SOURCES
)

list(REMOVE_ITEM TEST_SOURCES REGEX "${CMAKE_CURRENT_SOURCE_DIR}/prontera.cpp")

set(CORE_DIR ${PROJECT_SOURCE_DIR}/src/Core)
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/src/Libraries)
set(UTIL_DIR ${PROJECT_SOURCE_DIR}/src/Utility)
set(COMMON_DIR ${PROJECT_SOURCE_DIR}/src/Server/Common)
set(SERVER_DIR ${PROJECT_SOURCE_DIR}/src/Server)
set(ZONE_DIR ${SERVER_DIR}/Zone)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

foreach(TEST_SOURCE ${TEST_SOURCES})
	# Extract the filename without an extension (NAME_WE)
	get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

	set (ADD_LIBS "")
	set (ADD_SOURCES "")
	set (ADD_INCLUDE_DIRS "")
	
	if (TEST_NAME STREQUAL "TaskSchedulingTest")
		set (ADD_SOURCES
			${UTIL_DIR}/TaskScheduler.cpp
			${UTIL_DIR}/TaskScheduler.hpp)
	elseif (TEST_NAME STREQUAL "Sol2Test")
		set (ADD_LIBS ${LUA_LIBRARIES})
		set (ADD_INCLUDE_DIRS ${LUA_INCLUDE_DIR} ${SOL2_INCLUDE_DIR})
	elseif (TEST_NAME STREQUAL "LoggingTest")
		set (ADD_SOURCES
			${CORE_DIR}/Logging/Logger.cpp
			${CORE_DIR}/Logging/Logger.hpp)
		set(ADD_LIBS ${Boost_LIBRARIES})
		set(ADD_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
	elseif (TEST_NAME STREQUAL "SystemRoutinesTest")
		set (ADD_SOURCES ${ADD_SOURCES}
			${COMMON_DIR}/Server.hpp
			${COMMON_DIR}/Server.cpp
			${COMMON_DIR}/System.hpp
			${COMMON_DIR}/System.cpp
			${CORE_DIR}/Logging/Logger.cpp
			${CORE_DIR}/Logging/Logger.hpp
			${SERVER_DIR}/pch.hpp
			${COMMON_DIR}/Configuration/Horizon.hpp)
		set (ADD_LIBS 
			${Boost_LIBRARIES}
			${LUA_LIBRARIES}
			OpenSSL::SSL
		)
		set (ADD_INCLUDE_DIRS 
			${Boost_INCLUDE_DIRS}
			${PUBLIC_INCLUDES}
			${LUA_INCLUDE_DIR}
			${SOL2_INCLUDE_DIR})
	elseif (TEST_NAME STREQUAL "AuthServerTest")
		CollectSourceFiles(
    		${SERVER_DIR}/Auth
			AUTH_PRIVATE_SOURCES
		)
		list(REMOVE_ITEM AUTH_PRIVATE_SOURCES REGEX "${SERVER_DIR}/Auth/Main.cpp")
		set (ADD_SOURCES	
			${AUTH_TESTING_SOURCES}
		)
		set (ADD_LIBS 
			${Boost_LIBRARIES}
			networking
			${LUA_LIBRARIES}
			OpenSSL::SSL
		)
		set (ADD_INCLUDE_DIRS 
			${Boost_INCLUDE_DIRS}
			${PUBLIC_INCLUDES}
			${LUA_INCLUDE_DIR}
			${SOL2_INCLUDE_DIR})
	elseif (TEST_NAME STREQUAL "CharServerTest")
		set (ADD_SOURCES
			${CHAR_TESTING_SOURCES}
		)
		set (ADD_LIBS 
			${Boost_LIBRARIES}
			networking
			${LUA_LIBRARIES}
			OpenSSL::SSL
		)
		set (ADD_INCLUDE_DIRS 
			${Boost_INCLUDE_DIRS}
			${PUBLIC_INCLUDES}
			${LUA_INCLUDE_DIR}
			${SOL2_INCLUDE_DIR})
	elseif (TEST_NAME STREQUAL "ZoneServerTest")
		set (ADD_SOURCES
			${ZONE_TESTING_SOURCES}
		)
		set (ADD_LIBS 
        	${Boost_LIBRARIES}
			networking
			mcache
			${LUA_LIBRARIES}
			OpenSSL::SSL
		)
		set (ADD_INCLUDE_DIRS 
			${Boost_INCLUDE_DIRS}
			${PUBLIC_INCLUDES}
			${LUA_INCLUDE_DIR}
			${SOL2_INCLUDE_DIR})
	elseif (TEST_NAME STREQUAL "MySQLTest")
		set (ADD_LIBS OpenSSL::SSL)
	endif ()

	# Add test executable.
	add_executable(${TEST_NAME} ${TEST_SOURCE} ${ADD_SOURCES})

	# Link Boost libraries and additional required dependencies.
	target_link_libraries(${TEST_NAME}
		PUBLIC
		${Boost_LIBRARIES}
		${ADD_LIBS}
		${QUADMATH_LIBRARY})

	# Link additional include search directories for the executable.
	target_include_directories(${TEST_NAME}
		PUBLIC
			${PROJECT_SOURCE_DIR}/src
			${Boost_INCLUDE_DIRS}
			${ADD_INCLUDE_DIRS})

	if (MSVC)
		target_compile_definitions(${TEST_NAME} PRIVATE _CRTDBG_MAP_ALLOC)
		set_property(TARGET ${TEST_NAME} PROPERTY
			MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif(MSVC)

	# Sol2 LUA linker flags on macOS to avoid crashes.
	# @see http://luajit.org/install.html
	if (APPLE AND TEST_NAME STREQUAL "Sol2Test")
		set_target_properties(${TEST_NAME}
			PROPERTIES
			LINK_FLAGS "-pagezero_size 10000 -image_base 100000000"
			)
	endif()
endforeach(TEST_SOURCE)
