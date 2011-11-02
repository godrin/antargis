
cmake_minimum_required(VERSION 2.8)
project(antargis2)

find_package(SDL)
find_package(Ruby)
FIND_PACKAGE(SWIG REQUIRED)

if ( NOT SDL_FOUND )
   message ( FATAL_ERROR "Make sure that SDL is installed" )
endif ( NOT SDL_FOUND )

link_libraries (
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${SDLMIXER_LIBRARY}
   SDLmain
)
include_directories(${SDL_INCLUDE_DIR} ${RUBY_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE(${SWIG_USE_FILE})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(example.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(example.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(example ruby example.i example.cxx)
SWIG_LINK_LIBRARIES(example ${PYTHON_LIBRARIES})
set_target_properties(example PROPERTIES PREFIX "")

add_subdirectory(lib)

add_executable(antargis2 main.cpp ${mylib})
