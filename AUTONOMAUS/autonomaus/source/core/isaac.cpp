// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdio>
#include "autonomaus/core/isaac.hpp"
#include "autonomaus/core/log.hpp"

#ifdef AUTONOMAUS_BUILD_POSIX
#include <dlfcn.h>
#include <link.h>
#endif

autonomaus::Isaac::Isaac(Autonomaus& autonomaus, LibraryHandle handle)
{
	this->autonomaus = &autonomaus;
	this->handle = handle;
}

autonomaus::Autonomaus& autonomaus::Isaac::get_autonomaus() const
{
	return *this->autonomaus;
}

#ifdef AUTONOMAUS_BUILD_POSIX
#include <cstdio>
#include <stdlib.h>

static std::string copy(const std::string& source)
{
	static char directory_template[] = "/tmp/hdt-XXXXXX";
	static char* directory = mkdtemp(directory_template);
	static int index = 0;

	char filename[256];
	std::snprintf(filename, sizeof(filename), "%s/isaac%d.so", directory, index);
	++index;

	FILE* input = std::fopen(source.c_str(), "rb");
	FILE* output = std::fopen(filename, "wb");
	char buffer[1024];
	while (!std::feof(input))
	{
		auto count = std::fread(buffer, 1, sizeof(buffer), input);
		std::fwrite(buffer, 1, count, output);
	}

	std::fclose(input);
	std::fclose(output);

	autonomaus::Log::info("Copied Isaac module from '{0}' to '{1}'.", source, filename);

	return filename;
}
#endif

autonomaus::Isaac* autonomaus::Isaac::load(
	Autonomaus& autonomaus,
	const std::string& filename)
{
	Isaac* result = nullptr;

	Log::info("Creating Isaac module '{0}'...", filename);

#ifdef AUTONOMAUS_BUILD_POSIX
	// We need to ensure the shared object is always reloaded from disk.
	// So force it by copying the source library to a new location.
	//
	// ...Amazing.
	auto library_filename = copy(filename);

	auto handle = dlopen(library_filename.c_str(), RTLD_LAZY);
	if (!handle)
	{
		auto error_message = dlerror();
		if (error_message != nullptr)
		{
			Log::critical("couldn't load Isaac module '{0}' ('{1}'): {2}",
				filename, library_filename, error_message);
		}
		else
		{
			Log::critical(
				"couldn't load Isaac module '{0}' ('{1}')",
				filename, library_filename);
		}

		return nullptr;
	}

	auto create_symbol = dlsym(handle, "create_isaac_module");
	if (create_symbol == nullptr)
	{
		auto error_message = dlerror();
		if (error_message != nullptr)
		{
			Log::critical(
				"couldn't load symbol 'create_isaac_module' from Isaac module '{0}': {1}",
				filename, error_message);
		}
		else
		{
			Log::critical(
				"couldn't load symbol 'create_isaac_module' from Isaac module '{0}'",
				filename);
		}

		dlclose(handle);
		return nullptr;
	}

	auto destroy_symbol = dlsym(handle, "destroy_isaac_module");
	if (destroy_symbol == nullptr)
	{
		auto error_message = dlerror();
		if (error_message != nullptr)
		{
			Log::critical(
				"couldn't load symbol 'destroy_isaac_module' from Isaac module '{0}': {1}",
				filename, error_message);
		}
		else
		{
			Log::critical(
				"couldn't load symbol 'destroy_isaac_module' from Isaac module '{0}'",
				filename);
		}

		dlclose(handle);
		return nullptr;
	}

	try
	{
		typedef Isaac* (* CreateIsaacProc)(Autonomaus&, LibraryHandle);
		auto func = (CreateIsaacProc)create_symbol;
		result = func(autonomaus, (LibraryHandle)handle);
	}
	catch (const std::exception& e)
	{
		Log::critical(
			"couldn't create Isaac module: {0}",
			e.what());
		dlclose(handle);
	}
#elif AUTONOMAUS_BUILD_WINDOWS
	return nullptr;
#else
	#error not-yet-implemented
#endif

	Log::info("Created Isaac module ({0}).", (void*)result);

	return result;
}

void autonomaus::Isaac::unload(Isaac* isaac)
{
#ifdef AUTONOMAUS_BUILD_POSIX
	if (isaac == nullptr)
	{
		return;
	}

	Log::info("Unloading Isaac module ({0})...", (void*)isaac);

	auto handle = isaac->handle;
	auto destroy_symbol = dlsym((void*)handle, "destroy_isaac_module");
	assert(destroy_symbol != nullptr);

	typedef Isaac* (* DestroyIsaacProc)(Isaac*);
	auto func = (DestroyIsaacProc)destroy_symbol;

	func(isaac);

	dlclose((void*)handle);

	Log::info("Unloaded Isaac module.");
#else
	// Nothing.
#endif
}
