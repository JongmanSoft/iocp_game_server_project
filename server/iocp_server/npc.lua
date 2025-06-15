myid = 99999

function set_uid(x)
    myid = x
end

function find_free_position(collision_array, rx, ry)
    local x, y
    local found = false
    for i = 1, 10 do
        for j = 1, 10 do
            local array_i = rx + i - 1
            local array_j = ry + j - 1
            if array_i <= 100 and array_j <= 100 and not collision_array[array_i][array_j] then
                x = array_j
                y = array_i
                found = true
                break
            end
        end
        if found then break end
    end
    if not found then
        x = rx
        y = ry
    end
    return x, y
end

function set_init_npc_(o_type, zone, collision_array)
    local x, y
    if o_type == 1 then
        x = 49 + ((zone & 20) * 100)
        y = 34 + (math.floor(zone / 20) * 100)
    elseif o_type == 2 then
        local rx = math.random(10, 80)
        local ry = math.random(40, 90)
        x, y = find_free_position(collision_array, rx, ry)
        x = x+((zone & 20) * 100)
        y = y+ (math.floor(zone / 20) * 100)
    elseif o_type == 3 then
        local rx = math.random(0, 90)
        local ry = math.random(0, 25)
        x, y = find_free_position(collision_array, rx, ry)
        x = x+((zone & 20) * 100)
        y = y+ (math.floor(zone / 20) * 100)
    end
    return x, y
end