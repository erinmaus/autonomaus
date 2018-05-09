// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef JAQUES_PLATFORM_HPP
#define JAQUES_PLATFORM_HPP

#include <string>
#include "ganymede/plugin.hpp"

namespace jaques
{
	ganymede::CreatePluginProc load_plugin(const std::string& filename);
}

#endif
