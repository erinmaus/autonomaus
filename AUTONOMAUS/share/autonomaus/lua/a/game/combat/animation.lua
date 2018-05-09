-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Module = {}
Module.COOLDOWN = 0.6

local Animation = {}
function Animation:new(npc, animation, cooldown)
	self.npc = npc
	self.animation = animation
	self.timeout = 0
	self.cooldown = cooldown or Module.COOLDOWN
	self.count = 0
end

function Animation:reset()
	self.count = 0
	self.timeout = A.System.time() + Module.COOLDOWN
end

function Animation:match(count)
	return self.count >= count
end

function Animation:update()
	if A.System.time() < self.timeout then
		return
	end

	local world = A.Cortex.get("Game::World")
	local result = A.Cortex.pull(world, "nearby_npcs", {
		name = self.npc,
		visible = true
	})

	local id, animations
	if result then
		local r = {}
		if type(result.id) == 'table' then
			r = result.id
		else
			r = { result.id }
		end

		for i = 1, #r do
			local a = A.Cortex.pull(world, "npc_animations", { id = r[i] })
			if a then
				if a.result == "target" or type(a.result) == "string" then
					id = r[i]
					animations = { a.result }
					break
				elseif type(a.result) == "table" then
					for j = 1, #a.result do
						if a.result[i] == "target" then
							id = r[i]
							animations = a.result
							break
						end
					end

					if id then
						break
					end
				end
			end
		end
	end

	if id then
		for i = 1, #animations do
			if animations[i] == self.animation then
				self.count = self.count + 1
				self.timeout = A.System.time() + Module.COOLDOWN
				print("match", self.animation)
				break
			end
		end
	end
end

local function constructor(_, npc, animation)
	local instance = setmetatable({}, { __index = Animation })
	instance:new(npc, animation)

	return instance
end

return setmetatable(Module, { __call = constructor })
