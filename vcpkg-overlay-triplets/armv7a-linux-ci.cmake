set(VCPKG_TARGET_ARCHITECTURE armv7-a)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_BUILD_TYPE release)

# values from /usr/local/oecore-x86_64/environment-setup-cortexa9-vfpv3-nilrt-linux-gnueabi
set(VCPKG_C_FLAGS "-march=armv7-a -mfpu=vfpv3  -mfloat-abi=softfp -mcpu=cortex-a9")
set(VCPKG_CXX_FLAGS "-march=armv7-a -mfpu=vfpv3  -mfloat-abi=softfp -mcpu=cortex-a9")

# Reference the main toolchain file
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../scripts/toolchains/nilrt-armv7a.cmake")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)