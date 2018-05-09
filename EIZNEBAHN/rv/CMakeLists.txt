cmake_minimum_required(VERSION 3.4)
project(rv)

set(TARGET_UI_LIB "X11" CACHE STRING "Target UI API to use (e.g., X11)")

if(${TARGET_UI_LIB} STREQUAL "X11")
	find_package(X11 REQUIRED)

	if(NOT X11_XTest_FOUND)
		message(FATAL_ERROR "XTest extension required.")
	endif(NOT X11_XTest_FOUND)

	find_package(PNG REQUIRED)

	set(rv_INCLUDE_PATH ${X11_INCLUDE_DIRS} ${X11_XTest_INCLUDE_PATH} ${PNG_INCLUDE_DIRS})
	set(rv_LIBRARIES ${X11_LIBRARIES} ${X11_XTest_LIB} ${PNG_LIBRARIES})

	file (GLOB rv_SOURCE source/x11/*.c)
else(${TARGET_UI_LIB} STREQUAL "X11")
	message(FATAL_ERROR "Unsupported target: ${TARGET_UI_LIB}")
endif(${TARGET_UI_LIB} STREQUAL "X11")

add_library(rv SHARED ${rv_SOURCE})
target_link_libraries(rv ${rv_LIBRARIES})
target_include_directories(rv PUBLIC ${rv_INCLUDE_PATH} include)
