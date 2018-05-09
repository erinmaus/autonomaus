// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GUI_STATE_BUILDER_HPP
#define CHOMSKY_GUI_GUI_STATE_BUILDER_HPP

#include <stack>
#include <string>
#include <glm/glm.hpp>
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/resources/textureDictionary.hpp"
#include "kommon/utility/lightSharedPointer.hpp"

namespace chomsky
{
	class GUIElement;

	class GUIStateBuilder
	{
	public:
		GUIStateBuilder(const TextureDictionary& texture_dictionary);
		~GUIStateBuilder() = default;

		const GUIContent& get_current() const;
		const GUIContent& get_parent() const;

		void enter(bool copy = true);
		void enter_field(const std::string& name);
		void enter_array();
		void enter_array(std::size_t index);

		void discard();
		void replace();
		void leave();
		void leave(std::size_t index);

		void make_widget(const std::string& name);

		void collect(const GUIElement& element);
		void add_tag(const std::string& tag);
		void add_quad(int x, int y, int width, int height);
		void add_text(const std::string& text, const glm::vec4& color = glm::vec4(1.0f));

		void set_failed(const GUIElement& value);
		const GUIElement& get_failed() const;

		const GUIContent& get_root() const;

	private:
		const TextureDictionary* texture_dictionary;

		typedef kommon::Cow<GUIContent> GUIContentPointer;
		struct StackEntry
		{
			StackEntry();
			explicit StackEntry(const std::string& name, bool is_array = false);
			explicit StackEntry(const GUIContent& content, bool is_array = false);

			GUIContentPointer content;
			bool is_array = false;
		};
		std::vector<StackEntry> content_stack;
		GUIElement failed_element;
	};
}

#endif
