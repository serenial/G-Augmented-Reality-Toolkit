set(OpenCV_SHARED OFF)

# include tooling to generate export header
include(GenerateExportHeader)

# Find OpenCV
find_package(OpenCV 4.11 REQUIRED core imgproc imgcodecs)

# Find eigen3
find_package(Eigen3 CONFIG REQUIRED)

# specify an object library to provide exported core functionality
add_library(g_ar_toolkit_core OBJECT)

# add compile definitions
target_compile_definitions(g_ar_toolkit_core PUBLIC G_AR_TOOLKIT_BYTE_PACKING_${CMAKE_SIZEOF_VOID_P})

set_target_properties(g_ar_toolkit_core PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET default
  CXX_VISIBILITY_PRESET default
  VISIBILITY_INLINES_HIDDEN OFF
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_include_directories(g_ar_toolkit_core
  PUBLIC
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/g_ar_toolkit>"
    # add binary directory so auto-generated g_ar_toolkit_export.h is found
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

# plaform specific definitions
if(WIN32)
  # windows.h definitions and allow mismatch co-routine due to zxing (https://devblogs.microsoft.com/oldnewthing/20230111-00/?p=107694)
  target_compile_definitions(g_ar_toolkit_core PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX /wd4068 _ALLOW_COROUTINE_ABI_MISMATCH)
endif()

target_link_libraries(g_ar_toolkit_core PUBLIC
  opencv_core
  opencv_imgproc
  Eigen3::Eigen
)

target_sources(g_ar_toolkit_core PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
  FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_types.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_point_correspondence.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_termination_criteria.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/set_packing.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_vec_types.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_u32_colour.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_str.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_enums.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_camera_parameters.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_picture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_array_1d.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_functions.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_mat.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_array_md.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_array_2d.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/lv_error.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/lv_interop/reset_packing.hpp
)

target_sources(g_ar_toolkit_core PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_functions.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_error.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_image.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_u32_colour.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_str.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_enums.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_termination_critera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_point_correspondence.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_camera_parameters.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_vec_types.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lv_interop/lv_mat.cpp
)

# specify shared-library target
add_library(g_ar_toolkit SHARED)

# add compile definitions
target_compile_definitions(g_ar_toolkit PUBLIC G_AR_TOOLKIT_BYTE_PACKING_${CMAKE_SIZEOF_VOID_P})

target_include_directories(g_ar_toolkit
  PUBLIC
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/g_ar_toolkit>"
  # add binary directory so auto-generated g_ar_toolkit_export.h is found
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

set_target_properties(g_ar_toolkit PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET default
  CXX_VISIBILITY_PRESET default
  VISIBILITY_INLINES_HIDDEN OFF
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# plaform specific definitions
if(WIN32)
  # windows.h definitions and allow mismatch co-routine due to zxing (https://devblogs.microsoft.com/oldnewthing/20230111-00/?p=107694)
  target_link_libraries(g_ar_toolkit PRIVATE
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
  # try and remove unused code sections
  target_compile_options(g_ar_toolkit PUBLIC
    "-fdata-sections" "-ffunction-sections" "-Wl,--gc-sections"
  )
 
  # don't use "lib"-prefix on linux
  set_target_properties(g_ar_toolkit PROPERTIES PREFIX "")
endif()

# specify target properties
# winrt only supports C++17 std
set_property(TARGET g_ar_toolkit PROPERTY CXX_STANDARD 17)

# generate export header
generate_export_header(g_ar_toolkit)

# Set the Library Filename to include the bitness
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(g_ar_toolkit PROPERTIES OUTPUT_NAME gartoolkit_${BITNESS}-${g_ar_toolkit_shared_binary_VERSION_MAJOR})
target_link_libraries(g_ar_toolkit 
  PUBLIC
  g_ar_toolkit_core
  PRIVATE
  opencv_imgcodecs
)

include(${CMAKE_CURRENT_LIST_DIR}/src/capture/CMakeLists.txt)

target_sources(g_ar_toolkit PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/capture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_new.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_blur.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_resize.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_draw_picture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_in_range.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_bitwise.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_read_file.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_write_file.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_add_weighted.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_flip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_crop.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_greyscale_to_argb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_insert.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_argb_channel_operations.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_canny.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_copy.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_copy_raw_memory_ptr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_copy_from_buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_copy_pixmap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_copy_array.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_negate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_transform.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_rotate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_threshold.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_morphology.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_find_nonzero.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image_colour.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shape_utils.cpp
)
