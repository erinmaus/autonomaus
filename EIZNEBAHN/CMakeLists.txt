cmake_minimum_required(VERSION 3.6)
project(EIZNEBAHN)

if (NOT HDT_BUILD_TYPE STREQUAL "Package")
	add_subdirectory(deadhand)
endif()
add_subdirectory(rv)