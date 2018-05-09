// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_OVERLAY_HPP
#define AUTONOMAUS_GRAPHICS_OVERLAY_HPP

#include <cmath>
#include <memory>
#include <set>
#include <map>
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/graphics/renderer.hpp"

namespace autonomaus
{
	class Overlay
	{
	public:
		const static float DEFAULT_LIFESPAN;

		struct Tag
		{
		public:
			friend class Overlay;
			Tag() = default;
			Tag(const Tag& other);
			~Tag();

			bool valid() const;

			Tag& operator =(const Tag& other);

		private:
			typedef int ID;
			Tag(Overlay* overlay, ID id);

			void unref();
			void ref();

			struct Data
			{
				Overlay* overlay;
				ID id;
				int count = 1;
			};
			Data* data = nullptr;
		};

		explicit Overlay(Renderer& renderer, const DrawState& draw_state);
		virtual ~Overlay() = default;

		virtual void unload_resources() = 0;
		virtual void load_resources() = 0;

		float get_fade_duration() const;
		void set_fade_duration(float value);

		Tag allocate();
		void free(Tag& tag);

		virtual bool start(Tag& tag, bool clear_if_exists = true, float lifespan = DEFAULT_LIFESPAN);
		virtual void clear();
		virtual void stop();

		void draw(float time);

		const DrawState& get_draw_state() const;

	protected:
		struct DrawObject
		{
			virtual ~DrawObject() = default;
			virtual void draw() = 0;
		};
		typedef std::unique_ptr<DrawObject> DrawObjectPointer;
		void add_draw(DrawObject* draw);

		virtual void start_draw(int id, float age, float lifespan) = 0;
		virtual void stop_draw() = 0;

		Renderer& get_renderer() const;

	private:
		Renderer* renderer;
		const DrawState* draw_state;

		std::set<Tag::ID> current_tags;

		struct Draw
		{
			std::vector<DrawObjectPointer> objects;
			float age = 0.0f;
			float lifespan = DEFAULT_LIFESPAN;
		};
		std::map<Tag::ID, Draw> draws;

		static const Tag::ID TAG_ID_NONE;
		static const Tag::ID TAG_ID_BASE;
		Tag::ID next_tag_id = TAG_ID_BASE;
		Tag::ID current_tag_id = TAG_ID_NONE;

		float last_draw = HUGE_VALF;
		float fade_duration = 0.5f;
	};
}

#endif
