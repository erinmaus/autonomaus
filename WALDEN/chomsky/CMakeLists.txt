cmake_minimum_required(VERSION 3.6)
project(WALDEN)

find_package(Kompvter REQUIRED)
find_package(Kontinent1 REQUIRED)
find_package(Kvlc REQUIRED)
find_package(Kvre REQUIRED)

set(CHOMSKY_INCLUDE_FILES
	"include/chomsky/chomsky.hpp"
	"include/chomsky/resources/silhouette.hpp"
	"include/chomsky/resources/modelVertexCloud.hpp"
	"include/chomsky/resources/tileSpacePointCloud.hpp"
	"include/chomsky/world/animatedActorWorldUpdatePass.hpp"
	"include/chomsky/world/animatedSilhouette.hpp"
	"include/chomsky/world/bindPoseActorWorldUpdatePass.hpp"
	"include/chomsky/world/bindPoseSilhouette.hpp"
	"include/chomsky/world/terrainElevationWorldUpdatePass.hpp"
	"include/chomsky/world/propWorldUpdatePass.hpp"
	"include/chomsky/world/sceneryElevationWorldUpdatePass.hpp"
	"include/chomsky/world/sceneryWorldUpdatePass.hpp"
	"include/chomsky/world/tileElevationExtents.hpp"
	"include/chomsky/world/worldState.hpp"
	"include/chomsky/world/worldUpdateManager.hpp"
	"include/chomsky/world/worldUpdatePass.hpp"
	"include/chomsky/world/detail/actor.hpp"
	"include/chomsky/world/detail/common.hpp"
	"include/chomsky/world/detail/drawResourceCollection.hpp"
	"include/chomsky/world/detail/prop.hpp"
	"include/chomsky/world/detail/scenery.hpp"
	"include/chomsky/world/detail/tagManager.hpp")

set(CHOMSKY_SOURCE_FILES
	"source/resources/silhouette.cpp"
	"source/resources/modelVertexCloud.cpp"
	"source/resources/tileSpacePointCloud.cpp"
	"source/world/animatedActorWorldUpdatePass.cpp"
	"source/world/animatedSilhouette.cpp"
	"source/world/bindPoseActorWorldUpdatePass.cpp"
	"source/world/bindPoseSilhouette.cpp"
	"source/world/terrainElevationWorldUpdatePass.cpp"
	"source/world/propWorldUpdatePass.cpp"
	"source/world/sceneryElevationWorldUpdatePass.cpp"
	"source/world/sceneryWorldUpdatePass.cpp"
	"source/world/tileElevationExtents.cpp"
	"source/world/worldUpdateManager.cpp"
	"source/world/worldUpdatePass.cpp"
	"source/world/detail/actor.cpp"
	"source/world/detail/common.cpp"
	"source/world/detail/prop.cpp"
	"source/world/detail/scenery.cpp"
	"source/world/detail/tagManager.cpp")

set(CHOMSKY_INCLUDE_DIRS
	"include")

add_library(Chomsky SHARED
	${CHOMSKY_INCLUDE_FILES}
	${CHOMSKY_SOURCE_FILES})
set_target_properties(Chomsky PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET Chomsky PROPERTY CXX_STANDARD 14)
target_compile_options(Chomsky PRIVATE -Wall)

target_include_directories(Chomsky
	PUBLIC
	${CHOMSKY_INCLUDE_DIRS}
	${Kompvter_INCLUDE_DIRS}
	${Kontinent1_INCLUDE_DIRS}
	${Kvlc_INCLUDE_DIRS}
	${Kvre_INCLUDE_DIRS})
target_link_libraries(Chomsky PUBLIC
	${Kompvter_LIBRARIES}
	${Kontinent1_LIBRARIES}
	${Kvlc_LIBRARIES}
	${Kvre_LIBRARIES})
