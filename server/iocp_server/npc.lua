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
            if array_i <= 100 and array_i> 0 and array_j> 0 and array_j <= 100 and not collision_array[array_j][array_i] then
                x = array_j-1
                y = array_i-1
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
    local row = math.floor(zone / 20)
    local col = zone % 20
    if o_type == 1 then
        x = 49 + (col * 100)
        y = 34 + (row * 100)
    elseif o_type == 2 then
        local rx = math.random(0, 99)
        local ry = math.random(30, 99)
        y, x = find_free_position(collision_array, rx, ry)
        x = x+(col * 100)
        y = y+ (row * 100)
    elseif o_type == 3 then
        local rx = math.random(0, 25)
        local ry = math.random(0, 99)
        x, y = find_free_position(collision_array, rx, ry)
        x = x+(col * 100)
        y = y+ (row * 100)
    end
    return x, y
end

function event_hello(player)

   my_x = api_get_x(myid);
   my_y = api_get_y(myid);
   player_x = api_get_x(player);
   player_y = api_get_y(player);
 
   if (player_x == my_x) then
      if (player_y == my_y) then
         local  random_message = math.random(0,3)
         if random_message == 0 then
        api_sendHello(myid, player, "뭐하는거지? 어서 인간들을 잡아라!");
        elseif random_message == 1 then
         api_sendHello(myid, player, "인간녀석들...그동안 우릴 무시했겠다!");
        elseif random_message == 2 then
         api_sendHello(myid, player, "인간들에게 본때를 보여주는거다!");
        elseif random_message == 3 then
        api_sendHello(myid, player, "너도 어느덧 늠름한 전사가 되었구나! 자! 가는거다!");
        end
          
      end
   end
end