
#ifndef HORIZON_ZONE_GAME_SKILLEXECUTION_HPP
#define HORIZON_ZONE_GAME_SKILLEXECUTION_HPP

#include "Server/Common/Configuration/Horizon.hpp"
#include "Server/Zone/Definitions/SkillDefinitions.hpp"
#include "Server/Zone/Game/Map/Coordinates.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Script/ScriptManager.hpp"

namespace Horizon
{
namespace Zone
{
class GameLogicProcess;
class SkillExecution : public std::enable_shared_from_this<SkillExecution>
{
public:
	SkillExecution(std::shared_ptr<Unit> initial_source, int16_t skill_id, int16_t skill_lv);
	~SkillExecution();

	void execute(int initial_target_guid);
	void execute(MapCoords map_coords);
	void execute(int16_t x, int16_t y);
	void execute(MapCoords map_coords, std::string message);
	void execute(int16_t x, int16_t y, std::string message);

	sol::table get_skill_cast_data() { return _scd; }
private:
	void start_execution(enum skill_target_type target_type);
	std::shared_ptr<Map> map() { return _map.expired() ? nullptr : _map.lock(); }
	void set_map(std::shared_ptr<Map> map);
	std::shared_ptr<GameLogicProcess> map_container() { return _map_container_thread.lock(); }
	std::shared_ptr<ScriptManager> lua_manager() { return _lua_mgr.lock(); }
	std::shared_ptr<sol::state> lua_state() { return _lua_state.lock(); }

	std::weak_ptr<Map> _map;
	std::weak_ptr<GameLogicProcess> _map_container_thread;
	std::weak_ptr<ScriptManager> _lua_mgr;
	std::weak_ptr<sol::state> _lua_state;
	std::shared_ptr<Unit> _initial_source{nullptr};
	std::shared_ptr<Unit> _initial_target{nullptr};
	int16_t _skill_id;
	int16_t _skill_lv;
	MapCoords _map_coords;
	std::string _message;
	sol::table _scd;
};
}
}

#endif /* HORIZON_ZONE_GAME_SKILLEXECUTION_HPP */
