if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # set to use Multithreaded Static Runtime
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  # # vcpkg has some issues with not loading the static libs
  # # so manually fix the MSVC runtime components used
  # string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  # string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

set(CMAKE_CXX_STANDARD 17)

# Specify Project
project(g_ar_toolkit_shared_binary VERSION 1.0.0 LANGUAGES C CXX)

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