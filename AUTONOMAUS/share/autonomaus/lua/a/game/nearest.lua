-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

return function(wizard, from, resources)
	resources = resources or {}

	local best, plan
	local cost = math.huge
	if from == nil then
		for i = 1, #resources do
			p = A.Game.Plan.goal(wizard, resources[i])
			if p then
				local c = 0
				for j = 1, #p do
					c = c + p[j].cost
				end

				if c < cost then
					best = resources[i]
					plan = p
					cost = c
				end
			end
		end
	else
		for i = 1, #resources do
			p = A.Game.Plan.visit(wizard, from, resources[i])
			if p then
				local c = 0
				for j = 1, #p do
					c = c + p[j].cost
				end

				print(c, resources[i].address)
				if c < cost then
					best = resources[i]
					plan = p
					cost = c
				end
			end
		end
	end

	return best, plan
end
