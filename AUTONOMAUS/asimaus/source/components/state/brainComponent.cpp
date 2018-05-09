// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/log.hpp"
#include "autonomaus/components/state/brainComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/gui.hpp"
#include "autonomaus/state/memory/brain.hpp"

autonomaus::BrainComponent::BrainComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

autonomaus::BrainComponent::~BrainComponent()
{
	pending_result.cancel();
}

void autonomaus::BrainComponent::reload()
{
	// Nothing.
}

void autonomaus::BrainComponent::show()
{
	this->visible = true;
}

void autonomaus::BrainComponent::update()
{
	if (this->updating)
	{
		switch (pending_result.get())
		{
			case Cortex::Result::Status::failure:
				Log::warn("Action failed!");
				this->updating = false;
				this->was_updating = true;
				break;
			case Cortex::Result::Status::success:
				Log::info("Action succeeded!");
				this->updating = false;
				this->was_updating = true;
				if (pending_result.message())
				{
					this->message_result = pending_result.message();
				}
				break;
			case Cortex::Result::Status::empty:
				Log::warn("Action empty.");
				this->updating = false;
				this->was_updating = false;
				break;
			default:
				// Nothing.
				break;
		}
	}

	if (this->visible)
	{
		draw();
	}
}

std::string autonomaus::BrainComponent::get_name() const
{
	return "Brain Prodding Component";
}

void autonomaus::BrainComponent::draw()
{
	auto brain = get_autonomaus().get_userdata<Brain>();
	if (brain == nullptr)
	{
		Log::warn("No brain.");
		this->visible = false;
		return;
	}

	ImGui::Begin("Brain Prodder 2000", &this->visible);
	{
		if (this->was_updating || this->updating)
		{
			ImGui::Text("Poke");
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Status: ");
			ImGui::SameLine();
			switch (pending_result.get())
			{
				case Cortex::Result::Status::failure:
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "failed");
					break;
				case Cortex::Result::Status::success:
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "succeeded");
					break;
				case Cortex::Result::Status::working:
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "working...");
					break;
				default:
					ImGui::Text("none");
					break;
			}

			if (this->updating && ImGui::Button("Stop"))
			{
				pending_result.cancel();
				this->updating = false;
				this->was_updating = false;
			}
		}

		if (this->message_result)
		{
			ImGui::Text("Pull");
			ImGui::Separator();
			ImGui::Spacing();

			for (auto& i: this->message_result)
			{
				if (i.second.length() == 1)
				{
					ImGui::Text("%s = %s", i.first.c_str(), i.second.get_string().c_str());
				}
				else
				{
					ImGui::Text("%s", i.first.c_str());
					for (std::size_t j = 0; j < i.second.length(); ++j)
					{
						ImGui::BulletText("%s", i.second.get_string(j).c_str());
					}
				}
			}
		}

		if (this->was_updating || this->message_result)
		{
			ImGui::Separator();
			ImGui::Spacing();
		}

		ImGui::PushItemWidth(192);
		{
			ImGui::InputString("Cortex", this->cortex);
			if (brain->get(this->cortex) != nullptr)
			{
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Cortex exists.");
			}
			else
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Cortex not found.");
			}

			ImGui::InputString("Action", this->action);
			ImGui::InputString("Field", this->field);
			ImGui::InputString("Value", this->value);
			if (ImGui::InputInt("Index", &this->index))
			{
				this->index = std::max(this->index, 0);
			}
		}
		ImGui::PopItemWidth();

		if (ImGui::Button("Add") && !this->field.empty())
		{
			message.set_string(this->field, this->value, this->index);
			this->field.clear();
			this->value.clear();
		}

		if (ImGui::CollapsingHeader("Message"))
		{
			std::string pending;
			for (auto& i: this->message)
			{
				ImGui::PushID(i.first.c_str());
				if (ImGui::Button("-"))
				{
					pending = i.first;
				}

				ImGui::SameLine();

				if (i.second.length() == 1)
				{
					ImGui::Text("%s = %s", i.first.c_str(), i.second.get_string().c_str());
				}
				else
				{
					ImGui::Text("%s", i.first.c_str());
					for (std::size_t j = 0; j < i.second.length(); ++j)
					{
						ImGui::BulletText("%s", i.second.get_string(j).c_str());
					}
				}

				ImGui::PopID();
			}

			if (!pending.empty())
			{
				this->message.unset(pending);
			}
		}

		if (ImGui::Button("Poke"))
		{
			auto cortex = brain->get(this->cortex);
			if (cortex != nullptr)
			{
				this->pending_result = cortex->poke(this->action, this->message);
				this->message_result = Message();
				this->updating = true;
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Pull"))
		{
			auto cortex = brain->get(this->cortex);
			if (cortex != nullptr)
			{
				this->message_result = cortex->pull(this->action, this->message);
			}
		}
	}
	ImGui::End();
}
