// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_DRAW_HOOK_HPP
#define AUTONOMAUS_GRAPHICS_DRAW_HOOK_HPP

#include <cstdint>
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/graphics/renderer.hpp"
#include "touchstone/trace/call.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/immediateDispatch.hpp"

namespace autonomaus
{
	template <typename Overlay>
	class DrawHook
	{
	public:
		friend class Autonomaus;

		explicit DrawHook(
			touchstone::ImmediateDispatch& dispatch,
			DrawState& draw_state);
		virtual ~DrawHook() = default;

		Overlay& before();
		Overlay& after();

		DrawState& get_draw_state() const;

		virtual void invalidate();
		virtual void load();
		virtual void before_call(
			const touchstone::Context& context,
			std::uint32_t token) = 0;
		virtual void after_call(
			const touchstone::Context& context,
			const touchstone::Call* call) = 0;
		virtual void on_flip(const touchstone::Context& context) = 0;

	protected:
		Renderer& get_renderer();
		touchstone::ImmediateDispatch& get_dispatch() const;

	private:
		touchstone::ImmediateDispatch* dispatch;
		DrawState* draw_state;

		Renderer renderer;
		Overlay before_overlay;
		Overlay after_overlay;
	};
}

template <typename Overlay>
autonomaus::DrawHook<Overlay>::DrawHook(
	touchstone::ImmediateDispatch& dispatch, DrawState& draw_state) :
		renderer(dispatch),
		before_overlay(renderer, draw_state),
		after_overlay(renderer, draw_state)
{
	this->dispatch = &dispatch;
	this->draw_state = &draw_state;
}

template <typename Overlay>
Overlay& autonomaus::DrawHook<Overlay>::before()
{
	return this->before_overlay;
}

template <typename Overlay>
Overlay& autonomaus::DrawHook<Overlay>::after()
{
	return this->after_overlay;
}

template <typename Overlay>
autonomaus::DrawState& autonomaus::DrawHook<Overlay>::get_draw_state() const
{
	return *this->draw_state;
}

template <typename Overlay>
void autonomaus::DrawHook<Overlay>::invalidate()
{
	this->before_overlay.unload_resources();
	this->after_overlay.unload_resources();
}

template <typename Overlay>
void autonomaus::DrawHook<Overlay>::load()
{
	this->before_overlay.load_resources();
	this->after_overlay.load_resources();
}

template <typename Overlay>
autonomaus::Renderer& autonomaus::DrawHook<Overlay>::get_renderer()
{
	return this->renderer;
}

template <typename Overlay>
touchstone::ImmediateDispatch& autonomaus::DrawHook<Overlay>::get_dispatch() const
{
	return *this->dispatch;
}

#endif
