// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_COMMON_RE_HPP
#define ROSALIND_COMMON_RE_HPP

#include <glm/glm.hpp>

namespace rosalind
{
	namespace re
	{
		struct ViewTransform
		{
			glm::vec4 uCameraPosition;
			glm::mat4 uViewMatrix;
			glm::mat4 uProjectionMatrix;
			glm::mat4 uViewProjMatrix;
			glm::vec4 uZBufferParams;
		};

		static const int ITEM_ICON_WIDTH = 36;
		static const int ITEM_ICON_HEIGHT = 32;
	}
}

#endif
