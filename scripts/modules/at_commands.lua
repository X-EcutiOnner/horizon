-- At command functions!

-- @storage
local function storage(player, args)
	storage = player:storage(tonumber(args[2]))
	storage:notify_all()
end

-- @zeny
local function zeny(player, args)
	status = player:unit():status()
	status:zeny():set(tonumber(args[2]))
end
-- @statpoint
local function statpoint(player, args)
	status = player:unit():status()
	status:status_point():set(tonumber(args[2]))
end
-- @skillpoint
local function skillpoint(player, args)
	status = player:unit():status()
	status:skill_point():set(tonumber(args[2]))
end
-- @resetskillpoints
local function resetskillpoints(player, args)
	status = player:unit():status()
	status:skill_point():set(0)
end
-- @job
local function job(player, args)
	player:job_change(tonumber(args[2]))
end

local function speed(player, args)
	if args[2] == nil or tonumber(args[2]) < 0 or tonumber(args[2]) > 2000 then
		player:message("Please enter a speed between 0 and 2000.")
		return false
	end

	player:message("Movement speed has been set to " .. args[2])
	player:unit():status():movement_speed():set(tonumber(args[2]))

	return true
end

-- @warp/mapmove/rura/movemap <map> <x> <y>
-- @jump
local function mapmove(player, args)
	local map, coords = args[2], MapCoords.new(0, 0)
	local map_obj = nil
	--print(args[1], args[2], args[3], args[4])
	if args[1] ~= "jump" then
		if map == nil then
			player:message("Please specify a map name!")
			return false
		end

		map_obj = get_map_by_name(map)

		if map_obj == nil then
			player:message("Map " .. map .. " does not exist!")
			return false
		end

		if args[3] ~= nil or args[4] ~= nil then
			coords = MapCoords.new(tonumber(args[3]), tonumber(args[4]))
		end
		player:move_to_map(map_obj, coords)
		player:message("Moved to " .. map .. " (" .. player:map_coords():x() .. ", " .. player:map_coords():y() .. ")")
	else
		map_obj = player:unit():map()
		player:move_to_map(map_obj, coords)
		coords = player:map_coords()
		player:message("Jumped to " .. coords:x() .. " " .. coords:y())
	end
	return true
end

local function item(player, args)
	local inventory = player:inventory()
	local item_id, amount = tonumber(args[2]), tonumber(args[3])
	if item_id == nil or amount == nil or item_id <= 0 then
		player:message("Usage: @item <item_id> <amount>")
		return false
	end
	if amount <= 0 then
		player:message("The amount must be greater than 0.")
		return false
	end
	local result = inventory:add_item(item_id, amount, true)
	if result == 1 then
		player:message("Item with Id " .. item_id .. " does not exist.")
	elseif result == 2 then
		player:message("Could not product the item as you are overweight.")
	elseif result == 4 then
		player:message("You do not have enough inventory space.")
	elseif result == 5 then
		player:message("The specified amount is over the allowed quantity.")
	elseif result == 7 then
		player:message("The specified amount is over the allowed stack size.")
	elseif result ~= 0 then
		player:message("An unknown error occured during the item production!")
	end
	return true
end

local function iteminfo(player, args)
	local item_id = tonumber(args[2])
	if item_id <= 0 then
		player:message("Usage: @iteminfo <item_id>")
		return false
	end

	local item = get_item_info(item_id)

	if item == nil then
		player:message("Item with ID " .. item_id .. " was not found.")
		return true
	end

	player:message("Item '" .. item.name .. "'/'" .. item.name .. "[".. item.card_slot_count .. "] (".. item.item_id ..") Type: ".. item.type)
	player:message("Value Buy: " .. item.value_buy .. " | Value Sell: " .. item.value_sell)
	player:message("Weight: " .. tonumber(item.weight / 10))
end

local function change_base_level_cmd(player, args)
	local blvl = player:unit():status():base_level()
	local bexp = player:unit():status():base_experience()
	local nbexp = player:unit():status():next_base_experience()
	local amount = tonumber(args[2])
	if amount == nil or amount == 0 then
		player:message("Usage: @blvl <amount>")
	end
	if amount > 0 then
		for i = 1, amount do
			if (blvl:get() >= MAX_LEVEL) then
				nbexp:set(99999999, true) -- Aegis sets 99.9M value on reaching max level.
				bexp:set(0, true)
				player:message("Max Level " .. MAX_LEVEL .. " reached.")
				break
			end
			bexp:set(nbexp:get(), true)
		end
	else
		amount = math.abs(amount)
		if (blvl:get() - amount <= 1) then
			blvl:set(1, true)
		else
			blvl:set(blvl:get() - amount, true)
		end
		bexp:set(0, true)
	end
