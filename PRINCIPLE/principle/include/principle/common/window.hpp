// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_WINDOW_HPP
#define PRINCIPLE_COMMON_WINDOW_HPP

#include <string>

namespace principle
{
	class Application;

	class Window
	{
	public:
		Window(Application* application);
		virtual ~Window();

		virtual void open();
		virtual void close();

		bool get_is_open() const;
		bool get_is_closed() const;

		virtual void update(float timestamp);
		virtual void draw();

	protected:
		bool get_can_close() const;
		void set_can_close(bool value);

		virtual int get_flags() const;
		virtual std::string get_title() const;
		virtual std::string get_id() const;
		std::string get_id(const std::string& suffix) const;

		virtual void update_gui() = 0;

	private:
		bool focus = false;
		bool is_open = false;
		bool can_close = true;

		Application* application;
	};
}

#endif
