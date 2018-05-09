-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local find_action = A.Game.Actions.find
function A.Game.Actions.find(type, name)
	return find_action(type, name or "")
end

local register_wizard = A.Game.Wizard

local Wizards = {}
function A.Game.Wizard(name)
	if not Wizards[name] then
		local w = { name = name }
		local instance = register_wizard(w)
		Wizards[name] = { class = w, instance = instance }
	end

	return Wizards[name].class, Wizards[name].instance
end

local goal = A.Game.Plan.goal
function A.Game.Plan.goal(wizard, resource, quantity, step, max_depth)
	if not Wizards[wizard] then
		errorf("Wizard '%s' not defined.", wizard)
	end
	return goal(resource, quantity or 1, step or false, max_depth, Wizards[wizard].instance)
end

local visit = A.Game.Plan.visit
function A.Game.Plan.visit(wizard, from, to)
	if not Wizards[wizard] then
		errorf("Wizard '%s' not defined.", wizard)
	end
	return visit(from, to, Wizards[wizard].instance)
end

A.Game.Plan.nearest = require "autonomaus.a.game.nearest"

A.Game.Cost = require "autonomaus.a.game.cost"
A.Game.Requirements = require "autonomaus.a.game.requirements"
A.Game.Luggage = require "autonomaus.a.game.luggage"

A.Cortex.GUI = {
	ActionBar = require "autonomaus.a.game.gui.actionBar",
	BankWindow = require "autonomaus.a.game.gui.bankWindow",
	HomeTeleportWindow = require "autonomaus.a.game.gui.homeTeleportWindow",
	InventoryPanel = require "autonomaus.a.game.gui.inventoryPanel"
}

A.Game.Combat = {
	Animation = require "autonomaus.a.game.combat.animation"
}

A.Game.Data = {
	Abilities = require "autonomaus.a.game.data.abilities",
	Rotations = require "autonomaus.a.game.data.rotations"
}

-- TODO: Expose Autonomaus's logging facilities.
A.Log = {}
function A.Log.info(message, ...)
	message = string.format(message, ...)
	io.stdout:write(message, "\n")
end
function A.Log.warn(message, ...)
	message = string.format(message, ...)
	io.stdout:write("warning: ", message, "\n")
end
function A.Log.critical(message, ...)
	message = string.format(message, ...)
	io.stdout:write("critical: ", message, "\n")
end

require "autonomaus.a.wizards.travelWithBank"
require "autonomaus.a.wizards.travelWithoutBank"
require "autonomaus.a.wizards.walk"
