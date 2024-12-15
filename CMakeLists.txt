cmake_minimum_required(VERSION 3.27)

# Specify Project
project(g_ar_toolkit VERSION 1.0.0 LANGUAGES C CXX)

include(GenerateExportHeader)
include(FetchContent)

# export a json file with the compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# # Install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/LabVIEW/g_ar_toolkit.common)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # set to use Multithreaded Static Runtime
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  # # vcpkg has some issues with not loading the static libs
  # so manually fix the MSVC runtime components used
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

set(OpenCV_SHARED OFF)

# https://github.com/opencv/opencv/wiki/Compact-build-advice
set(ENABLE_LTO ON)

# # Find OpenCV
find_package(OpenCV 4.10.0 REQUIRED)

# # Find tinyobjloader
find_package(tinyobjloader CONFIG REQUIRED)


# # These dependencies managed outside of vcpkg are accesses from their repos but a snapshot of them is included in
# #  third-party
if(UNIX)
  FetchContent_Declare(v4l2listdevices
    GIT_REPOSITORY https://github.com/improvess/v4l2-list-devices
    GIT_TAG 9e4b0bb6c4e52ee174fe0055c1ba654de9c0ace7 # main latest commit 2022/06/09
    PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/v4l2_list_devices.patch
    UPDATE_DISCONNECTED 1
  )
  FetchContent_MakeAvailable(v4l2listdevices)
endif()

FetchContent_Declare(apriltag
  GIT_REPOSITORY https://github.com/AprilRobotics/apriltag.git
  GIT_TAG v3.4.2 # main latest commit 2024/05/13
  PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/apriltag.patch
  UPDATE_DISCONNECTED 1
)
FetchContent_MakeAvailable(apriltag)

FetchContent_Declare(
  whereami
  GIT_REPOSITORY https://github.com/gpakosz/whereami.git
  GIT_TAG f5e3eac441acbb4ec1fe3e2c32646248ae463398 # latest commit as of Jun 9, 2024
  PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/whereami.patch # ensure patch UTF8 encoded!
  UPDATE_DISCONNECTED ON # prevents trying to patch multiple times
)
FetchContent_MakeAvailable(whereami)

# specify shared-library target
add_library(${PROJECT_NAME} SHARED)

# add compile definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC G_AR_TOOLKIT_BYTE_PACKING_${CMAKE_SIZEOF_VOID_P})

# define include root dir
set(PUBLIC_HEADER_DIR include/${PROJECT_NAME})

# define platform specific headers and source
# Platform name should match the value in
SET(PLATFORM_HEADER_FILES_Windows
  ${PUBLIC_HEADER_DIR}/capture/win/utils.hpp
  ${PUBLIC_HEADER_DIR}/capture/win/source_reader.hpp
)

SET(PLATFORM_SOURCE_FILES_Windows
  src/capture/win/enumerator.cpp
  src/capture/win/stream.cpp
  src/capture/win/utils.cpp
  src/capture/win/source_reader.cpp
)

SET(PLATFORM_HEADER_FILES_Linux
  ${PUBLIC_HEADER_DIR}/capture/linux/decoders.hpp
  ${PUBLIC_HEADER_DIR}/capture/linux/utils.hpp
)

SET(PLATFORM_SOURCE_FILES_Linux
  src/capture/linux/enumerator.cpp
  src/capture/linux/stream.cpp
  src/capture/linux/decoders.cpp
  src/capture/linux/utils.cpp
)

# Add Project sources
target_sources(${PROJECT_NAME} PRIVATE
  src/lv_interop/lv_functions.cpp
  src/lv_interop/lv_error.cpp
  src/lv_interop/lv_image.cpp
  src/lv_interop/lv_u32_colour.cpp
  src/lv_interop/lv_str.cpp
  src/lv_interop/lv_enums.cpp
  src/lv_interop/lv_termination_critera.cpp
  src/lv_interop/lv_point_correspondence.cpp
  src/lv_interop/lv_camera_parameters.cpp
  src/lv_interop/lv_vec_types.cpp
  src/lv_interop/lv_mat.cpp
  src/capture/stream.cpp
  src/image_create.cpp
  src/image_blur.cpp
  src/image_window.cpp
  src/image_resize.cpp
  src/image_draw_picture.cpp
  src/image_draw.cpp
  src/image_in_range.cpp
  src/image_bitwise.cpp
  src/image_read_file.cpp
  src/image_write_file.cpp
  src/image_add_weighted.cpp
  src/image_flip.cpp
  src/image_crop.cpp
  src/image_greyscale_to_argb.cpp
  src/image_insert.cpp
  src/image_argb_channel_operations.cpp
  src/image_canny.cpp
  src/image_copy.cpp
  src/image_copy_raw_memory_ptr.cpp
  src/image_copy_pixmap.cpp
  src/image_copy_array.cpp
  src/capture_stream.cpp
  src/image_negate.cpp
  src/image_transform.cpp
  src/image_rotate.cpp
  src/image_threshold.cpp
  src/feature_detection_chessboard.cpp
  src/feature_detection_apriltag.cpp
  src/feature_detection_contours.cpp
  src/feature_detection_hough.cpp
  src/feature_detection_barcode.cpp
  src/feature_detection_we_chat_qr.cpp
  src/ar_camera_calibration.cpp
  src/ar_project_points.cpp
  src/ar_obj_loader.cpp
  src/ar_solve_pnp.cpp
  src/ar_rodrigues.cpp
  src/shape_utils.cpp
  "${PLATFORM_SOURCE_FILES_${CMAKE_HOST_SYSTEM_NAME}}"
)

# Add Project Headers for better IDE usability
# target_include_dirs still required
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS
  FILES
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_types.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_image.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_point_correspondence.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_termination_criteria.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/set_packing.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_vec_types.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_u32_colour.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_edvr_managed_object.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_str.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_enums.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_camera_parameters.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_picture.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_array_1d.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_functions.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_mat.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_array_2d.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_error.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/reset_packing.hpp
  ${PUBLIC_HEADER_DIR}/capture/stream.hpp
  ${PUBLIC_HEADER_DIR}/capture/enumerator.hpp
  "${PLATFORM_HEADER_FILES_${CMAKE_HOST_SYSTEM_NAME}}"
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"

  # add binary directory so auto-generated g_ar_toolkit_export.h is found
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

# plaform specific definitions
if(WIN32)
  # windows.h definitions
  target_compile_definitions(${PROJECT_NAME} PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX /wd4068)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    mf
    mfplat
    Mfreadwrite
    mfuuid
    Strmiids
    Rpcrt4
    runtimeobject
    Mfsensorgroup
    shlwapi
  )
elseif(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
  target_link_libraries(${PROJECT_NAME} PRIVATE
    v4l2listdevices
  )
  target_compile_options(${PROJECT_NAME} PRIVATE
    "-fdata-sections" "-ffunction-sections"
  )
  target_link_options(${PROJECT_NAME} PRIVATE
    "--gc-sections"
  )
endif()

# specify target properties
# winrt only supports C++17 std
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_target_properties(${target_name} PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

# generate export header
generate_export_header(${PROJECT_NAME})

# Set the Library Filename to include the bitness
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}.${CMAKE_PROJECT_VERSION_MAJOR}.${BITNESS})
target_link_libraries(${PROJECT_NAME} PRIVATE
  opencv_core
  opencv_imgproc
  opencv_highgui
  opencv_calib3d
  opencv_objdetect
  opencv_wechat_qrcode
  tinyobjloader::tinyobjloader
  apriltag
  whereami
)

# Install Shared Library
INSTALL(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
)