-- This file is a part of [hdt].
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

package.path = "mashina/?.lua;mashina/init.lua;" .. package.path

if os.is("bsd") then
	premake.platforms["clang"] = {
		cfgsuffix = "",
		iscrosscompiler = true
	}

	table.insert(premake.option.list["platform"].allowed, { "clang", "clang on POSIX" })
	table.insert(premake.fields.platforms.allowed, "clang")

	premake.gcc.platforms["clang"] = {
		cc = "clang",
		cxx = "clang++",
		ar = "ar",
		cppflags = "-std=c++14 -Wall -Wno-mismatched-tags"
	}
end

_DEFAULTS = {
	["hdt-bin-dir"] = ".output/bin",
	["hdt-lib-dir"] = ".output/lib",
	["hdt-obj-dir"] = ".output/obj",
	["hdt-src-dir"] = ".",
	["deps"] = "."
}

newoption {
	trigger     = "hdt-bin-dir",
	value       = "DIR",
	description = "Output directory for binaries. Defaults to .output/bin"
}

newoption {
	trigger     = "hdt-lib-dir",
	value       = "DIR",
	description = "Output directory for libraries. Defaults to .output/lib"
}

newoption {
	trigger     = "hdt-obj-dir",
	value       = "DIR",
	description = "Output directory for build objects. Defaults to .output/obj"
}

newoption {
	trigger     = "hdt-src-dir",
	value       = "DIR",
	description = "Root directory containing module sources. Defaults to current directory."
}

newoption {
	trigger     = "deps",
	value       = "DIR",
	description = "Root directory containing dependencies."
}

function get_option(option)
	assert(type(option) == 'string')
	return _OPTIONS[option] or _DEFAULTS[option] or assert(false, "bad property")
end

function make_path(...)
	local args = { n = select('#', ...), ... }
	local result = ""
	for i = 1, args.n do
		if args[i] ~= nil then
			result = path.join(result, args[i])
		end
	end

	return result
end

HDT_CURRENT_MODULE = false
HDT_CURRENT_PROJECT = false
HDT_CURRENT_MODULE_ROOT = false
function hdt_module(name)
	assert(type(name) == 'string')
	assert(name:match("^[A-Z]+$") ~= nil)

	HDT_CURRENT_MODULE = name
	HDT_CURRENT_MODULE_ROOT = make_path(get_option("hdt-src-dir"), name)
end

HDT_CURRENT_PROJECT_ROOT = false
HDT_CURRENT_PROJECT_OBJ_DIR = false
HDT_CURRENT_PROJECT_TARGET_DIR = false
HDT_CURRENT_PROJECT_SOURCE_DIR = false
HDT_CURRENT_PROJECT_INCLUDE_DIR = false
HDT_PROJECTS = {}

function hdt_get_project(name)
	if HDT_PROJECTS[name] == nil then
		HDT_PROJECTS[name] = {
			dependencies = {},
			is_static = false,
			premake = {
				defines = {},
				includedirs = {},
				buildoptions = {},
				links = {},
				linkoptions = {}
			}
		}
	end

	return HDT_PROJECTS[name]
end

function hdt_depends(names)
	assert(HDT_CURRENT_MODULE)
	assert(HDT_CURRENT_PROJECT)

	local project = hdt_get_project(HDT_CURRENT_PROJECT)
	for i = 1, #names do
		project.dependencies[names[i]] = true
	end
end

local private = {
	defines = defines,
	includedirs = includedirs,
	buildoptions = buildoptions,
	links = links,
	linkoptions = linkoptions,
}

function defines(values)
	private.defines(values)

	if HDT_CURRENT_MODULE and HDT_CURRENT_PROJECT then
		local project = hdt_get_project(HDT_CURRENT_PROJECT)
		for i = 1, #values do
			project.premake.defines[values[i]] = true
		end
	end
end

function includedirs(values)
	assert(HDT_CURRENT_MODULE)
	assert(HDT_CURRENT_PROJECT)

	private.includedirs(values)
	local project = hdt_get_project(HDT_CURRENT_PROJECT)
	for i = 1, #values do
		project.premake.includedirs[values[i]] = true
	end
end

function links(values)
	assert(HDT_CURRENT_MODULE)
	assert(HDT_CURRENT_PROJECT)

	private.links(values)
	local project = hdt_get_project(HDT_CURRENT_PROJECT)
	for i = 1, #values do
		project.premake.links[values[i]] = true
	end
