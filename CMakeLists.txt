cmake_minimum_required(VERSION 3.27)

# Specify Project
project(g_ar_toolkit VERSION 1.0.0 LANGUAGES C CXX)

include(GenerateExportHeader)
include(FetchContent)

# # Install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/LabVIEW)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # set to use Multithreaded Static Runtime
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  ## vcpkg has some issues with not loading the static libs
  # so manually fix the MSVC runtime components used
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
  set(OpenCV_SHARED OFF)
endif()

## Find OpenCV
find_package(OpenCV 4.8.0 REQUIRED)
## Find tinyobjloader
find_package(tinyobjloader CONFIG REQUIRED)

if(WIN32)
  FetchContent_Declare(rosmsftcam
    GIT_REPOSITORY https://github.com/ms-iot/ros_msft_camera.git
    GIT_TAG fe2b4190a1cc8bb837286244a417e083493c198c # foxy-devel latest commit 2021/08/09
    PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/ros_msft_cam.patch # remember to save the patch with UTF8 encoding 
    UPDATE_DISCONNECTED 1
  )
  FetchContent_MakeAvailable(rosmsftcam)
  
elseif(UNIX)

  FetchContent_Declare(v4l2listdevices
    GIT_REPOSITORY https://github.com/improvess/v4l2-list-devices
    GIT_TAG 9e4b0bb6c4e52ee174fe0055c1ba654de9c0ace7 # main latest commit 2022/06/09
    PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/v4l2_list_devices.patch
    UPDATE_DISCONNECTED 1
  )
  FetchContent_MakeAvailable(v4l2listdevices)

endif()

# specify shared-library target
add_library(${PROJECT_NAME} SHARED)

# add compile definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC G_AR_TOOLKIT_BYTE_PACKING_${CMAKE_SIZEOF_VOID_P})

# define include root dir
set(PUBLIC_HEADER_DIR include/${PROJECT_NAME})

# define platform specific headers and source
# Platform name should match the value in
SET(PLATFORM_HEADER_FILES_Windows
  ${PUBLIC_HEADER_DIR}/win/context_wmf.hpp
  ${PUBLIC_HEADER_DIR}/win/stream_wmf.hpp
  ${rosmsftcam_SOURCE_DIR}/include/win_camera/wincapture.h
)

SET(PLATFORM_SOURCE_FILES_Windows
  src/win/context_wmf.cpp
  src/win/stream_wmf.cpp
  ${rosmsftcam_SOURCE_DIR}/src/wincapture.cpp
)

SET(PLATFORM_HEADER_FILES_Linux
  ${PUBLIC_HEADER_DIR}/linux/context_v4l2.hpp
  ${PUBLIC_HEADER_DIR}/linux/stream_v4l2.hpp
  ${PUBLIC_HEADER_DIR}/linux/decoders_v4l2.hpp
  ${PUBLIC_HEADER_DIR}/linux/utils_v4l2.hpp
  ${v4l2listdevices_SOURCE_DIR}/include/v4l2_list_devices/list_devices.hpp
)

SET(PLATFORM_SOURCE_FILES_Linux
  src/linux/context_v4l2.cpp
  src/linux/stream_v4l2.cpp
  src/linux/decoders_v4l2.cpp
  src/linux/utils_v4l2.cpp
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
  src/capture/context.cpp
  src/image_create.cpp
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
  src/image_copy.cpp
  src/image_copy_raw_memory_ptr.cpp
  src/image_copy_pixmap.cpp
  src/image_copy_array.cpp
  src/capture_context.cpp
  src/capture_stream.cpp
  src/image_negate.cpp
  src/image_transform.cpp
  src/image_rotate.cpp
  src/feature_detection_chessboard.cpp
  src/ar_camera_calibration.cpp
  src/ar_project_points.cpp
  src/ar_obj_loader.cpp
  src/ar_solve_pnp.cpp
  "${PLATFORM_SOURCE_FILES_${CMAKE_HOST_SYSTEM_NAME}}"
)

# Add Project Headers for better IDE usability
# target_include_dirs still required
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS
  FILES
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_functions.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_types.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_error.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_picture.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_u32_colour.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_str.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_array_1d.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_array_2d.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_termination_criteria.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/reset_packing.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/set_packing.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_edvr_managed_object.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_fixed_size_matrix.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_enums.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_fixed_size_matrix.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_image.hpp
  ${PUBLIC_HEADER_DIR}/capture/context.hpp
  ${PUBLIC_HEADER_DIR}/capture/stream.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_point_correspondence.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_camera_parameters.hpp
  ${PUBLIC_HEADER_DIR}/lv_interop/lv_vec_types.hpp
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
  target_compile_definitions(${PROJECT_NAME} PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)
  target_include_directories(${PROJECT_NAME} PUBLIC ${rosmsftcam_SOURCE_DIR}/include)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    mf
    mfplat
    Mfreadwrite
    mfuuid
    Rpcrt4
    runtimeobject
    Mfsensorgroup
    shlwapi
  )
elseif(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

  # set linking flags required for static ffmpeg without --fPIC
  # set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")
  target_include_directories(${PROJECT_NAME} PUBLIC
    ${v4l2listdevices_SOURCE_DIR}/include
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
  tinyobjloader::tinyobjloader
)

# Install Shared Library
INSTALL(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
)