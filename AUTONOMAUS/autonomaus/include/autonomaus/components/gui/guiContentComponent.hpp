// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_GUI_GUI_CONTENT_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_GUI_GUI_CONTENT_COMPONENT_HPP

#include <functional>
#include <vector>
#include "autonomaus/components/component.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"
#include "autonomaus/state/gui.hpp"
#include "chomsky/gui/guiContent.hpp"

namespace autonomaus
{
	class GUIContentComponent : public Component
	{
	public:
		explicit GUIContentComponent(Autonomaus& autonomaus);
		~GUIContentComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		GUI gui;
		bool is_empty = true;

		bool visible = false;
		void draw();
		void draw_content(const chomsky::GUIContent& content);

		void highlight(int x, int y, int width, int height);
	};
}

#endif
