
cmake_minimum_required(VERSION 2.8)
project(antargis)

find_package(SDL)
find_package(SDL_image)
find_package(SDL_ttf)
find_package(SDL_mixer)
#find_package(Ruby)
find_package(ZLIB)
find_package(OpenGL)
find_package(PNG)
find_package(Boost COMPONENTS signals)
#FIND_PACKAGE(SWIG REQUIRED)

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
   ${PNG_LIBRARY}
   ${OPENGL_gl_LIBRARY}
   ${OPENGL_glu_LIBRARY}
   ${Boost_LIBRARIES}
)
include_directories(${SDL_INCLUDE_DIR} ${RUBY_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/gui
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sound
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/3dengine
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/entities
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/hljobs
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/players
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/formations
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/path
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/game/widgets
)

#INCLUDE(${SWIG_USE_FILE})
list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")
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
  lib/game/players/*.cc
  lib/game/formations/*.cc
  lib/game/path/*.cc
  lib/game/widgets/*.cc
)
FILE(GLOB MYLIB_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} lib/*.h lib/gui/*.h lib/sound/*.h
  lib/3dengine/*.h
  lib/game/*.h
  lib/players/*.h
  lib/formations/*.h
  lib/path/*.h
  lib/game/widgets/*.h
  
)

add_executable(antargis main.cpp ${MYLIB_SOURCES})

INSTALL(TARGETS antargis DESTINATION /usr/bin)
INSTALL(DIRECTORY data DESTINATION /usr/share/antargis)

SET (CPACK_PACKAGING_INSTALL_PREFIX "/tmp")

SET (CPACK_GENERATOR "DEB")
#"TGZ" "RPM")

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

SET (CPACK_PACKAGE_CONTACT "David Kamphausen <david.kamphausen@web.de> ")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl-image1.2 (>=1.2.10-2.1), libsdl-mixer1.2 (>=1.2.8-6.3build1), 
 libsdl-ttf2.0-0 (>=2.0.9-1build2)")


SET(DEBIAN_PACKAGE_BUILDS_DEPENDS " libsdl-image1.2-dev (>=1.2.10-2.1)
 libsdl-mixer1.2-dev (>=1.2.8-6.3build1)
 libsdl-ttf2.0-dev (>=2.0.9-1build2)
 libsdl1.2-dev (>=1.2.14-6.1ubuntu4)
 libpng12-dev (>=1.2.46-3ubuntu1)
 zlib1g-dev (>=1:1.2.3.4.dfsg-3ubuntu3)
   g++ (>=4:4.6.1-2ubuntu5)")
include(CPack)
