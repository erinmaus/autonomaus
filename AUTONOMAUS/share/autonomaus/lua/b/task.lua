-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local TreeBuilder = require "autonomaus.b.treeBuilder"

local Task = {}
function Task.Cost()
	return math.huge
end

function Task.Match()
	return true
end

function Task:priority()
	return -math.huge
end

function Task:update()
	-- Nothing.
end

function Task:begin(executor)
	if self.OnBegin:empty() then
		return B.Status.Success
	else
		return self.OnBegin:execute(executor)
	end
end

function Task:run(executor)
	if self.OnRun:empty() then
		return B.Status.Success
	else
		return self.OnRun:execute(executor)
	end
end

function Task:yield(executor)
	if self.OnYield:empty() then
		return B.Status.Success
	else
		return self.OnYield:execute(executor)
	end
end

function Task:resume(executor)
	if self.OnResume:empty() then
		return self:begin()
	else
		return self.OnResume:execute(executor)
	end
end

local create_task = B.Task

local Module = {}
Module.__index = Module
Module.Tasks = { ['*'] = {} }

function Module.Get(action, key)
	if action ~= nil then
		local a = string.format("%s.%s", action.type, action.name)
		T = Module.Tasks[a] or {}
	else
		T = Module.Tasks['*']
	end

	if key ~= nil then
		return T[key]
	else
		local r = {}
		for _, value in pairs(T) do
			table.insert(r, value)
		end
		return unpack(r)
	end
end

function Module.Best(action, count)
	if action.task ~= nil then
		return Module.Get(action, action.task)
	end

	local a = string.format("%s.%s", action.type, action.name)
	local r = { Module.Get(action) }

	local best = { task = nil, cost = math.huge }
	for i = 1, #r do
		if r[i].Match(action) then
			local cost = r[i].Cost(action, count)
			if cost < best.cost then
				best.task = r[i]
				best.cost = cost
			end
		end
	end

	return best.task
end

function Module:__call(action, name, super)
	local Trees = {}
	Trees.OnBegin = B.TreeBuilder.Root()
	Trees.OnRun = B.TreeBuilder.Root()
	Trees.OnYield = B.TreeBuilder.Root()
	Trees.OnResume = B.TreeBuilder.Root()

	local Derived = { super = Task, name = name, action = action }
	local root = B.Tree()

	local Metatable = {}
	function Metatable:__index(key)
		local s = rawget(self, key)
		if s then
			return s
		else
			return Derived[key] or Task[key] or Trees[key]
		end
	end

	function Metatable:__newindex(key, value)
		if value == B.Reference() then
			Derived[key] = B.Reference(name, key)
		elseif getmetatable(value) == B.TreeBuilder.Type then
			Trees[key] = value
		else
			Derived[key] = value
		end
	end

	function Metatable:__call(...)
		local instance = {}
		local aliases = {}

		for key, value in pairs(Derived) do
			if B.is_local(value) then
				local v = root:tlocal(name .. "::" .. key)
				aliases[value] = v
				instance[key] = v
			end
		end

		for tree in pairs(Trees) do
			if #Trees[tree]._arguments == 0 then
				instance[tree] = B.Tree()
			else
				instance[tree] = TreeBuilder.materialize(Trees[tree], aliases)
			end
		end

		setmetatable(instance, Metatable)

		if self.new or self.instantiate then
			local n = select("#", ...)
			if n == 2 then-- state, message
				self.instantiate(instance, ...)
			else -- state, action, count, ...
				self.new(instance, ...)
			end
		end

		return instance
	end

	local T
	do
		if Module.Tasks['*'][name] then
			errorf("Task '%s' (for action '%s') already defined.", name, action)
		else
			T = setmetatable({}, Metatable)
			create_task(name, T)

			if not Module.Tasks[action] then
				Module.Tasks[action] = {}
			end
			Module.Tasks[action][name] = T
			Module.Tasks['*'][name] = T
		end
	end

	return T
end

return setmetatable({}, Module)
