cmake_minimum_required(VERSION 2.8.12.2)
project(tapankaikki)
set(PROJECT_VERSION 4.09)

if(NOT MINGW)
	set(CMAKE_CXX_FLAGS -std=c++11 ${CMAKE_CXX_FLAGS})
else()
	# Workaround for http://stackoverflow.com/q/38538924/3425536
	set(CMAKE_CXX_FLAGS -std=gnu++11 ${CMAKE_CXX_FLAGS})
	# Add -mwindows to the linker flags to avoid opening a console window when the
	# program is started (see https://cygwin.com/ml/cygwin/2007-04/msg00027.html)
	set(CMAKE_EXE_LINKER_FLAGS -mwindows)
endif()

# dependencies ###########################################################################
find_package(SDL 1 REQUIRED)
find_package(SDL_image 1 REQUIRED)
find_package(SDL_mixer 1 REQUIRED)
find_package(SDL_net 1 REQUIRED)
include_directories(
	${SDL_INCLUDE_DIR}
	${SDL_IMAGE_INCLUDE_DIRS}
	${SDL_MIXER_INCLUDE_DIRS}
	${SDL_NET_INCLUDE_DIRS})
include_directories() # Temporary workaround for 'X11/Xlib.h not found'
if(APPLE)
	add_definitions(-DDISABLE_GAMMA) # SDL_SetGamma causes segfaults occasionally
endif()


# On Windows native win32 api is used and therefore GTK+ is not needed
if(NOT WIN32)
	find_package(GTK2)
	include_directories(${GTK2_INCLUDE_DIRS})
endif()


# common #################################################################################
file(GLOB COMMON_SOURCES src/common/*.h src/common/*.cpp)
if(WIN32)
	add_library(tk4-common SHARED ${COMMON_SOURCES})
	# Export symbols, default behaviour is import
	target_compile_definitions(tk4-common PUBLIC -DCOMMON_EXPORTS)
else()
	add_library(tk4-common STATIC ${COMMON_SOURCES})
endif()
target_link_libraries(tk4-common ${SDL_LIBRARY} ${SDL_IMAGE_LIBRARIES})


# game ###################################################################################
file(GLOB GAME_SOURCES src/*.h src/*.cpp src/*.rc)
if(WIN32)
	list(REMOVE_ITEM GAME_SOURCES ${CMAKE_SOURCE_DIR}/src/lists_unix.cpp)
else()
	list(REMOVE_ITEM GAME_SOURCES ${CMAKE_SOURCE_DIR}/src/lists_win32.cpp)
endif()
add_executable(tk4 ${GAME_SOURCES})
target_link_libraries(tk4 tk4-common ${SDL_LIBRARY} ${SDL_IMAGE_LIBRARIES} ${SDL_MIXER_LIBRARIES} ${SDL_NET_LIBRARIES})


# editor #################################################################################
file(GLOB EDITOR_SOURCES src/Editor/*.h src/Editor/*.cpp src/Editor/*.rc)
if(WIN32)
	list(REMOVE_ITEM EDITOR_SOURCES ${CMAKE_SOURCE_DIR}/src/Editor/EditorLevelIO_gtk.cpp)
else()
	list(REMOVE_ITEM EDITOR_SOURCES ${CMAKE_SOURCE_DIR}/src/Editor/EditorLevelIO_win32.cpp)
endif()

target_link_libraries(tk4-common ${SDL_LIBRARY} ${SDL_IMAGE_LIBRARIES} ${GTK2_LIBRARIES})


# efpconv ################################################################################
file(GLOB EFPCONV_SOURCES src/efpconv/*.h src/efpconv/*.cpp)
add_executable(efpconv ${EFPCONV_SOURCES})
target_link_libraries(efpconv tk4-common ${SDL_LIBRARY} ${SDL_IMAGE_LIBRARIES})


# installation configuration #############################################################
if(UNIX)
	include(GNUInstallDirs)
	install(TARGETS tk4 efpconv DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(DIRECTORY data/ DESTINATION ${CMAKE_INSTALL_DATADIR}/tk4)
	add_definitions(-DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}/tk4")
endif()
