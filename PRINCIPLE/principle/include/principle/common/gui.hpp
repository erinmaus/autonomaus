// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_GUI_HPP
#define PRINCIPLE_COMMON_GUI_HPP

#include <string>
#include "principle/deps/imgui/imgui.h"

namespace ImGui
{
	bool InputString(
		const char* label,
		std::string& buffer,
		std::size_t buffer_size = 512,
		ImGuiInputTextFlags flags = 0,
		ImGuiTextEditCallback callback = nullptr,
		void* user_data = nullptr);
}

#endif
