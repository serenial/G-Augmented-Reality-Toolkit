# specify shared-library target
add_library(g_ar_toolkit_ar SHARED)

# Find tinyobjloader
find_package(tinyobjloader REQUIRED)

# plaform specific definitions
if(WIN32)

elseif(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(g_ar_toolkit_ar PROPERTIES PREFIX "")
endif()

# specify target properties
# winrt only supports C++17 std
set_property(TARGET g_ar_toolkit_ar PROPERTY CXX_STANDARD 17)
set_target_properties(g_ar_toolkit_ar PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET default
  CXX_VISIBILITY_PRESET default
  VISIBILITY_INLINES_HIDDEN ON
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)

set_target_properties(g_ar_toolkit_ar PROPERTIES OUTPUT_NAME gartoolkit_ar_${BITNESS}-${g_ar_toolkit_shared_binary_VERSION_MAJOR})
target_link_libraries(g_ar_toolkit_ar PRIVATE
  g_ar_toolkit
  tinyobjloader::tinyobjloader
)

target_sources(g_ar_toolkit_ar PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_camera_calibration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_project_points.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_obj_loader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_solve_pnp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_rodrigues.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_quaternion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_detect_chessboard.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_distortion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_undistort.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_undistortion.cpp
)