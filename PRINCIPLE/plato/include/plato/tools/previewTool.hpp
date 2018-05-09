// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_TOOLS_PREVIEW_TOOL_HPP
#define PLATO_TOOLS_PREVIEW_TOOL_HPP

#include <functional>
#include <glm/glm.hpp>
#include "plato/tools/tool.hpp"

namespace plato
{
	class PreviewTool : public Tool
	{
	public:
		PreviewTool(Toolbox& toolbox);

		void draw(ToolRenderer& renderer) override;

	private:
		void draw(ToolRenderer& renderer, const mapp::Location& location);
	};
}

#endif
