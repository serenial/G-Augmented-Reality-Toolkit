set(VCPKG_TARGET_ARCHITECTURE arm)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# Reference the main toolchain file
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../scripts/toolchains/nilrt-arm32v7.cmake")

# Compiler flags for ARMv7-A Cortex-A9
set(VCPKG_C_FLAGS "-march=armv7-a -mfpu=vfpv3 -mfloat-abi=softfp -mcpu=cortex-a9 -O2 -pipe")
set(VCPKG_CXX_FLAGS "-march=armv7-a -mfpu=vfpv3 -mfloat-abi=softfp -mcpu=cortex-a9 -O2 -pipe")
set(VCPKG_LINKER_FLAGS "-Wl,-O1")

# Pass through PKG_CONFIG environment variables
set(VCPKG_ENV_PASSTHROUGH PKG_CONFIG_PATH PKG_CONFIG_SYSROOT_DIR)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)