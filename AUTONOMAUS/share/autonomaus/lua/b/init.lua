-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local create_reference = B.Reference
local REFERENCE_PROXY = {}

function B.Reference(class, name)
	if class and name then
		return create_reference(class .. "::" .. name)
	else
		return REFERENCE_PROXY
	end
end

function B.Node(name)
	local Metatable = {}
	function Metatable:__newindex(key, value)
		if value == REFERENCE_PROXY then
			rawset(self, key, create_reference(name .. "::" .. tostring(key)))
		else
			rawset(self, key, value)
		end
	end

	local n = { name = name }
	return setmetatable(n, Metatable)
end

function B.Action(name)
	return B.Node(name)
end

local Output = { Type = {} }
do
	function Output.Type:__index(key)
		if type(key) == 'number' then
			rawset(self, "_field", key)
			return self
		else
			return nil
		end
	end

	function Output:__index(key)
		return setmetatable({ _key = key:upper(), _field = nil }, Output.Type)
	end

	function Output:__newindex(...)
		error("B.Output cannot be assigned a value")
	end

	function Output:__call(t)
		if type(t) ~= "table" then
			error("B.Output table syntax requires table")
		end

		return setmetatable({ _table = t }, Output.Type)
	end
end

B.Output = setmetatable({ Type = Output.Type }, Output)
B.TreeBuilder = require "autonomaus.b.treeBuilder"
B.Task = require "autonomaus.b.task"
