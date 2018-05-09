-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

-- TODO: Add personality
local Cost = {}
function Cost.Travel(a, b)
	if type(a) == "number" then
		return a
	else
		-- TODO: Use 'visit'
		a = A.Game.Atlas.find(a)[1]
		b = A.Game.Atlas.find(b)[1]

		local dx = a.anchor.x - b.anchor.x
		local dy = a.anchor.y - b.anchor.y

		return math.sqrt(dx ^ 2 + dy ^ 2)
	end
end

function Cost.Skill(a, count, action)
	-- TODO: Add personality.
	-- TODO: Take into account action. (E.g., ticks per item when smithing).
	return count * 1
end

return Cost
