add_subdirectory(Utility)
add_subdirectory(Core)
add_subdirectory(Libraries)

#################
# Horizon Project Servers
#################
if (NOT WITHOUT_SERVERS)
	add_subdirectory(Server)
endif (NOT WITHOUT_SERVERS)

#################
# Horizon Project Tests
#################
if (WITH_TESTS AND WITHOUT_SERVERS)
	add_subdirectory(Server/Common)
	CollectSourceFiles(
    	${PROJECT_SOURCE_DIR}/src/Server/Auth
		AUTH_TARGET_SOURCES
	)
	list(REMOVE_ITEM AUTH_TARGET_SOURCES REGEX "${PROJECT_SOURCE_DIR}/src/Server/Auth/Main.cpp")
	set(AUTH_TESTING_SOURCES
		${AUTH_TARGET_SOURCES}
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Auth.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Auth.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Interface/AuthClientInterface.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Interface/AuthClientInterface.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Session/AuthSession.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Session/AuthSession.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Socket/AuthSocket.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/Socket/AuthSocket.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/SocketMgr/ClientSocketMgr.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Auth/SocketMgr/ClientSocketMgr.cpp
		${COMMON_SOURCES}
		${COMMON_HEADERS}
		${CORE_SOURCES}
		${UTILITY_SOURCES}
	)
	CollectSourceFiles(
    	${PROJECT_SOURCE_DIR}/src/Server/Char
		CHAR_TARGET_SOURCES
	)
	list(REMOVE_ITEM CHAR_TARGET_SOURCES REGEX "${PROJECT_SOURCE_DIR}/src/Server/Char/Main.cpp")
	set(CHAR_TESTING_SOURCES
		${CHAR_TARGET_SOURCES}
		${CMAKE_SOURCE_DIR}/src/Server/Char/Char.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/Char.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/Interface/CharClientInterface.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/Interface/CharClientInterface.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/Session/CharSession.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/Session/CharSession.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/Socket/CharSocket.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/Socket/CharSocket.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/SocketMgr/ClientSocketMgr.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Char/SocketMgr/ClientSocketMgr.cpp
		${COMMON_SOURCES}
		${COMMON_HEADERS}
		${CORE_SOURCES}
		${UTILITY_SOURCES}
	)
	add_subdirectory(Server/Zone/Packets)
	add_subdirectory(Server/Zone/Game)
	add_subdirectory(Server/Zone/Persistence)
	add_subdirectory(Server/Zone/Script)
	add_subdirectory(Server/Zone/Interface)
	set(ZONE_TESTING_SOURCES
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Zone.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Zone.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/ZoneSystem.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/ZoneSystem.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Interface/ZoneClientInterface.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Interface/ZoneClientInterface.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Session/ZoneSession.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Session/ZoneSession.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Socket/ZoneSocket.cpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/Socket/ZoneSocket.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/SocketMgr/ClientSocketMgr.hpp
		${CMAKE_SOURCE_DIR}/src/Server/Zone/SocketMgr/ClientSocketMgr.cpp
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
	add_subdirectory(Tests)
elseif(WITH_TEST)
	add_subdirectory(Tests)
endif ()

#################
# Horizon Project Tools
#################
if (WITH_TOOLS)
    add_subdirectory(Tools)
endif (WITH_TOOLS)