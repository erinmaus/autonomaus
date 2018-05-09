// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_INPUT_INPUT_PROVIDER_HPP
#define EIZNEBAHN_INPUT_INPUT_PROVIDER_HPP

#include "eiznebahn/input/inputSpoofer.hpp"

namespace eiznebahn
{
	class BasicInputProvider
	{
	public:
		virtual ~BasicInputProvider() = default;

		virtual void update(float delta, InputSpoofer& spoofer) = 0;
		virtual void cancel() = 0;
		virtual bool done() const = 0;
	};

	template <int Channel>
	class InputProvider : public BasicInputProvider
	{
	public:
		static const int CHANNEL = Channel;
	};
}

#endif
