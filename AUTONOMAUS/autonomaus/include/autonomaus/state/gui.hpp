// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_GUI_HPP
#define AUTONOMAUS_STATE_GUI_HPP

#include <cstddef>
#include <map>
#include <string>
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiContent.hpp"

namespace autonomaus
{
	class GUI
	{
	public:
		friend class GUIProvider;

		GUI() = default;
		~GUI() = default;

		bool has(const std::string& name) const;
		bool valid(const std::string& name) const;
		const chomsky::GUIContent& get(const std::string& name) const;
		std::string get_error(const std::string& name);

		std::size_t get_num_elements() const;
		const chomsky::GUIElement& get_element(std::size_t index) const;

	private:
		std::vector<chomsky::GUIElement> elements;

		typedef std::map<std::string, chomsky::GUIContent> ContentMap;
		ContentMap contents;
		chomsky::GUIContent empty_content;

		std::map<std::string, std::string> compile_errors;

	public:
		typedef ContentMap::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
