// This file is a part of EIZNABAHN.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#ifndef RV_PRIVATE_TIME_H
#define RV_PRIVATE_TIME_H

#ifndef RV_INCLUDE_API
	#error "don't include private headers; use 'rv.h' instead"
#endif

// Gets the clock time since epoch.
//
// The time value is in milliseconds.
long long rv_get_time();

// Sleeps for `period' milliseconds.
void rv_sleep(long long period);

#endif
