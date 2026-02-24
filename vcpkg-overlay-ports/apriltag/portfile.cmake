if (VCPKG_TARGET_IS_WINDOWS)
    vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO AprilRobotics/apriltag
    REF 8c52e4c6f2c85cf753367bc45212e652ece5ffc2 # master commit on Feb 24, 2026
    SHA512 373a4b0d140af91c4aadd06a735346f93a013cf3105c3b77c56d400f946568fc7d3e8fcd919065223c6e70d16fbb3168e921fc7c9cf404bfc6fba481682e5f93
    HEAD_REF master
    PATCHES
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
