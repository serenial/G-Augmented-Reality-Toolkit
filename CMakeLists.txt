cmake_minimum_required(VERSION 3.27)

# SET the toolchain file to the one in the VCPKG git-submodule
SET(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(g_ar_toolkit)

# add source
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/shared-binary)