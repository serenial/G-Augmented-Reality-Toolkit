cmake_minimum_required(VERSION 3.27)

# Use static build on windows to generate one big binary
# with all dependent .lib files bundled
# this VCPKG flag needs to be set before the "project" directive
# so every dependency is built for a static binary target
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
  if(${CMAKE_GENERATOR} MATCHES "Visual Studio [0-9]+ [0-9]+")
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "win32")
      set(VCPKG_TARGET_TRIPLET x86-windows-static)
    else()
      set(VCPKG_TARGET_TRIPLET x64-windows-static)
    endif()
  elseif(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
    set(VCPKG_TARGET_TRIPLET x64-linux)
  endif()
endif()

# Specify Project
project(g_ar_toolkit VERSION 1.0.0 LANGUAGES C CXX)

include(GenerateExportHeader)
include(FetchContent)

# # Install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/LabVIEW)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # set to use Multithreaded Static Runtime
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  # # vcpkg has some issues with not loading the static libs
  # so manually fix the MSVC runtime components used
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
  set(OpenCV_SHARED OFF)
endif()

# # Find OpenCV
find_package(OpenCV 4.8.0 REQUIRED)

# include_directories(${hiberlite_SOURCE_DIR}/include)
if(WIN32)
  set(rosmsftcam_patch git apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/ros_msft_cam.patch)
  FetchContent_Declare(rosmsftcam
    GIT_REPOSITORY https://github.com/ms-iot/ros_msft_camera.git
    GIT_TAG fe2b4190a1cc8bb837286244a417e083493c198c # foxy-devel latest commit 2021/08/09
    PATCH_COMMAND ${rosmsftcam_patch}
    UPDATE_DISCONNECTED 1
  )
  FetchContent_MakeAvailable(rosmsftcam)
elseif(UNIX)
  find_package(FFMPEG REQUIRED)
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
  ${PUBLIC_HEADER_DIR}/win/context-wmf.hpp
  ${PUBLIC_HEADER_DIR}/win/context-wmf-formats.hpp
  ${PUBLIC_HEADER_DIR}/win/stream-wmf.hpp
  ${rosmsftcam_SOURCE_DIR}/include/win_camera/wincapture.h
)

SET(PLATFORM_SOURCE_FILES_Windows
  src/win/context-wmf.cpp
  src/win/stream-wmf.cpp
  ${rosmsftcam_SOURCE_DIR}/src/wincapture.cpp
)

SET(PLATFORM_HEADER_FILES_Linux
  ${PUBLIC_HEADER_DIR}/linux/context-v4l2.hpp
  ${PUBLIC_HEADER_DIR}/linux/context-v4l2-formats.hpp
  ${PUBLIC_HEADER_DIR}/linux/stream-v4l2.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/constants.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/conversions.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/usb_cam.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/utils.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/av_pixel_format_helper.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/m420.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/mjpeg.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/mono.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/pixel_format_base.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/rgb.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/utils.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/uyvy.hpp
  ${3RD_PARTY_ROS_USB_CAM}/include/usb_cam/formats/yuyv.hpp
  ${3RD_PARTY_DIR}/v4l2-list-devices/include/list_devices.hpp
)

SET(PLATFORM_SOURCE_FILES_Linux
  src/linux/context-v4l2.cpp
  src/linux/stream-v4l2.cpp
  ${3RD_PARTY_ROS_USB_CAM}/src/usb_cam.cpp
  ${3RD_PARTY_ROS_USB_CAM}/src/conversions.cpp
)

# Add Project sources
target_sources(${PROJECT_NAME} PRIVATE
  src/lv-interop/lv-functions.cpp
  src/lv-interop/lv-error.cpp
  src/lv-interop/lv-image.cpp
  src/lv-interop/lv-u32-colour.cpp
  src/lv-interop/lv-str.cpp
  src/lv-interop/lv-enums.cpp
  src/capture/context.cpp
  src/image-create.cpp
  src/image-window.cpp
  src/image-resize.cpp
  src/image-draw-picture.cpp
  src/image-draw.cpp
  src/image-in-range.cpp
  src/image-bitwise.cpp
  src/image-read-file.cpp
  src/image-write-file.cpp
  src/image-add-weighted.cpp
  src/image-flip.cpp
  src/image-crop.cpp
  src/image-greyscale-to-argb.cpp
  src/image-insert.cpp
  src/image-argb-channel-operations.cpp
  src/image-copy.cpp
  src/image-copy-raw-memory-ptr.cpp
  src/image-copy-pixmap.cpp
  src/image-copy-array.cpp
  src/capture-context.cpp
  src/capture-stream.cpp
  src/image-negate.cpp
  src/image-transform.cpp
  src/image-rotate.cpp
  "${PLATFORM_SOURCE_FILES_${CMAKE_HOST_SYSTEM_NAME}}"
)

# Add Project Headers for better IDE usability
# target_include_dirs still required
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS
  FILES
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-functions.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-types.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-error.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-picture.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-u32-colour.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-str.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-array.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/reset-packing.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/set-packing.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-edvr-managed-object.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-fixed-size-matrix.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-enums.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-fixed-size-matrix.hpp
  ${PUBLIC_HEADER_DIR}/lv-interop/lv-image.hpp
  ${PUBLIC_HEADER_DIR}/capture/context.hpp
  ${PUBLIC_HEADER_DIR}/capture/stream.hpp
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
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")
  target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${3RD_PARTY_ROS_USB_CAM}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${3RD_PARTY_DIR}/v4l2-list-devices/include>"
    ${FFMPEG_INCLUDE_DIRS}
  )
  target_link_directories(${PROJECT_NAME} PRIVATE
    ${FFMPEG_LIBRARY_DIRS}
  )
  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${FFMPEG_LIBRARIES}
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
)

# Install Shared Library
INSTALL(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
)