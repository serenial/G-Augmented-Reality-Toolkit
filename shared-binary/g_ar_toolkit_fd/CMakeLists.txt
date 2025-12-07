# include tooling to generate export header
include(GenerateExportHeader)

# Find apriltag
find_package(apriltag 3.4.5 REQUIRED)

# Find OpenCV
find_package(OpenCV 4.11 REQUIRED objdetect)

# specify shared-library target
add_library(g_ar_toolkit_fd SHARED)

generate_export_header(g_ar_toolkit_fd)

# plaform specific definitions
if(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(g_ar_toolkit_fd PROPERTIES PREFIX "")
endif()

target_include_directories(g_ar_toolkit_fd
  PUBLIC

  # add binary directory so auto-generated g_ar_toolkit_fd_export.h is found
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)
set_target_properties(g_ar_toolkit_fd PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

# Set the Library Filename to include the bitness
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(g_ar_toolkit_fd PROPERTIES OUTPUT_NAME gartoolkit_fd_${BITNESS}-${g_ar_toolkit_shared_binary_VERSION_MAJOR})
target_link_libraries(g_ar_toolkit_fd PRIVATE
  g_ar_toolkit
  apriltag::apriltag
  opencv_objdetect
)

target_sources(g_ar_toolkit_fd PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/feature_detection_apriltag.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/feature_detection_contours.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/feature_detection_hough.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/feature_detection_qr_encoder.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/feature_detection_qr_detector.cpp
)

if(NOT OMIT_ZXING)
  # Find ZXing
  find_package(ZXing CONFIG REQUIRED)

  target_link_libraries(g_ar_toolkit_fd PRIVATE
    ZXing::Core
    ZXing::ZXing
  )

  target_sources(g_ar_toolkit_fd PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/feature_detection_zxing.cpp
  )
endif()