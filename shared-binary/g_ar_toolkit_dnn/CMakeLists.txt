# include tooling to generate export header
include(GenerateExportHeader)

# Find OpenCV
find_package(OpenCV 4.11 REQUIRED objdetect wechat_qrcode dnn)

# specify shared-library target
add_library(g_ar_toolkit_dnn SHARED)

generate_export_header(g_ar_toolkit_dnn)

# plaform specific definitions
if(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(g_ar_toolkit_dnn PROPERTIES PREFIX "")
endif()

target_include_directories(g_ar_toolkit_dnn
  PUBLIC
    # add binary directory so auto-generated g_ar_toolkit_dnn_export.h is found
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

# Set the Library Filename to include the bitness
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(g_ar_toolkit_dnn PROPERTIES OUTPUT_NAME gartoolkit_dnn_${BITNESS}-${g_ar_toolkit_shared_binary_VERSION_MAJOR})
target_link_libraries(g_ar_toolkit_dnn PRIVATE
  g_ar_toolkit
  opencv_objdetect
  opencv_wechat_qrcode
  opencv_dnn
)

target_sources(g_ar_toolkit_dnn PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/dnn_barcode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/dnn_we_chat_qr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/dnn_text.cpp
)