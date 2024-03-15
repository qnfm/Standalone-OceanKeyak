cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(OceanKeyak C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(sodium_USE_STATIC_LIBS ON)
find_package(Sodium REQUIRED)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(src)
add_subdirectory(tests)