end

function linkoptions(values)
	assert(HDT_CURRENT_MODULE)
	assert(HDT_CURRENT_PROJECT)

	private.linkoptions(values)
	local project = hdt_get_project(HDT_CURRENT_PROJECT)
	for i = 1, #values do
		project.premake.linkoptions[values[i]] = true
	end
end

function buildoptions(values)
	assert(HDT_CURRENT_MODULE)
	assert(HDT_CURRENT_PROJECT)

	private.buildoptions(values)
	local project = hdt_get_project(HDT_CURRENT_PROJECT)
	for i = 1, #values do
		project.premake.buildoptions[values[i]] = true
	end
end

function hdt_project(name, target_kind, target_type)
	assert(HDT_CURRENT_MODULE)
	assert(type(name) == 'string')
	assert(type(target_kind) == 'string')
	assert(target_type == 'bin' or target_type == 'lib')

	local target_root = string.format("hdt-%s-dir", target_type)
	local include_root = make_path(get_option("hdt-src-dir"), HDT_CURRENT_MODULE, name, "include")

	HDT_CURRENT_PROJECT = name
	HDT_CURRENT_PROJECT_OBJ_DIR = make_path(get_option("hdt-obj-dir"), HDT_CURRENT_MODULE, name)
	HDT_CURRENT_PROJECT_TARGET_DIR = make_path(get_option(target_root), HDT_CURRENT_MODULE, name)
	HDT_CURRENT_PROJECT_SOURCE_DIR = make_path(get_option("hdt-src-dir"), HDT_CURRENT_MODULE, name, "source")
	HDT_CURRENT_PROJECT_INCLUDE_DIR = make_path(include_root, name)

	hdt_get_project(name).root_dir = make_path(get_option("hdt-src-dir"), HDT_CURRENT_MODULE, name)

	project(name)
		language "C++"
		kind(target_kind)

		if is_platform("posix") then
			private.buildoptions { "-std=c++14", "-Wno-multichar" }
		end

		if is_platform("windows") then
			defines { "_USE_MATH_DEFINES" }
		end

		if is_platform("windows") then
			if characterset then
				characterset "ASCII"
			end

			flags { "StaticRuntime" }
		end

		configuration "Debug"
			targetsuffix "_debug"
			objdir(make_path(HDT_CURRENT_PROJECT_OBJ_DIR, "debug"))
			targetdir(make_path(HDT_CURRENT_PROJECT_TARGET_DIR, "debug"))
		configuration "Release"
			objdir(make_path(HDT_CURRENT_PROJECT_OBJ_DIR, "release"))
			targetdir(make_path(HDT_CURRENT_PROJECT_TARGET_DIR, "release"))
		configuration "RTM"
			objdir(make_path(HDT_CURRENT_PROJECT_OBJ_DIR, "release"))
			targetdir(make_path(HDT_CURRENT_PROJECT_TARGET_DIR, "release"))
		configuration {}
		includedirs { include_root }
		files {
			make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "**.h"),
			make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "**.hpp"),
			make_path(HDT_CURRENT_PROJECT_SOURCE_DIR, "**.c"),
			make_path(HDT_CURRENT_PROJECT_SOURCE_DIR, "**.cpp")
		}

		if get_option("deps") ~= "." then
			private.includedirs { make_path(get_option("deps"), "include") }

			if is_platform("posix") then
				private.linkoptions { "-L" .. make_path(get_option("deps"), "lib") }
			elseif is_platform("windows") then
				private.linkoptions { "/LIBPATH:" .. make_path(get_option("deps"), "lib") }
			end
		end

		if is_platform("windows") then
			private.buildoptions { "/wd4099" }
		end

end

function hdt_library(name)
	if is_platform("posix") then
		hdt_project(name, "SharedLib", "lib")
	else
		hdt_project(name, "StaticLib", "lib")
	end

	if is_platform("posix") then
		configuration "Debug"
			private.linkoptions {
				string.format("-Wl,-soname,lib%s_debug.so", name),
				"-Wl,-z,defs"
			}
		configuration "Release"
			private.linkoptions {
				string.format("-Wl,-soname,lib%s.so", name),
				"-Wl,-z,defs"
			}
		configuration "RTM"
			private.linkoptions {
				string.format("-Wl,-soname,lib%s.so", name),
				"-Wl,-z,defs"
			}
		configuration {}
	end
end

