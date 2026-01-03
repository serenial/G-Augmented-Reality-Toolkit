# Specify Project
project(gartoolkit-shared-binaries VERSION 1.4 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(g_ar_toolkit)
add_subdirectory(g_ar_toolkit_ar)
add_subdirectory(g_ar_toolkit_fd)

set(BUILD_TARGETS
  g_ar_toolkit
  g_ar_toolkit_ar
  g_ar_toolkit_fd
)

if(NOT OMIT_DNN)
  add_subdirectory(g_ar_toolkit_dnn)
  list(APPEND BUILD_TARGETS g_ar_toolkit_dnn)
endif()

if(NOT OMIT_TESSERACT)
  add_subdirectory(g_ar_toolkit_tesseract)
  list(APPEND BUILD_TARGETS g_ar_toolkit_tesseract)
endif()

if(NOT OMIT_VIDEOIO)
  add_subdirectory(g_ar_toolkit_videoio)
  list(APPEND BUILD_TARGETS g_ar_toolkit_videoio)
endif()

add_subdirectory(opkg_pkg_build)

# Install Shared Library
INSTALL(TARGETS ${BUILD_TARGETS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
)

if(GENERATE_ARCHIVE)

SET(ARCHIVE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

add_custom_target(archived_binaries_prepare ALL)

# For each library target
foreach(lib_target ${BUILD_TARGETS})
    add_custom_command(TARGET archived_binaries_prepare POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_target}> ${ARCHIVE_DIRECTORY}/$<TARGET_FILE_NAME:${lib_target}>
    )
    add_dependencies(archived_binaries_prepare ${lib_target})
endforeach()

# Bundle up dependencies
add_custom_target(archived_binaries_tar ALL)
add_dependencies(archived_binaries_tar archived_binaries_prepare)

string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWERCASE)

add_custom_command(TARGET archived_binaries_tar POST_BUILD
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar "cfvz" "${PROJECT_NAME}_${PROJECT_VERSION}_${SYSTEM_NAME_LOWERCASE}_${ARCHIVE_PLATFORM_NAME}.tar.gz" "${ARCHIVE_DIRECTORY}"
    COMMENT "Building tar package..."
)

endif()