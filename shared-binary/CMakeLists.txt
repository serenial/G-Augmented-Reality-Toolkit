set(CMAKE_CXX_STANDARD 17)

# Specify Project
project(g_ar_toolkit_shared_binary VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(g_ar_toolkit)
add_subdirectory(g_ar_toolkit_ar)
add_subdirectory(g_ar_toolkit_dnn)
add_subdirectory(g_ar_toolkit_fd)
add_subdirectory(g_ar_toolkit_tesseract)
add_subdirectory(g_ar_toolkit_videoio)

# Install Shared Library
INSTALL(TARGETS g_ar_toolkit g_ar_toolkit_ar g_ar_toolkit_dnn g_ar_toolkit_fd g_ar_toolkit_tesseract g_ar_toolkit_videoio
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
)