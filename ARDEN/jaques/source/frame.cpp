// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <snappy.h>
#include <turbojpeg.h>
#include "jaques/frame.hpp"
#include "jaques/platform.hpp"
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/immediateDispatch.hpp"

touchstone::TraceBuilder& jaques::Frame::get_trace_builder()
{
	if (!trace_builder_instance)
	{
		trace_builder_instance = std::make_unique<touchstone::TraceBuilder>(get_gl());
	}

	return *trace_builder_instance.get();
}

touchstone::ImmediateDispatch& jaques::Frame::get_gl()
{
	if (!gl_instance)
	{
		gl_instance = std::make_unique<touchstone::ImmediateDispatch>();
	}

	return *gl_instance.get();
}

touchstone::Dispatch& jaques::Frame::get_dispatch()
{
	// TODO: Return dispatch based on some configuration mechanism.
	return get_gl();
}

void jaques::Frame::before_switch_context(std::uintptr_t value, std::uintptr_t window)
{
	current_context = value;
	for_each_plugin(
		[](auto plugin)
		{
			plugin->before_switch_context();
		});
}

void jaques::Frame::after_switch_context(std::uintptr_t value, std::uintptr_t window)
{
	current_context = value;
	for_each_plugin(
		[value, window](auto plugin)
		{
			plugin->after_switch_context(value != 0, window);
		});
}

void jaques::Frame::before_call(std::uint32_t token)
{
	for_each_plugin(
		[token](auto plugin)
		{
			plugin->before_call(token);
		});
}

void jaques::Frame::after_call(const touchstone::Call* call)
{
	for_each_plugin(
		[call](auto plugin)
		{
			plugin->after_call(call);
		});
}

#include "touchstone/gl/calls.hpp"

#include "touchstone/gl/glTrace.hpp"

static void test()
{
	static const int pixels[4];
	touchstone::CallStreamBuilder builder;
	builder.begin<touchstone::call::TexSubImage2D>();
	builder.parameter<touchstone::detail::GLenum>(GL_TEXTURE_2D);
	builder.parameter<touchstone::detail::GLint>(0);
	builder.parameter<touchstone::detail::GLint>(0);
	builder.parameter<touchstone::detail::GLint>(0);
	builder.parameter<touchstone::detail::GLint>(2);
	builder.parameter<touchstone::detail::GLint>(2);
	builder.parameter<touchstone::detail::GLenum>(GL_RGBA);
	builder.parameter<touchstone::detail::GLenum>(GL_UNSIGNED_BYTE);
	builder.parameter<touchstone::detail::GLvoid>(pixels);
	builder.data(sizeof(int) * 4, pixels);
	builder.end<touchstone::call::TexSubImage2D>();

	touchstone::CallStream reader(builder.get_data(), builder.get_size());
	touchstone::GLTrace trace;
	trace.extract(reader);

	printf("success\n");
}

