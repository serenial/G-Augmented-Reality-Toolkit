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