
cmake_minimum_required(VERSION 2.8)
project(antargis2)

find_package(SDL)
find_package(SDL_image)
find_package(SDL_ttf)
find_package(SDL_mixer)
#find_package(Ruby)
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
#   ${RUBY_LIBRARY}
   ${PNG_LIBRAY}
   ${OPENGL_gl_LIBRARY}
   ${OPENGL_glu_LIBRARY}
)
include_directories(${SDL_INCLUDE_DIR} ${RUBY_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/gui
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sound
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/3dengine
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/entities
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/hljobs
)

INCLUDE(${SWIG_USE_FILE})

SET(CMAKE_SWIG_FLAGS "")

#SET_SOURCE_FILES_PROPERTIES(example.i PROPERTIES CPLUSPLUS ON)
#SET_SOURCE_FILES_PROPERTIES(example.i PROPERTIES SWIG_FLAGS "-includeall")
#SWIG_ADD_MODULE(example ruby example.i example.cxx)
#SWIG_LINK_LIBRARIES(example ${PYTHON_LIBRARIES})
#set_target_properties(example PROPERTIES PREFIX "")


FILE(GLOB MYLIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} lib/*.cc lib/*.c lib/gui/*.cc lib/sound/*.cc
  lib/3dengine/*.cc
  lib/game/*.cc
  lib/game/entities/*.cc
  lib/game/hljobs/*.cc
)
FILE(GLOB MYLIB_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} lib/*.h lib/gui/*.h lib/sound/*.h
  lib/3dengine/*.h lib/game/*.h
  
)

add_executable(antargis2 main.cpp ${MYLIB_SOURCES})
