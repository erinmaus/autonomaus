// This file is a part of EIZNABAHN.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#include <time.h>
#include <unistd.h>

long long rv_get_time()
{
	struct timespec t = { 0 };

	clock_gettime(CLOCK_MONOTONIC, &t);

	return (long long)((t.tv_sec * 1000LL) + (t.tv_nsec / 1000000LL));
}

void rv_sleep(long long period)
{
	usleep(period * 1000);
}
