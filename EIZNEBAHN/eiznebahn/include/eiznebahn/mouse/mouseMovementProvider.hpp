// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_MOUSE_MOUSE_MOVEMENT_PROVIDER_HPP
#define EIZNEBAHN_MOUSE_MOUSE_MOVEMENT_PROVIDER_HPP

#include "eiznebahn/input/inputProvider.hpp"
#include "eiznebahn/input/inputQueue.hpp"
#include "eiznebahn/mouse/mousePath.hpp"

namespace eiznebahn
{
	class MouseMovementProvider : public InputProvider<InputQueue::channel_mouse>
	{
	public:
		explicit MouseMovementProvider(const MousePath& path);
		~MouseMovementProvider() = default;

		void update(float delta, InputSpoofer& spoofer) override;
		void cancel() override;
		bool done() const override;

	private:
		MousePath path;
		float current_time = 0.0f;
		float target_time = 0.0f;
		std::size_t current_index = 0;

		bool pending;
	};
}

#endif
