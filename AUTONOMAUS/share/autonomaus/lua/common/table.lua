-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

function table.clone(t, e)
	e = e or {}

	local r = {}
	for key, value in pairs(t) do
		local k, v

		if type(key) == 'table' then
			if e[key] then
				k = e[key]
			else
				k = table.clone(key, e)
				e[key] = k
			end
		else
			k = key
		end

		if type(value) == 'table' then
			if e[value] then
				v = e[value]
			else
				v = table.clone(value, e)
				e[key] = v
			end
		else
			v = value
		end

		r[k] = v
	end

	setmetatable(r, getmetatable(t))

	return r
end