void jaques::Frame::end()
{
	test();

	if (!plugins.loaded)
	{
		load_plugins();
	}

	if (!plugins.values.empty())
	{
		for_each_plugin(
			[](auto plugin)
			{
				plugin->on_flip(get_trace_builder().get_call_stream());
			});
	}

	static auto const trace_filename = std::getenv("JAQUES_TRACE_FILENAME");
	static auto const screenshot_frame = std::getenv("JAQUES_SCREENSHOT_FRAME");

	if (trace_filename != nullptr)
	{
		std::ofstream file_stream(trace_filename, std::ios::app | std::ios::binary);

		auto call_stream = get_trace_builder().get_call_stream();
		std::string input_frame(
			(const char*)call_stream.get_frame_data(),
			call_stream.get_frame_data_size());
		std::string output_frame;
		snappy::Compress(input_frame.data(), input_frame.size(), &output_frame);

		std::uint64_t frame_size = call_stream.get_frame_data_size();
		std::uint64_t compressed_frame_size = output_frame.size();
		file_stream.write((const char*)&frame_size, sizeof(std::uint64_t));
		file_stream.write((const char*)&compressed_frame_size, sizeof(std::uint64_t));
		file_stream.write(
			&output_frame[0],
			compressed_frame_size);

		if (screenshot_frame != nullptr)
		{
			auto& gl = get_gl();

			// TODO: Make this robust.
			// 1) Get backbuffer dimensions from context creation calls.
			// 2) Store the pixel packing state and unbind any pixel unpack buffer.
			// 3) Read pixels and save.
			// 4) Restore old pixel packing state and bind previous pixel unpack buffer.
			GLint viewport[4];
			gl.GetIntegerv(GL_VIEWPORT, viewport);

			auto pixels = std::make_unique<std::uint8_t[]>(
				viewport[2] * viewport[3] * 4);
			gl.ReadPixels(
				viewport[0], viewport[1],
				viewport[2], viewport[3],
				GL_RGBA, GL_UNSIGNED_BYTE,
				pixels.get());

			tjhandle compressor = tjInitCompress();
			unsigned char* screenshot_data;
			unsigned long screenshot_data_size = 0;
			tjCompress2(
				compressor,
				pixels.get(),
				viewport[2], 0, viewport[3], TJPF_RGBA,
				&screenshot_data, &screenshot_data_size,
				TJSAMP_444, 50, TJFLAG_FASTDCT | TJFLAG_BOTTOMUP);

			std::uint64_t screenshot_size = screenshot_data_size;
			file_stream.write((const char*)&screenshot_size, sizeof(std::uint64_t));
			file_stream.write((const char*)screenshot_data, screenshot_size);

			tjDestroy(compressor);
			tjFree(screenshot_data);
		}
		else
		{
			std::uint64_t screenshot_size = 0;
			file_stream.write((const char*)&screenshot_size, sizeof(std::uint64_t));
		}
	}

	printf("size: %lu\n", get_trace_builder().get_call_stream().get_frame_data_size());
	FILE* f = std::fopen("test.log", "a");
	touchstone::GLTrace trace;
	trace.extract(get_trace_builder().get_call_stream());
	for (int i = 0; i < trace.count(); ++i)
	{
		std::fprintf(f, "%s\n", trace.at(i)->get_name());
	}
	std::fprintf(f, "end frame\n\n");
	std::fclose(f);

	get_trace_builder().reset();
}

void jaques::Frame::for_each_plugin(
	const std::function<void(ganymede::Plugin*)>& iter)
{
	if (!plugins.loaded)
	{
		load_plugins();
	}

	for (auto& plugin: plugins.values)
	{
		iter(plugin);
	}
}

void jaques::Frame::load_plugins()
{
	const char* current_plugin = std::getenv("JAQUES_PLUGINS");
	if (current_plugin == nullptr)
	{
		return;
	}

	const char* next_plugin = std::strpbrk(current_plugin, ":");
	do
	{
		std::string filename;
		if (next_plugin == nullptr)
		{
			filename = current_plugin;
			current_plugin = nullptr;
		}
		else
		{
			filename = std::string(current_plugin, next_plugin);

			current_plugin = next_plugin + 1;
			next_plugin = std::strpbrk(next_plugin + 1, ":;");
		}

		if (filename.empty())
		{
			continue;
		}

		auto create_proc = jaques::load_plugin(filename);
#ifndef NDEBUG
		std::printf("loading plugin '%s' ", filename.c_str());
		if (create_proc == nullptr)
		{
			std::printf("failed!");
		}
		else
		{
			std::printf("succeeded.");
		}
		std::putc('\n', stdout);
#endif

		if (create_proc != nullptr)
		{
			auto plugin = create_proc(&get_gl());
			if (plugin != nullptr)
			{
				plugins.values.emplace_back(plugin);
			}
#ifndef NDEBUG
			std::printf("creating plugin '%s' ", filename.c_str());
			if (plugin == nullptr)
			{
				std::printf("failed!");
			}
			else
			{
				std::printf("succeeded.");
			}
			std::putc('\n', stdout);
#endif
		}
	} while(current_plugin != nullptr);

	plugins.loaded = true;
}

jaques::Frame::PluginGuard::~PluginGuard()
{
	std::lock_guard<std::mutex> lock(this->mutex);
	for (auto p: this->values)
	{
		delete p;
	}
	this->values.clear();
}

std::unique_ptr<touchstone::TraceBuilder> jaques::Frame::trace_builder_instance;
std::unique_ptr<touchstone::ImmediateDispatch> jaques::Frame::gl_instance;
std::uintptr_t jaques::Frame::current_context = 0;
jaques::Frame::PluginGuard jaques::Frame::plugins;