end
-- @go <map_name/map_id>
local function go(player, args)
	local var = args[2]
	local map_obj = nil
  
	local go_to = nil
	local locations = {
	  [0] = {map = "prontera", coords = {156, 191}},
	  [1] = {map = "morocc", coords = {156, 93}},
	  [2] = {map = "geffen", coords = {119, 59}},
	  [3] = {map = "payon", coords = {162, 233}},
	  [4] = {map = "alberta", coords = {192, 147}},
	  [5] = {map = "izlude", coords = {128, 114}},
	  [6] = {map = "aldebaran", coords = {140, 131}},
	  [7] = {map = "lutie", coords = {147, 134}},
	  [8] = {map = "comodo", coords = {209, 143}},
	  [9] = {map = "yuno", coords = {157, 51}},
	  [10] = {map = "amatsu", coords = {198, 84}},
	  [11] = {map = "gonryun", coords = {160, 120}},
	  [12] = {map = "umbala", coords = {89, 157}},
	  [13] = {map = "niflheim", coords = {21, 153}},
	  [14] = {map = "louyang", coords = {217, 147}},
	  [15] = {map = "new_1-1", coords = {53, 111}},
	  [16] = {map = "sec_pri", coords = {23, 61}},
	  [17] = {map = "jawaii", coords = {249, 127}},
	  [18] = {map = "ayothaya", coords = {151, 117}},
	  [19] = {map = "einbroch", coords = {64, 200}},
	  [20] = {map = "lighthalzen", coords = {158, 92}},
	  [21] = {map = "einbech", coords = {70, 95}},
	  [22] = {map = "hugel", coords = {96, 145}},
	  [23] = {map = "rachel", coords = {130, 110}},
	  [24] = {map = "veins", coords = {216, 123}},
	  [25] = {map = "moscovia", coords = {223, 184}},
	  [26] = {map = "mid_camp", coords = {180, 240}},
	  [27] = {map = "manuk", coords = {282, 138}},
	  [28] = {map = "splendide", coords = {201, 147}},
	  [29] = {map = "brasilis", coords = {182, 239}},
	  [30] = {map = "dicastes01", coords = {198, 187}},
	  [31] = {map = "mora", coords = {44, 151}},
	  [32] = {map = "dewata", coords = {200, 180}},
	  [33] = {map = "malangdo", coords = {140, 114}},
	  [34] = {map = "malaya", coords = {242, 211}},
	  [35] = {map = "eclage", coords = {110, 39}},
	  [36] = {map = "lasagna", coords = {193, 182}},
	}
  
	-- Check if the input is a number or a map name
	if tonumber(var) and locations[tonumber(var)] then
	  go_to = tonumber(var)
	else
	  -- If not a number, find the index matching the map name
	  for index, location in pairs(locations) do
		  if location.map == var then
			  go_to = index
			  break
		  end
	  end
	end
  
	local location = locations[go_to]
	if location then
		  map_obj = get_map_by_name(location.map)
		  if map_obj == nil then
			  player:message("Map " .. map .. " does not exist!")
			  return false
		  else
			  player:move_to_map(map_obj, MapCoords.new(location.coords[1], location.coords[2]))
		  end
	else
		  player:message("Example : @go prontera or @go 0")
		  player:message("=============== Town List  ===============")
		  player:message("0  : Prontera")
		  player:message("1  : Morroc")
		  player:message("2  : Geffen")
		  player:message("3  : Payon")
		  player:message("4  : Alberta")
		  player:message("5  : Izlude")
		  player:message("6  : Aldebaran")
		  player:message("7  : Lutie")
		  player:message("8  : Comodo")
		  player:message("9  : Yuno")
		  player:message("10 : Amatsu")
		  player:message("11 : Gonryun")
		  player:message("12 : Umbala")
		  player:message("13 : Niflheim")
		  player:message("14 : Louyang")
		  player:message("15 : Training Ground")
		  player:message("16 : Prison")
		  player:message("17 : Jawaii")
		  player:message("18 : Ayothaya")
		  player:message("19 : Einbroch")
		  player:message("20 : Lighthalzen")
		  player:message("21 : Einbech")
		  player:message("22 : Hugel")
		  player:message("23 : Rachel")
		  player:message("24 : Veins")
		  player:message("25 : Moscovia")
		  player:message("26 : Rune Midgard Allied Forces Post")
		  player:message("27 : Manuk")
		  player:message("28 : Splendide")
		  player:message("29 : Brasilis")
		  player:message("30 : El Dicastes")
		  player:message("31 : Mora")
		  player:message("32 : Dewata")
		  player:message("33 : Malangdo")
		  player:message("34 : Malaya")
		  player:message("35 : Ecllage")
		  player:message("36 : Lasagna")
		  player:message("Please specify a valid map name or map ID! Look above list!")
	end
  
	return true
  end

local function reinitialize_state(player)
	player:reinitialize_state()
	player:message("Script/LUA state has been reloaded.")
end

local at_commands = {
	player = nil,
	cmd = nil,
	player_guid = 0,
	cmd_args = nil,

	-- COMMAND FUNCTIONS --
	cmd_tbl = {
		["speed"] = speed,
		["reloadstate"] = reinitialize_state,
		["mapmove"] = mapmove,
		["rura"] = mapmove,
		["warp"] = mapmove,
		["jump"] = mapmove,
		["item"] = item,
		["blvl"] = change_base_level_cmd,
		["iteminfo"] = iteminfo,
		["job"] = job,
		["statpoint"] = statpoint,
		["skillpoint"] = skillpoint,
		["resetskillpoints"] = resetskillpoints,
		["go"] = go,
		["zeny"] = zeny
	}
}

function at_commands:new (o)
	o = o or at_commands
	setmetatable(o, self)
	self.__index = self
	return o
end

function at_commands:init(player, cmd)
	self.player = player
	self.player_guid = player:unit():guid()
	self.cmd = cmd
	self.cmd_args = explode(" ", cmd)
end

function at_commands:run()
	if self.cmd_tbl[self.cmd_args[1]] ~= nil then
		self.cmd_tbl[self.cmd_args[1]](self.player, self.cmd_args)
	else
		print("Command " .. self.cmd_args[1] .. " was not found.")
	end
end

return at_commands
