if (VCPKG_TARGET_IS_WINDOWS)
    vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO AprilRobotics/apriltag
    REF c5a4fc1cc5c6edcb5929dfc3a79f167bf173f7a5 # master commit on jan 20, 2026
    SHA512 0cc8e50a2d7db1dab1e5a75ee0bf4484e40cb65532899c80bd0980cb08785f006205def14ec642fd735ec9dbbf780cba431ef7f4497384f0020337614bcbee32
    HEAD_REF master
    PATCHES
        custom-001-fix-compile-errors-on-vardiac-macro.patch
        custom-002-windows-perf.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_PYTHON_WRAPPER=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_TESTING=OFF
)

vcpkg_cmake_install()

vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_cmake_config_fixup(CONFIG_PATH lib/${PORT}/cmake)
vcpkg_fixup_pkgconfig()

if (VCPKG_TARGET_IS_WINDOWS)
    file(READ "${CURRENT_PACKAGES_DIR}/share/${PORT}/${PORT}Config.cmake" FIXED_CONFIG)
    file(WRITE "${CURRENT_PACKAGES_DIR}/share/${PORT}/${PORT}Config.cmake" "
    include(CMakeFindDependencyMacro)
    find_dependency(PThreads4W)
    ${FIXED_CONFIG}
    ")
endif()

file(INSTALL "${SOURCE_PATH}/LICENSE.md" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/lib/apriltag" "${CURRENT_PACKAGES_DIR}/lib/apriltag")
