// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include "principle/common/gui.hpp"

bool ImGui::InputString(
	const char* label,
	std::string& buffer,
	std::size_t buffer_size,
	ImGuiInputTextFlags flags,
	ImGuiTextEditCallback callback,
	void* user_data)
{
	std::string value = buffer;
	value.resize(std::max(buffer_size, buffer.length() * 2), 0);

	if (ImGui::InputText(label, &value[0], value.length(), flags, callback, user_data))
	{
		// Ensure we don't include the padding.
		buffer = value.c_str();
		return true;
	}

	return false;
}
