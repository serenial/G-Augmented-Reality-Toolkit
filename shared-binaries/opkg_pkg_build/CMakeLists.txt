
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND BUILD_OPKG)

    include(ExternalProject)

    SET(OPKG_UTILS_PATH ${CMAKE_CURRENT_BINARY_DIR}/opkg-utils)

    ExternalProject_Add(
        opkg_utils
        URL https://git.yoctoproject.org/opkg-utils/snapshot/opkg-utils-0.7.0.tar.gz
        URL_HASH SHA512=441ee5ed416c3565617ae5fc413846ebc53e33876f9cce5e721afef2b8d9cd68723231ea12c8a2effaba2ccb33a36dc6e180994a57e6871f34d671c03ab36b7b
        CONFIGURE_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_COMMAND "make"
        BUILD_IN_SOURCE true
        SOURCE_DIR "${OPKG_UTILS_PATH}"
    )

    # Add some targets to handle opkg-generation
    set(OPKG_BINARY_DIR "${CMAKE_BINARY_DIR}/opkg")
    set(OPKG_PACKAGE_INPUTS_DIR "${OPKG_BINARY_DIR}/${PROJECT_NAME}")

    # OPKG Meta Data Generation
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/control.in"
        "${OPKG_PACKAGE_INPUTS_DIR}/CONTROL/control"
        @ONLY
        NEWLINE_STYLE UNIX
    )

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/debian-binary.in"
        "${OPKG_PACKAGE_INPUTS_DIR}/debian-binary"
        NEWLINE_STYLE UNIX
    )

    add_custom_target(opkg_prepare ALL)
    add_dependencies(opkg_prepare opkg_utils)

    # For each library target
    foreach(lib_target ${BUILD_TARGETS})
        add_custom_command(TARGET opkg_prepare POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_target}> ${OPKG_PACKAGE_INPUTS_DIR}/usr/lib/$<TARGET_FILE_NAME:${lib_target}>
        )
        add_dependencies(opkg_prepare ${lib_target})
    endforeach()

    add_custom_target(opkg_build ALL)
    add_dependencies(opkg_build opkg_prepare)

    add_custom_command(TARGET opkg_build POST_BUILD
        WORKING_DIRECTORY "${OPKG_BINARY_DIR}"
        COMMAND fakeroot "${OPKG_UTILS_PATH}/opkg_build" "${PROJECT_NAME}"
        COMMENT "Building opkg package..."
    )
endif()
