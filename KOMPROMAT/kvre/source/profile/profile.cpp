// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvre/profile/profile.hpp"
#include "kvre/profile/profiles/v1Profile.hpp"

kvre::Profile* kvre::Profile::current_profile = nullptr;

const kvre::Profile* kvre::Profile::get_default()
{
	if (current_profile == nullptr)
	{
		current_profile = new V1Profile();
	}

	return current_profile;
}
