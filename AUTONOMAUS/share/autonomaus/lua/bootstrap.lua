-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

function errorf(...)
	return error(string.format(...), 2)
end

require "autonomaus.a"
require "autonomaus.b"
require "autonomaus.common"

K = B.TreeBuilder.Root()

-- That's all, folks.
