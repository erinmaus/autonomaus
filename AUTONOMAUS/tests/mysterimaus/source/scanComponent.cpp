// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/deps/imgui/imgui.h"
#include "mysterimaus/scanComponent.hpp"
#include "mysterimaus/deps/imgui/imgui_memory_editor.h"

autonomaus::ScanComponent::ScanComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

void autonomaus::ScanComponent::reload()
{
	// Nothing.
}

void autonomaus::ScanComponent::show()
{
	visible = true;
}

void autonomaus::ScanComponent::update()
{
	if (visible)
	{
		draw();
	}
}

std::string autonomaus::ScanComponent::get_name() const
{
	return "Mysterimaus Scan";
}

void autonomaus::ScanComponent::draw()
{
	ImGui::Begin("Scanner Derply", &visible);
	{
		static int value = 0;
		static int size = 4;
		static int mode = Scan::scan_equal;

		if (ImGui::Button("Reset"))
		{
			scan.reset();
		}

		if (ImGui::Button("Scan"))
		{
			scan.scan(value, size, mode);
		}

		ImGui::InputInt("Value", &value);
		ImGui::InputInt("Size", &size);

		ImGui::Text("Mode");
		{
			ImGui::SameLine();
			ImGui::RadioButton("==", &mode, Scan::scan_equal);

			ImGui::SameLine();
			ImGui::RadioButton("!=", &mode, Scan::scan_not_equal);

			ImGui::SameLine();
			ImGui::RadioButton("<", &mode, Scan::scan_less);

			ImGui::SameLine();
			ImGui::RadioButton("<=", &mode, Scan::scan_less_equal);

			ImGui::SameLine();
			ImGui::RadioButton(">", &mode, Scan::scan_greater);

			ImGui::SameLine();
			ImGui::RadioButton(">=", &mode, Scan::scan_greater_equal);
		}

		if (!scan.empty())
		{
			ImGui::Separator();
			ImGui::Spacing();

			if (scan.count() == 1)
			{
				auto record = scan.get(0);
				static MemoryEditor editor;
				editor.ReadOnly = true;
				editor.GotoAddr = record.get_offset();

				auto allocation = record.get_allocation();
				ImGui::Text("Size: %lu bytes", allocation->size);
				for (auto i = 0; i < mysterimaus::MAX_BACKTRACE; ++i)
				{
					ImGui::BulletText("%p\n", allocation->backtrace[i]);
				}

				editor.DrawContents((std::uint8_t*)allocation->pointer, allocation->size);
			}
			else
			{
				ImGui::Text("%lu results", scan.count());
			}
		}
	}
	ImGui::End();
}
