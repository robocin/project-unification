cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Version defines and project name.
include(version.cmake)

# Configuring project with given name.
project(${ROBOCIN_PROJECT_NAME} VERSION ${ROBOCIN_PROJECT_VERSION} LANGUAGES CXX)

# adds executable subdir and tests subdir.
add_subdirectory(src ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_subdirectory(test ${CMAKE_CURRENT_SOURCE_DIR}/bin/test)