function hdt_hook(name)
	hdt_project(name, "SharedLib", "lib")

	if is_platform("posix") then
		configuration "Debug"
			private.linkoptions {
				string.format("-Wl,-soname,lib%s_debug.so", name),
				"-Wl,-z,defs"
			}
		configuration "Release"
			private.linkoptions {
				string.format("-Wl,-soname,lib%s.so", name),
				"-Wl,-z,defs"
			}
		configuration "RTM"
			private.linkoptions {
				string.format("-Wl,-soname,lib%s.so", name),
				"-Wl,-z,defs"
			}
		configuration {}
	end
end

function hdt_static_library(name)
	hdt_project(name, "StaticLib", "lib")
		if is_platform("posix") then
			private.buildoptions { "-fPIC" }
		end

	hdt_get_project(name).is_static = true
end

function hdt_application(name)
	hdt_project(name, "WindowedApp", "bin")
		kind "WindowedApp"
end

function hdt_binary(name)
	hdt_project(name, "ConsoleApp", "bin")
end

HDT_MASHINA = {}
function hdt_mashina()
	HDT_MASHINA[HDT_CURRENT_PROJECT] = true
end

function pkg_config_command(library, ...)
	local args = { n = select('#', ...), ... }
	local options = ""
	for i = 1, args.n do
		if args[i] ~= nil then
			options = options .. "--" .. args[i]

			if i ~= args.n then
				options = options .. " "
			end
		end
	end

	return string.format("`pkg-config %s %s`", options, library)
end

function pkg_config(library)
	linkoptions { pkg_config_command(library, "libs") }
	buildoptions { pkg_config_command(library, "cflags") }
end

function is_platform(platform)
	if platform == "posix" then
		return os.is("bsd") or os.is("linux") or os.is("macosx")
	else
		return os.is(platform)
	end
end

function warn(format, ...)
	io.stderr:write("[")
	if HDT_CURRENT_MODULE then
		io.stderr:write(HDT_CURRENT_MODULE)
		if HDT_CURRENT_PROJECT then
			io.stderr:write(":", HDT_CURRENT_PROJECT)
		end
	else
		io.stderr:write("warning")
	end
	io.stderr:write("]", " ")

	io.stderr:write(string.format(format, ...), "\n")
end

