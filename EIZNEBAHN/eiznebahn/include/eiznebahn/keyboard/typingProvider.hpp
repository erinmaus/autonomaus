// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_KEYBOARD_TYPING_PROVIDER_HPP
#define EIZNEBAHN_KEYBOARD_TYPING_PROVIDER_HPP

#include <set>
#include "eiznebahn/input/inputProvider.hpp"
#include "eiznebahn/input/inputQueue.hpp"
#include "eiznebahn/keyboard/keySequence.hpp"

namespace eiznebahn
{
	class TypingProvider : public InputProvider<InputQueue::channel_keyboard>
	{
	public:
		explicit TypingProvider(const KeySequence& sequence);
		~TypingProvider() = default;

		void update(float delta, InputSpoofer& spoofer) override;
		void cancel() override;
		bool done() const override;

	private:
		KeySequence sequence;
		std::size_t current_index = 0;
		float current_time = 0.0f;
		float target_time;

		static bool is_modifier(int key_code);

		std::set<int> keys;
		std::set<int> modifiers;

		bool canceled = false;
		bool pending;
	};
}

#endif
