# include tooling to generate export header
include(GenerateExportHeader)

# Find tinyobjloader
find_package(tinyobjloader REQUIRED)

# Find OpenCV
find_package(OpenCV 4.11 REQUIRED core calib3d)

# specify shared-library target
add_library(g_ar_toolkit_ar SHARED)

generate_export_header(g_ar_toolkit_ar)

# plaform specific definitions
if(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(g_ar_toolkit_ar PROPERTIES PREFIX "")
endif()

target_include_directories(g_ar_toolkit_ar
  PUBLIC
    # add binary directory so auto-generated g_ar_toolkit_ar_export.h is found
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)


# Set the Library Filename to include the bitness
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(g_ar_toolkit_ar PROPERTIES OUTPUT_NAME gartoolkit_ar_${BITNESS}-${g_ar_toolkit_shared_binary_VERSION_MAJOR})
target_link_libraries(g_ar_toolkit_ar PRIVATE
  g_ar_toolkit
  tinyobjloader::tinyobjloader
  opencv_calib3d
)

target_sources(g_ar_toolkit_ar PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_camera_calibration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_project_points.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_obj_loader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_solve_pnp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_rodrigues.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_quaternion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_chessboard.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_image_distortion.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_image_undistort.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ar_image_undistortion.cpp
)