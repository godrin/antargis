
FILE(GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc)
add_executable(ant_test ${TEST_SOURCES})
target_link_libraries(ant_test antlib)
add_test (NAME MyAntTest COMMAND ant_test)
