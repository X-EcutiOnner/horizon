local player, monster, with_drops, with_exp = ...

local find_monster_by_id = require("scripts/modules/find_monster_by_id")

local m_config = find_monster_by_id(monster:unit():job_id())

print(player, monster, with_drops, with_exp)

print("Player " .. player:unit():name() .. " Gained EXP: " .. m_config["Exp"] .. " Job EXP: " .. m_config["JExp"])

player_status = player:unit():status()

local stage = player:unit():combat_registry():create_combat_stage(os.time())
stage:push_attribute_operation(player:unit(), monster:unit(), player_status:base_experience():attribute(), AttributeOperationType.AddToBase, m_config["Exp"])
stage:push_attribute_operation(player:unit(), monster:unit(), player_status:job_experience():attribute(), AttributeOperationType.AddToBase, m_config["JExp"])
player:unit():combat_registry():queue_combat_stage(stage)

local drops = m_config["Drops"]
math.randomseed(os.time())
if drops then
    -- for loop to iterate through the drops
    -- iterate key and value pairs in a table
    for key, value in pairs(drops) do
        local random_number = math.random(1, 10000) -- Generate a random number between 1 and 10000
        if random_number <= value then -- If the random number is less than or equal to the drop chance
            -- Add the item to the player's inventory
            find_item_by_name = require("scripts/modules/find_item_by_name")
            item_config = find_item_by_name(key)
            player:unit():map():add_new_item_drop(item_config["Id"], monster:unit():map_coords(), 1, 0);
        end
    end
end

