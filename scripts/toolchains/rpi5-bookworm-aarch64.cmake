if(NOT _RPI_BOOKWORM_AARCH64)
    set(_RPI_BOOKWORM_AARCH64 TRUE)

    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_CROSSCOMPILING ON)

    set(TOOLCHAIN_NAME aarch64-linux-gnu)
    set(TOOL_BASE_PATH "/usr/bin/${TOOLCHAIN_NAME}-")

    # Specify the cross compiler
    set(CMAKE_C_COMPILER "${TOOL_BASE_PATH}gcc")
    set(CMAKE_CXX_COMPILER "${TOOL_BASE_PATH}g++")

    # Specify binutils
    set(CMAKE_AR "${TOOL_BASE_PATH}ar")
    set(CMAKE_RANLIB "${TOOL_BASE_PATH}ranlib")
    set(CMAKE_STRIP "${TOOL_BASE_PATH}strip")
    set(CMAKE_NM "${TOOL_BASE_PATH}nm")
    set(CMAKE_OBJCOPY "${TOOL_BASE_PATH}objcopy")
    set(CMAKE_OBJDUMP "${TOOL_BASE_PATH}objdump")

    # Specify the sysroot (optional, adjust path as needed)
    set(CMAKE_SYSROOT ~/downloads/hiya)

    # Additional compiler flags optimized for Raspberry Pi 5 (Cortex-A76)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a76 -mtune=cortex-a76 -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a76 -mtune=cortex-a76 -fPIC")

    set(CMAKE_MODULE_LINKER_FLAGS_INIT "-Wl,-O1")
    set(CMAKE_SHARED_LINKER_FLAGS_INIT "-Wl,-O1")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-O1")
endif()