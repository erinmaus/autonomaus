-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local ProxyCortexPullNode = require "autonomaus.b.proxyCortexPullNode"
local ProxyCortexPokeNode = require "autonomaus.b.proxyCortexPokeNode"

local TreeBuilder = {}
local TreeBuilderNode = {}

local function import(path, args, aliases)
	local real_path = { "kindermaus" }
	local last_component = ""
	for i, m in ipairs(path) do
		local component

		-- GUI.HomeTeleportWindow.Teleport -> gui.homeTeleportWindow.teleport
		if m:upper() == m then
			component = m:lower()
		else
			component = m:sub(1, 1):lower() .. m:sub(2)
		end

		table.insert(real_path, component)
	end

	local success, result = pcall(require, table.concat(real_path, "."))
	if success then
		return result
	else
		local cortex_name = {}
		for i = 1, #path - 1 do
			-- This allows explicitly calling a Cortex if a specialized node has
			-- been created. For example, if the node "Game.World.PokeNearbyObject"
			-- has been explicitly definedm, "Game::World.poke_nearby_object" can
			-- still be invoked via Cortex.Game.World.PokeNearbyObject.
			if i ~= 1 or path[i]:lower() ~= "cortex" then
				table.insert(cortex_name, path[i])
			end
		end
		cortex_name = table.concat(cortex_name, "::")

		local action
		do
			-- Still, something like "GetGUIX" would be ambigiuous: "get_gui_x"
			-- or "get_guix"? So just don't do that, silly!
			local m = path[#path]
			local acronyms = m:gsub("([A-Z]*)([A-Z][a-z])", function(a, b) return a:sub(1, 1) .. a:sub(2):lower() .. b end)
			local trailing_acronyms = acronyms:gsub("([A-Z])([A-Z]+)", function(a, b) return a .. b:lower() end)
			local words = trailing_acronyms:gsub("([A-Z])([a-z]*)", function(a, b) return a:lower() .. b .. "_" end)
			local duplicate_underscores = words:gsub("(__*)", function(a) return "_" end)
			local trailing_underscore = duplicate_underscores:gsub("^(.*)_$", function(a) return a end)
			action = trailing_underscore:lower()
		end

		if A.Cortex.get(cortex_name) then
			local pull = action:match("get_(.*)")
			if pull then
				return ProxyCortexPullNode(cortex_name, pull, args, aliases)
			else
				return ProxyCortexPokeNode(cortex_name, action, args, aliases)
			end
		end

		error(string.format("Failed to import node %s.", table.concat(path, ".")))
	end
end

local function materialize(tree, definition, class, node, aliases, e, depth)
	e = e or {}
	depth = depth or 0
	if e[definition] and not e.warned then
		A.Log.warn("warning: tree possibly recursive")
		e.warned = true
	else
		e[definition] = definition
	end

	depth = depth + 1

	local children = definition._arguments
	for i = 1, #children do
		if getmetatable(children[i]) ~= TreeBuilderNode then
			error(string.format("child %d of node %s is not valid", i, table.concat(definition[i]._path)))
		end

		local c = import(children[i]._path, children[i]._arguments, aliases)
		local n = tree:child(node, c)
		materialize(tree, children[i], c, n, aliases, e, depth)
	end

	local wires = definition._arguments
	for key, value in pairs(wires) do
		local v
		local s
		if B.is_reference(value) then
			v = value
			s = B.get_wire_name(v)
		elseif B.is_local(value) then
			v = aliases[value] or value
			s = B.get_wire_name(v)
		elseif getmetatable(value) == B.Output.Type then
			if not value._table then
				v = class[value._key:upper()]
				s = B.get_wire_name(v)
			end
		elseif type(key) ~= 'number' then
			v = tree:constant(value)
			s = tostring(value)
		end

		if v then
			if B.is_reference(key) then
				tree:output(node, v, key)
			elseif B.is_local(key) then
				tree:output(node, v, aliases[key] or key)
			elseif type(key) == "string" then
				local k = class[key:upper()]
				if B.is_reference(k) then
					tree:input(node, v, k)
				end
			end
		end
	end
end

function TreeBuilderNode:__index(key)
	return TreeBuilder.Node(self._path, key)
end

function TreeBuilderNode:__call(args)
	if type(args) ~= "table" then
		errorf("expected table (%s)", table.concat(self._path, "."))
	end

	for key, value in pairs(args) do
		if type(key) ~= "number" then
			self._arguments[key] = value
		end
	end

	for index, value in ipairs(args) do
		table.insert(self._arguments, value)
	end

	return self
end

TreeBuilder.Type = TreeBuilderNode

function TreeBuilder.Root()
	return TreeBuilder.Node()
end

function TreeBuilder.Node(parent, name)
	parent = parent or {}

	local r = {
		_path = { unpack(parent) },
		_children = {},
		_arguments = {}
	}

	table.insert(r._path, name)

	return setmetatable(r, TreeBuilderNode)
end

function TreeBuilder.materialize(node, aliases)
	if getmetatable(node) ~= TreeBuilderNode then
		error(debug.traceback("expected TreeBuilder"))
	end

	aliases = aliases or {}

	local tree = B.Tree()
	do
		local root = node._arguments[1]
		local class = import(root._path, root._arguments, aliases)
		local root = tree:child(class)
		materialize(tree, node, class, root, aliases)
	end

	return tree
end

return TreeBuilder
