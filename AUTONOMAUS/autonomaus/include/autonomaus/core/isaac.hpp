// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_CORE_ISAAC_HPP
#define AUTONOMAUS_CORE_ISAAC_HPP

#include <string>

namespace autonomaus
{
	class Autonomaus;

	class Isaac
	{
	public:
		typedef std::uintptr_t LibraryHandle;

		Isaac(Autonomaus& autonomaus, LibraryHandle handle);
		virtual ~Isaac() = default;

		virtual void update() = 0;

		static Isaac* load(Autonomaus& autonomaus, const std::string& filename);
		static void unload(Isaac* isaac);

	protected:
		Autonomaus& get_autonomaus() const;

	private:
		Autonomaus* autonomaus;
		LibraryHandle handle;
	};
}

#endif