-- My name is OZYMANDIAS, king of kings
-- Look on my works, ye mighty, and DESPAIR.
--
--              Percy Shelley, "Ozymandias"
solution "hdt"
	configurations { "Debug", "Release", "RTM" }
	platforms { "x64" }

	configuration "Debug"
		private.defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		private.defines { "DEBUG" }
		flags { "Optimize", "Symbols" }

	configuration "RTM"
		private.defines { "NDEBUG" }
		flags { "Optimize" }

	configuration {}
		if is_platform("posix") then
			private.buildoptions { "-Wall" }
		end

	if is_platform("windows") then
		warn("Windows is not yet supported. Build will fail.")
	end

	hdt_module "AUTONOMAUS"
		hdt_static_library "asimaus"
			hdt_depends { "autonomaus", "mapp", "twoflower" }

			includedirs {
				make_path(get_option("hdt-src-dir"), "BMASHINA", "bmashina", "include"),
				make_path(get_option("hdt-src-dir"), "AUTONOMAUS", "asimaus", "mashina", "output", "include")
			}

			files {
				make_path(get_option("hdt-src-dir"), "AUTONOMAUS", "asimaus", "mashina", "output", "source", "**.cpp")
			}

			if is_platform("posix") then
				pkg_config "lua51"
				links { "boost_coroutine", "boost_context", "yaml-cpp", "lua5.1" }
				links { "m", "boost_context", "boost_regex" }
				links { "ode" }
			else
				warn("linking third-party libraries directly")
				links { "ode", "boost_context", "boost_regex", "yaml-cpp", "lua" }
			end

		hdt_hook "autonomaus"
			if is_platform("posix") then
				defines { "AUTONOMAUS_BUILD_POSIX" }
			else
				if is_platform("windows") then
					defines { "AUTONOMAUS_BUILD_WINDOWS" }
				end
			end

			includedirs {
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps"),
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps", "imgui")
			}

			hdt_depends {
				"ganymede",
				"jaques",
				"rosalind",
				"touchstone",
				"glooper",
				"mapp",
				"eiznebahn",
				"kommon",
				"kompvter",
				"chomsky",
				"thoreau"
			}
		
		hdt_hook "mysterimaus"
			if is_platform("linux") then
				links { "dl" }
			end

		hdt_hook "asimaus_WalkTest2"
			hdt_depends { "asimaus", "autonomaus" }

			files {
				make_path(HDT_CURRENT_MODULE_ROOT, "tests", "walk_test2", "**.cpp")
			}

		hdt_hook "asimaus_Cortex"
			hdt_depends { "asimaus", "autonomaus" }

			files {
				make_path(HDT_CURRENT_MODULE_ROOT, "tests", "cortex", "**.cpp")
			}

		hdt_hook "asimaus_SectorBorder"
			hdt_depends { "asimaus", "autonomaus" }

			files {
				make_path(HDT_CURRENT_MODULE_ROOT, "tests", "sector_border", "**.cpp")
			}

		hdt_binary "asimaus_Lua"
			hdt_depends { "asimaus", "autonomaus" }

			files {
				make_path(HDT_CURRENT_MODULE_ROOT, "tests", "lua", "**.cpp")
			}

		hdt_hook "asimaus_Mysterimaus"
			hdt_depends { "asimaus", "autonomaus", "mysterimaus" }

			includedirs {
				make_path(HDT_CURRENT_MODULE_ROOT, "tests", "mysterimaus", "include")
			}

			files {
				make_path(HDT_CURRENT_MODULE_ROOT, "tests", "mysterimaus", "include", "**.hpp"),
				make_path(HDT_CURRENT_MODULE_ROOT, "tests", "mysterimaus", "source", "**.cpp")
			}

	hdt_module "ARDEN"
		hdt_library "ganymede"
			hdt_depends { "touchstone" }

		hdt_library "jaques"
			hdt_depends {
				"ganymede",
				"touchstone"
			}

			if is_platform("posix") then
				if is_platform("bsd") then
					pkg_config "snappy"
					pkg_config "libturbojpeg"
				else
					links { "snappy", "turbojpeg" }
				end

				defines { "JAQUES_BUILD_POSIX" }
			else
				if is_platform("windows") then
					defines { "JAQUES_BUILD_WINDOWS" }
				end

				warn("linking third-party libraries directly")
				links { "snappy", "turbojpeg" }
			end

		if is_platform("windows") then
			hdt_hook("orlando")
				hdt_depends { "jaques" }

				links { "opengl32" }
		end

		hdt_library "touchstone"
			if is_platform("posix") then
				links { "m" }
				defines { "TOUCHSTONE_BUILD_POSIX" }
			elseif is_platform("windows") then
				defines { "TOUCHSTONE_BUILD_WINDOWS" }
			else
				warn("unknown platform; cannot set Touchstone properties")
			end

			if is_platform("linux") then
				links { "dl" }
			end

		hdt_library "rosalind"
			hdt_depends {
				"touchstone",
				"kompvter"
			}

	hdt_module "BMASHINA"
		--hdt_library "bmashina"

	hdt_module "DISCWORLD"
		hdt_library "glooper"
			hdt_depends {
				"kommon",
				"kompvter",
				"kvre",
				"mantorok",
				"chomsky",
				"thoreau"
			}

			links {
				"boost_system", "boost_filesystem"
			}

			includedirs {
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps", "inih")
			}

		hdt_static_library "mapp"
			hdt_depends { "twoflower", "thoreau" }
			links { "boost_regex" }

		hdt_static_library "twoflower"
			links { "sqlite3" }

	hdt_module "EIZNEBAHN"
		hdt_library "eiznebahn"
			if is_platform("posix") then
				defines { "EIZNEBAHN_BUILD_X11" }
				links { "X11", "m" }

				if is_platform("linux") then
					links { "dl" }
					linkoptions { "-pthread" }
				end
			elseif is_platform("windows") then
				defines { "EIZNEBAHN_BUILD_WIN32" }
			end

		if is_platform("posix") then
			hdt_library "deadhand"
				hdt_depends { "ganymede" }
				if is_platform("bsd") then
					links { "thr" }
				elseif is_platform("linux") then
					linkoptions { "-pthread" }
				end
				links { "X11" }
		end

	hdt_module "KOMPROMAT"
		hdt_library "kommon"
			includedirs {
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps"),
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps", "ctpl")
			}

			hdt_depends {
				"touchstone"
			}

			if is_platform("posix") then
				if is_platform("bsd") then
					pkg_config "snappy"
					pkg_config "libturbojpeg"
					links { "thr" }
				elseif is_platform("linux") then
					links { "snappy", "turbojpeg" }
					linkoptions { "-pthread" }
				end
				
				pkg_config "libpng"
			else
				warn("linking third-party libraries directly")
				links { "snappy", "turbojpeg", "png" }
			end

		hdt_library "kompvter"
			hdt_depends {
				"kommon"
			}

			if is_platform("posix") then
				links { "m" }
			end

			includedirs {
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps"),
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps", "detex")
			}

		hdt_library "kvlc"
			hdt_depends { "kommon" }

			if is_platform("posix") then
				links { "m" }

				links { "ode" }
			else
				warn("linking third-party libraries directly")
				links { "ode" }
			end

		hdt_library "kvncer"
			hdt_depends { "kompvter" }

			if is_platform("posix") then
				pkg_config "glew"
			else
				warn("linking third-party libraries directly")
				links { "glew" }
			end

		hdt_library "kvre"
			hdt_depends {
				"kompvter",
				"kvlc"
			}

	hdt_module "NECRONOMICON"
		hdt_library "mantorok"
			hdt_depends { "kompvter" }

	hdt_module "PRINCIPLE"
		hdt_application "acropolis"
			hdt_depends {
				"continent1",
				"principle",
			}

		hdt_library "continent1"
			hdt_depends {
				"principle"
			}

		hdt_application "continent7"
			hdt_depends {
				"continent1",
				"principle"
			}

		hdt_application "plato"
			hdt_depends {
				"principle",
				"mapp"
			}

			links { "jsoncpp" }

		hdt_application "poltergeist"
			hdt_depends {
				"principle"
			}

		hdt_library "principle"
			hdt_depends {
				"rosalind",
				"glooper",
				"kompvter",
				"kvncer",
				"kvre",
				"mantorok",
				"chomsky",
				"thoreau"
			}

			defines {
				"TOUCHSTONE_NATIVE_GL"
			}

			includedirs {
				make_path(HDT_CURRENT_PROJECT_INCLUDE_DIR, "deps", "imgui")
			}

			if is_platform("posix") then
				if is_platform("bsd") then
					links { "inotify" }
					pkg_config "snappy"
					pkg_config "libturbojpeg"
				else
					links { "snappy", "turbojpeg" }
				end

				links { "GL" }

				pkg_config "allegro-5"
				pkg_config "allegro_dialog-5"
				pkg_config "allegro_image-5"
				pkg_config "allegro_font-5"
				pkg_config "allegro_ttf-5"
				pkg_config "glew"
			else
				if is_platform("windows") then
					links { "opengl32" }
				end

				warn("linking third-party libraries directly")
				links {
					"allegro",
					"allegro_dialog",
					"snappy",
					"turbojpeg",
					"glew"
				}
			end

		hdt_application "zeitgeist"
			hdt_depends {
				"principle"
			}

		hdt_binary "jaegerspiel"
			hdt_depends {
				"rosalind",
				"kompvter",
				"glooper",
				"chomsky"
			}

	hdt_module "WALDEN"
		hdt_library "chomsky"
			hdt_depends {
				"thoreau",
				"kompvter",
				"kvlc",
				"kvre",
				"mantorok"
			}

			if is_platform("posix") then
				pkg_config "libsodium"
			else
				warn("linking third-party libraries directly")
				links { "sodium" }
			end

		hdt_library "thoreau"

-- Resolve dependencies.
for name in pairs(HDT_PROJECTS) do
	project(name)

	local function walk_static_libraries(name, e)
		e = e or {}

		local project = HDT_PROJECTS[name]
		for d in pairs(project.dependencies) do
			local dependency = HDT_PROJECTS[d]
			if dependency.is_static and not e[d] then
				e[d] = true
				private.links({ d })
				for argument in pairs(dependency.premake.links) do
					private.links({ argument })
				end
				walk_static_libraries(d, e)
			end
		end
	end
	walk_static_libraries(name, { name })

	local function walk_dependencies(project, e)
		e = e or {}

		local dependency = HDT_PROJECTS[project]
		for property, value in pairs(dependency.premake) do
			for argument in pairs(value) do
				private[property]({ argument })
			end
		end

		for d in pairs(dependency.dependencies) do
			if not e[d] then
				e[d] = true
				private.links({ d })
				walk_dependencies(d, e)
			end
		end
	end
	walk_dependencies(name, { name })
end
