#include "SkillExecution.hpp"

#include "Server/Zone/Game/Units/Unit.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/StaticDB/SkillDB.hpp"
#include "Server/Zone/Script/Components/CombatComponent.hpp"
#include "Server/Zone/Script/Components/ItemComponent.hpp"
#include "Server/Zone/Script/Components/SkillComponent.hpp"
#include "Server/Zone/Script/Components/UnitComponent.hpp"
#include "Server/Zone/Definitions/SkillDefinitions.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

SkillExecution::SkillExecution(std::shared_ptr<Unit> initial_source, int16_t skill_id, int16_t skill_lv)
: _initial_source(initial_source), _skill_id(skill_id), _skill_lv(skill_lv), _map_coords(MapCoords(0, 0)), _message("")
{
	set_map(initial_source->map());
}

SkillExecution::~SkillExecution()
{
}

void SkillExecution::start_execution(enum skill_target_type target_type)
{
	std::shared_ptr<const struct skill_config_data> skd = SkillDB->get_skill_by_id(_skill_id);

	if (target_type == SKTT_SINGLE_TARGETED && _initial_target == nullptr)
		return;

	if (!skd) {
		HLog(debug) << "SkillExecution::start_execution: Could not find skill with id '" << _skill_id << "' in the database. Ignoring skill usage...";
		return;
	}

	try {
		sol::load_result fx = lua_state()->load_file(sZone->config().get_script_root_path().string() + "skills/" + skd->name + ".lua");
		sol::protected_function_result result = fx();
		if (!result.valid()) {
			sol::error err = result;
			HLog(error) << "SkillExecution::start_execution: " << err.what();
			return;
		}

		sol::table skill_functions = result;
		sol::protected_function initiate_skill_use = skill_functions["initiate_skill_use"];

		_scd = lua_state()->create_table();
		_scd["skill_execution"] = shared_from_this(); // ptr to self.
		_scd["initial_source"] = _initial_source;
		_scd["skill_id"] = _skill_id;
		_scd["skill_lv"] = _skill_lv;
		_scd["target_type"] = target_type;

		if (target_type == SKTT_SINGLE_TARGETED) {
			_scd["initial_target"] = _initial_target;
		} else {
			_scd["map_coords"] = _map_coords;
			if (_message.empty() == false)
				_scd["message"] = _message;
		}

		result = initiate_skill_use(_scd, skd);
		if (!result.valid()) {
			sol::error err = result;
			HLog(error) << "SkillExecution::start_execution: Error on initiate_skill_use function: " << err.what();
			return;
		}
	} catch (sol::error &e) {
		HLog(error) << "SkillExecution::start_execution: " << e.what();
		return;
	}
}

void SkillExecution::execute(int initial_target_guid)
{
	_initial_target = _initial_source->get_nearby_unit(initial_target_guid);
	start_execution(SKTT_SINGLE_TARGETED);
}

void SkillExecution::execute(int16_t x, int16_t y)
{
	execute(MapCoords(x, y));
}
void SkillExecution::execute(MapCoords map_coords)
{
	_map_coords = map_coords;
	start_execution(SKTT_GROUND_TARGETED);
}

void SkillExecution::execute(int16_t x, int16_t y, std::string message)
{
	execute(MapCoords(x, y), message);
}
void SkillExecution::execute(MapCoords map_coords, std::string message)
{
	_map_coords = map_coords;
	_message = message;
	start_execution(SKTT_GROUND_TARGETED);
}


void SkillExecution::set_map(std::shared_ptr<Map> map)
{
	_map = map;
	_map_container_thread = map->container();
	_lua_mgr = sZone->get_component_of_type<ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM);
	_lua_state = sZone->get_component_of_type<ScriptManager>(Horizon::System::RUNTIME_SCRIPTVM)->lua_state();
}