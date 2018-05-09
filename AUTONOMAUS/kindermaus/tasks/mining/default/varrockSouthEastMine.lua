-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local SimpleMiningTask = require "kindermaus.tasks.mining.default.simpleMiningTask"
local T = SimpleMiningTask(
	"Mining (Varrock South-East Mine)",
	"East Bank, Varrock, Misthalin",
	"South-East Mine, Varrock, Misthalin"
)

function T.Match(action)
	local requirements = A.Game.Requirements(action)
	local ore = { requirements:outputs("item") }
	if #ore ~= 1 then
		return false
	end

	if ore[1].name ~= "Tin ore" or
	   ore[1].name ~= "Copper ore" or
	   ore[1].name ~= "Iron ore"
	then
		return false
	end

	return true
end

return T
