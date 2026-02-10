if (VCPKG_TARGET_IS_WINDOWS)
    vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO AprilRobotics/apriltag
    REF 122ab9b4a1ee37e8bebfab59fa5808fe317b7b93 # master commit on Feb 5, 2026
    SHA512 a4503d74597225813bd0ed2319a1b108bbdec774f003454aaafa297c379b33071ace7c5bd4b3e987006b0a2e6da99d63f48ea3d88e0d29f7e7138bbee14c0aeb
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
