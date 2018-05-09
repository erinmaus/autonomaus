// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/regex.hpp>
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"
#include "chomsky/gui/guiText.hpp"

autonomaus::ToolTipCortex::ToolTipCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::ToolTip", autonomaus, "*.main.hover-tool-tip:tool-tip"),
	values_query("*.values[*]:tool-tip-value.text")
{
	on_pull(
		"match",
		[this](auto message)
		{
			Message result;
			result.set_boolean("result", this->match(message.get_string("option")));
			return result;
		});

	on_pull(
		"match_any",
		[this](auto message)
		{
			std::vector<std::string> values;
			auto options = message.get("options");
			for (std::size_t i = 0; i < options.length(); ++i)
			{
				values.push_back(options.get_string(i));
			}

			Message result;
			result.set_boolean("result", this->match_any(values));
			return result;
		});
}

std::vector<std::string> autonomaus::ToolTipCortex::get_text() const
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return {};
	}

	std::vector<chomsky::GUIContent> elements;
	if (!this->values_query.collect(root, elements) || elements.empty())
	{
		return {};
	}

	std::vector<std::string> result;
	std::size_t previous_size = 0;
	for (auto& element: elements)
	{
		auto& text = element.get_text();
		for (auto& i: text)
		{
			auto t = std::get<chomsky::GUIText::SEGMENT_STRING>(i);
			if (!t.empty())
			{
				result.push_back(t);
			}
		}

		if (result.size() > previous_size)
		{
			result.back() += "\n";
			previous_size = result.size();
		}
	}

	return result;
}

std::string autonomaus::ToolTipCortex::to_string() const
{
	auto text = get_text();

	std::string result;
	for (auto& i: text)
	{
		result += i;
	}

	return result;
}

bool autonomaus::ToolTipCortex::match(const std::string& value)
{
	return match_any({ value });
}

bool autonomaus::ToolTipCortex::match_any(
	const std::vector<std::string>& values)
{
	if (values.empty())
	{
		return false;
	}

	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return false;
	}

	std::vector<chomsky::GUIContent> elements;
	if (!this->values_query.collect(root, elements) || elements.empty())
	{
		return false;
	}

	for (auto& child: elements)
	{
		for (auto& value: values)
		{
			if (value.empty())
			{
				continue;
			}

			if (fuzzy_string_match(child.get_text().to_string(), value))
			{
				return true;
			}
		}
	}

	return false;
}
