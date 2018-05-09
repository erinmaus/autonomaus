// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_GUI_HPP
#define AUTONOMAUS_GRAPHICS_GUI_HPP

#include <string>
#include "autonomaus/deps/imgui/imgui.h"

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
