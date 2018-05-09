-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

return function(cortex, pull, args, aliases)
	local Cortex = B.Action("pull::A.Cortex." .. cortex .. "." .. pull)

	function Cortex:update(mashina, state)
		local cortex = A.Cortex.get(cortex)

		local a = {}
		for key, value in pairs(args) do
			if type(key) == "string" then
				a[key] = state[aliases[value] or value] or value
			end
		end

		local result = A.Cortex.pull(cortex, pull, a)
		if not result or next(result) == nil then
			return B.Status.Failure
		end

		function resolve(t, e)
			e = e or {}

			local r = {}
			for key, value in pairs(t) do
				local v
				if type(value) == "table" then
					if e[value] then
						v = e[value]
					elseif getmetatable(value) ~= B.Output.Type then
						resolve(value, e)
					else
						v = value
					end
				end

				if getmetatable(v) == B.Output.Type then
					local k = v
					v = result[k._key:lower()]

					if k._field and type(v) == 'table' then
						v = v[k._field]
					end
				end

				r[key] = v
			end

			return r
		end

		for key, value in pairs(args) do
			if type(key) == "userdata" then
				local v
				if type(value) == "table" then
					if getmetatable(value) == B.Output.Type then
						if value._table then
							v = resolve(value._table)
						else
							v = result[value._key:lower()]
						end
					else
						v = table.clone(value)
					end
				else
					v = value
				end

				state[aliases[key] or key] = v
			end
		end

		return B.Status.Success
	end

	return Cortex
end
