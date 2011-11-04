
cmake_minimum_required(VERSION 2.8)
project(antargis2)

find_package(SDL)
find_package(SDL_image)
find_package(SDL_ttf)
find_package(SDL_mixer)
find_package(Ruby)
find_package(ZLIB)
find_package(OpenGL)
find_package(PNG)
FIND_PACKAGE(SWIG REQUIRED)

if ( NOT SDL_FOUND )
   message ( FATAL_ERROR "Make sure that SDL is installed" )
endif ( NOT SDL_FOUND )

link_libraries (
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${SDLMIXER_LIBRARY}
   ${SDLTTF_LIBRARY}
   SDLmain
   ${ZLIB_LIBRARY}
   ${RUBY_LIBRARY}
   ${PNG_LIBRAY}
   ${OPENGL_gl_LIBRARY}
   ${OPENGL_glu_LIBRARY}
)
include_directories(${SDL_INCLUDE_DIR} ${RUBY_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/lib)

INCLUDE(${SWIG_USE_FILE})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(example.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(example.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(example ruby example.i example.cxx)
SWIG_LINK_LIBRARIES(example ${PYTHON_LIBRARIES})
set_target_properties(example PROPERTIES PREFIX "")

#add_subdirectory(lib)


FILE(GLOB MYLIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} lib/*.cc lib/*.c)
FILE(GLOB MYLIB_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} lib/*.h)

#add_library(mylib )

add_executable(antargis2 main.cpp ${MYLIB_SOURCES})
#target_link_libraries(antargis2 mylib)
