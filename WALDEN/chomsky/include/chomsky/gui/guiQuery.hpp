// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GUI_QUERY_HPP
#define CHOMSKY_GUI_GUI_QUERY_HPP

#include <string>
#include <vector>
#include "chomsky/gui/guiContent.hpp"

namespace chomsky
{
	class GUIQuery
	{
	public:
		static const std::string WILDCARD;
		static const std::string RECURSIVE_WILDCARD;
		static const std::string INPUT;

		GUIQuery() = default;
		GUIQuery(const std::string& path);
		GUIQuery(const char* path);
		~GUIQuery() = default;

		bool valid() const;
		operator bool() const;

		void set_input(std::size_t index, int value);
		void set_input(std::size_t index, const std::string& value);
		void unset_input(std::size_t index);
		std::size_t get_num_inputs() const;

		bool collect(
			const GUIContent& content,
			std::vector<GUIContent>& result) const;

	private:
		void collect(
			const GUIContent& content,
			std::size_t current_index,
			std::vector<GUIContent>& result) const;

		static const std::size_t NONE;
		struct Match
		{
			enum
			{
				match_field,
				match_index
			};

			explicit Match(int type, std::string name = "*");

			int type;
			std::string name;
			std::size_t name_input = NONE;
			std::string widget = "*";
			std::size_t widget_input = NONE;
			std::string index = "*";
			std::size_t index_input = NONE;
		};

		void parse(const std::string& path);
		static bool is_identifier(char c);

		std::vector<Match> query;
		std::vector<std::string> inputs;
		std::string get_match_name(std::size_t index) const;
		std::string get_match_widget(std::size_t index) const;
		std::string get_match_index(std::size_t index) const;
		std::string get_input(std::size_t index) const;
	};
}

#endif
