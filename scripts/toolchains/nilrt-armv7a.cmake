# based on vcpkg/scripts/toolchains/linux.cmake
# values from /usr/local/oecore-x86_64/environment-setup-cortexa9-vfpv3-nilrt-linux-gnueabi

set(OE_INSTALL_SCRIPT oecore-x86_64-cortexa9-vfpv3-toolchain-6.0.sh)
set(OE_INSTALL_SCRIPT_HASH SHA256=c6e7c560a662b31f45b4c323af9b81ab4bf301f1ccd02066dbe1550e940fcbaa)

if(NOT _NILRT_ARM3V7_A_TOOLCHAIN)
    set(_NILRT_ARMV7_A_TOOLCHAIN 1)

    set(CMAKE_SYSTEM_NAME Linux CACHE STRING "")
    set(CMAKE_SYSTEM_PROCESSOR armv7a)

    set(CMAKE_CROSSCOMPILING ON)

    include(FetchContent)

    FetchContent_GetProperties(oe_armv7a_compile_tools)

    if(NOT oe_armv7a_compile_tools_POPULATED)
        message(NOTICE "-->> Downloading and installing cross compile tools for armv7 targets from NI.com...")

        set(OE_COMPILE_TOOLS_DIR "${CMAKE_BINARY_DIR}/oe_armv7a_compile_tools")

        FetchContent_Populate(
            oe_armv7a_compile_tools
            URL "https://download.ni.com/support/softlib/labview/labview_rt/2018/Linux%20Toolchains/linux/${OE_INSTALL_SCRIPT}"
            URL_HASH "${OE_INSTALL_SCRIPT_HASH}"
            DOWNLOAD_DIR "${OE_COMPILE_TOOLS_DIR}"
            DOWNLOAD_NO_EXTRACT TRUE
            UPDATE_COMMAND chmod +x "${OE_COMPILE_TOOLS_DIR}/${OE_INSTALL_SCRIPT}" && "${OE_COMPILE_TOOLS_DIR}/${OE_INSTALL_SCRIPT}" -y -d "${OE_COMPILE_TOOLS_DIR}"
            PATCH_COMMAND echo "-->> Cross compile tools for armv7 targets installed!"
        )
    endif()

    # Sysroot configuration
    set(SDKTARGETSYSROOT "${OE_COMPILE_TOOLS_DIR}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi")
    set(OECORE_NATIVE_SYSROOT "${OE_COMPILE_TOOLS_DIR}/sysroots/x86_64-nilrtsdk-linux")

    set(CMAKE_SYSROOT ${SDKTARGETSYSROOT})
    set(CMAKE_FIND_ROOT_PATH ${SDKTARGETSYSROOT})

    # Cross-compiler paths
    set(CROSS_COMPILE_PREFIX "arm-nilrt-linux-gnueabi-")
    set(TOOLCHAIN_BIN_DIR "${OECORE_NATIVE_SYSROOT}/usr/bin/arm-nilrt-linux-gnueabi")

    set(CMAKE_C_COMPILER "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}gcc")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}g++")
    set(CMAKE_ASM_COMPILER "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}as")

    # Binutils
    set(CMAKE_AR "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}ar" CACHE FILEPATH "Archiver")
    set(CMAKE_RANLIB "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}ranlib" CACHE FILEPATH "Ranlib")
    set(CMAKE_STRIP "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}strip" CACHE FILEPATH "Strip")
    set(CMAKE_NM "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}nm" CACHE FILEPATH "NM")
    set(CMAKE_OBJCOPY "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}objcopy" CACHE FILEPATH "Objcopy")
    set(CMAKE_OBJDUMP "${TOOLCHAIN_BIN_DIR}/${CROSS_COMPILE_PREFIX}objdump" CACHE FILEPATH "Objdump")

    # Compiler and Linker Flags
    set(CMAKE_C_FLAGS_INIT "-march=armv7-a -mfpu=vfpv3  -mfloat-abi=softfp -mcpu=cortex-a9 -fPIC")
    set(CMAKE_CXX_FLAGS_INIT "-march=armv7-a -mfpu=vfpv3  -mfloat-abi=softfp -mcpu=cortex-a9 -fPIC")

    set(CMAKE_MODULE_LINKER_FLAGS_INIT "-Wl,-O1")
    set(CMAKE_SHARED_LINKER_FLAGS_INIT "-Wl,-O1")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-O1")
endif()