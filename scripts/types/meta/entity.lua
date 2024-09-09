---@meta unit

---@param unit Unit
---@return Monster
function cast_unit_to_monster(unit) end

---@class Intelligence
Intelligence = {}

---@return integer # Total amount of intelligence that the unit has
function Intelligence:get() end

---@class CurrentHP
CurrentHP = {}

---@param amount integer How much hp to subtract
function CurrentHP:sub(amount) end

---@class Status
Status = {}

---@return CurrentHP
function Status:current_hp() end

---@return Intelligence
function Status:intelligence() end

---@class Unit
Unit = {}

-- TODO: add unit_type enum
---@return integer # the unit type value
function Unit:type() end

---@return boolean # Returns true if unit is dead
function Unit:is_dead() end

---@return Map
function Unit:map() end

---@return MapCoords
function Unit:map_coords() end

---@return Status
function Unit:status() end

---@return integer # Unit's unique id
function Unit:guid() end

---@param target Unit
---@param skill_id integer
---@param skill_lv integer
---@param start_time integer
---@param attack_motion integer
---@param attacked_motion integer
---@param damage integer
---@param count integer
---@param action_type DmgActionType
function Unit:show_skill_damage(target, skill_id, skill_lv, start_time, attack_motion, attacked_motion, damage, count, action_type) end

---@param target Unit
---@param skill_id integer
---@param cast_time integer
---@param element Element
function Unit:cast_skill_at_target(target, skill_id, cast_time, element) end

---@param unit Unit
---@param range integer
---@return boolean
function Unit:is_in_range_of(unit, range) end

---@return Combat? # Instance of combat if in combat
function Unit:combat() end
