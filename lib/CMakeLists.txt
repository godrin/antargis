
#include_directories("${PROJECT_SOURCE_DIR}/lib")

#add_library(mylib STATIC ag_fs.cc rk_string.cc rk_debug.cc ag_config          \
                 .cc ag_collector.cc
#ag_fs.cc ag_main.cc ag_rand_base.cc ag_serial.cc ag_singleton                 \
    .cc ag_stringstream.cc
#ag_video_base.cc ag_xml.cc ag_profiler.cc
#)