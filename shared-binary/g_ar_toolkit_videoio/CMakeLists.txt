# include tooling to generate export header
include(GenerateExportHeader)

# Find OpenCV
find_package(OpenCV 4.11 REQUIRED videoio)

# specify shared-library target
add_library(g_ar_toolkit_videoio SHARED)

generate_export_header(g_ar_toolkit_videoio)

# plaform specific definitions
if(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(g_ar_toolkit_videoio PROPERTIES PREFIX "")

  # set linker flags for ffmpeg
  target_link_options(g_ar_toolkit_videoio PRIVATE "-Wl,-Bsymbolic")
endif()

target_include_directories(g_ar_toolkit_videoio
  PUBLIC
    # add binary directory so auto-generated g_ar_toolkit_videoio_export.h is found
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

# specify target properties
# winrt only supports C++17 std
set_property(TARGET g_ar_toolkit_videoio PROPERTY CXX_STANDARD 17)
set_target_properties(g_ar_toolkit_videoio PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

# Set the Library Filename to include the bitness
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(g_ar_toolkit_videoio PROPERTIES OUTPUT_NAME gartoolkit_videoio_${BITNESS}-${g_ar_toolkit_shared_binary_VERSION_MAJOR})
target_link_libraries(g_ar_toolkit_videoio PRIVATE
  g_ar_toolkit
  opencv_videoio
)

target_sources(g_ar_toolkit_videoio PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/video_io.cpp
)
