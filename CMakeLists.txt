cmake_minimum_required(VERSION 3.0)

project("M1S2_ProgSyst")

add_compile_options(-g -ffast-math -pthread -std=gnu99 -Wall -fdiagnostics-show-option)

#include -threads
find_package(Threads REQUIRED)

# include gtk+-3.0
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})



set(CONDUCT_HEADERS
  "src/conduct/conduct.h"
  "src/utils/bench.h"
)

set(CONDUCT_SOURCES
  "src/conduct/conduct.c"
  "src/utils/bench.c"
  ${CONDUCT_HEADERS}
)

# -----------------------------------------------------------------------------
# Test files
# -----------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/test/)

file(GLOB tests "src/tests/*.c")
foreach(file ${tests})
  get_filename_component(FILENAME ${file} NAME)
  string(REGEX MATCH "^[a-zA-Z0-9_-]*" FILENAME ${FILENAME})
  set(SOURCES
    ${file}
    ${CONDUCT_SOURCES}
  )
  add_executable(${FILENAME} ${SOURCES})
  target_link_libraries(${FILENAME} ${CMAKE_THREAD_LIBS_INIT} ${GTK3_LIBRARIES})
endforeach()

# -----------------------------------------------------------------------------
# Bench files
# -----------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/bench/)
file(GLOB benchs "src/bench/*.c")
foreach(file ${benchs})
  get_filename_component(FILENAME ${file} NAME)
  string(REGEX MATCH "^[a-zA-Z0-9_-]*" FILENAME ${FILENAME})
  set(SOURCES
    ${file}
    ${CONDUCT_SOURCES}
  )
  add_executable(${FILENAME} ${SOURCES})
  target_link_libraries(${FILENAME} ${CMAKE_THREAD_LIBS_INIT} ${GTK3_LIBRARIES})
endforeach()
