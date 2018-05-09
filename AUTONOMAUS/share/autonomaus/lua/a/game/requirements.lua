-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Requirements = {}

function Requirements:new(action)
	self.requirements = action:requirements()
end

function Requirements:resource(type)
	local v = self:resources(type)
	return v
end

function Requirements:resources(type)
	local r = {}
	for i = 1, #self.requirements do
		if self.requirements[i].resource.type:lower() == type:lower() then
			table.insert(r, self.requirements[i].resource)
		end
	end

	return unpack(r)
end

function Requirements:input(type)
	local r = self:inputs(type)
	return r
end

function Requirements:inputs(type)
	local r = {}
	for i = 1, #self.requirements do
		if self.requirements[i].is_input
		   and self.requirements[i].resource.type:lower() == type:lower()
		then
			local j = table.clone(self.requirements[i].resource)
			j.requirement = table.clone(self.requirements[i])
			j.requirement.resource = nil

			table.insert(r, j)
		end
	end

	return unpack(r)
end

function Requirements:output(type)
	local r = self:outputs(type)
	return r
end

function Requirements:outputs(type)
	local r = {}
	for i = 1, #self.requirements do
		if self.requirements[i].is_output
		   and self.requirements[i].resource.type:lower() == type:lower()
		then
			local j = table.clone(self.requirements[i].resource)
			j.requirement = table.clone(self.requirements[i])
			j.requirement.resource = nil

			table.insert(r, j)
		end
	end

	return unpack(r)
end

return function(...)
	local r = setmetatable({}, { __index = Requirements })
	r:new(...)

	return r
end
