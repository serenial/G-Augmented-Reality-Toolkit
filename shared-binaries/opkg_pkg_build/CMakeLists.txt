
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND BUILD_OPKG)

    include(ExternalProject)

    SET(OPKG_UTILS_PATH ${CMAKE_CURRENT_BINARY_DIR}/opkg-utils)

    ExternalProject_Add(
        opkg_utils
        GIT_REPOSITORY https://git.yoctoproject.org/opkg-utils
        GIT_TAG 0.7.0
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
        # call opkg-build on build outputs
        COMMAND fakeroot "${OPKG_UTILS_PATH}/opkg-build" "${PROJECT_NAME}"
        COMMENT "Building opkg package..."
    )
endif()
