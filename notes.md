https://web.archive.org/web/20201027083952/https://www.labviewmakerhub.com/doku.php?id=learn:tutorials:libraries:linx:misc:build-on-target

https://forums.ni.com/t5/NI-Linux-Real-Time-Discussions/Any-doc-on-hosting-my-own-OPKG-repository-How-about-how-to-make/td-p/3892635

https://www.gernot-walzl.at/index.php?nav=.%2FDebian%2FRaspbian%2Fcross-compile

https://downloads.raspberrypi.com/raspios_oldstable_lite_arm64/images/raspios_oldstable_lite_arm64-2025-11-24/2025-11-24-raspios-bookworm-arm64-lite.img.xz

https://devblogs.microsoft.com/cppblog/vcpkg-artifacts/

=========================== Parked ==============================

if(VCPKG_CROSSCOMPILING)
    message(STATUS "-- Downloading the nilrt armv7a compile tools from NI.com...")

    vcpkg_download_distfile(
        OE_CROSS_COMPILER_INSTALL_SCRIPT
        URLS "https://download.ni.com/support/softlib/labview/labview_rt/2018/Linux%20Toolchains/linux/oecore-x86_64-cortexa9-vfpv3-toolchain-6.0.sh"
        FILENAME "ecore-x86_64-cortexa9-vfpv3-toolchain-6.0.sh"
        SHA512 31833d2ddc49b18d740332d9c2e8680ab3834b0f0a8e5555511323d6d6a819f01cd43725ad6c79c1571e47a0b23fa8209c367d40bb90b755fc896d51cf41cd7d
    )

    message(STATUS "-- Extracting and installing the nilrt armv7a compile tools...")

    vcpkg_execute_required_process(
        COMMAND chmod +x "${OE_CROSS_COMPILER_INSTALL_SCRIPT}" && "${OE_CROSS_COMPILER_INSTALL_SCRIPT}" -y -d "${CURRENT_PACKAGES_DIR}/tools/${PORT}"
        LOGNAME nilrt-armv7a-setup-${TARGET_TRIPLET}
        WORKING_DIRECTORY ${CURRENT_PACKAGES_DIR}
    )

    message(STATUS "-- Compile tools installed to ${CURRENT_PACKAGES_DIR}/tools/${PORT}...")

else()
    message(STATUS "-- NOT CROSS COMPILING")
endif()
