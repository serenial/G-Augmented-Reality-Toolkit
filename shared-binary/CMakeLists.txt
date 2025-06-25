if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # set to use Multithreaded Static Runtime
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  # vcpkg has some issues with not loading the static libs
  # so manually fix the MSVC runtime components used
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

# Specify Project
project(g_ar_toolkit_shared_binary VERSION 1.0.0 LANGUAGES C CXX)

add_subdirectory(g_ar_toolkit)
add_subdirectory(g_ar_toolkit_ar)
add_subdirectory(g_ar_toolkit_dnn)
add_subdirectory(g_ar_toolkit_fd)


# # include tooling to generate export header
# include(GenerateExportHeader)

# # export a json file with the compile commands
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(OpenCV_SHARED OFF)

# # Find OpenCV
# find_package(OpenCV 4.11 REQUIRED)


# # Find eigen3
# find_package(Eigen3 REQUIRED)

# # Find apriltag
# find_package(apriltag REQUIRED)

# # Find ZXing
# find_package(ZXing CONFIG REQUIRED)

# # specify shared-library target
# add_library(g_ar_toolkit SHARED)

# # add source
# include(${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt)

# # add compile definitions
# target_compile_definitions(g_ar_toolkit PUBLIC G_AR_TOOLKIT_BYTE_PACKING_${CMAKE_SIZEOF_VOID_P})

# target_include_directories(g_ar_toolkit
#   PUBLIC
#   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/g_ar_toolkit>"
#   # add binary directory so auto-generated g_ar_toolkit_export.h is found
#   "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
# )

# # plaform specific definitions
# if(WIN32)
#   # windows.h definitions and allow mismatch co-routine due to zxing (https://devblogs.microsoft.com/oldnewthing/20230111-00/?p=107694)
#   target_compile_definitions(g_ar_toolkit PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX /wd4068 _ALLOW_COROUTINE_ABI_MISMATCH)
#   target_link_libraries(g_ar_toolkit PRIVATE
#     mf
#     mfplat
#     Mfreadwrite
#     mfuuid
#     Strmiids
#     Rpcrt4
#     runtimeobject
#     Mfsensorgroup
#     shlwapi
#   )
# elseif(UNIX)
#   # # link against static system libs
#   # target_link_options(g_ar_toolkit PRIVATE
#   #   "-static-libgcc" "-static-libstdc++"
#   # )

#   # try and remove unused code sections
#   target_compile_options(g_ar_toolkit PRIVATE
#     "-fdata-sections" "-ffunction-sections" "-Wl,--gc-sections"
#   )

#   # set linker flags for ffmpeg
#   target_link_options(g_ar_toolkit PRIVATE "-Wl,-Bsymbolic") 
 
#   # don't use "lib"-prefix on linux
#   set_target_properties(g_ar_toolkit PROPERTIES PREFIX "")
# endif()

# # specify target properties
# # winrt only supports C++17 std
# set_property(TARGET g_ar_toolkit PROPERTY CXX_STANDARD 17)
# set_target_properties(g_ar_toolkit PROPERTIES
#   POSITION_INDEPENDENT_CODE ON
#   C_VISIBILITY_PRESET hidden
#   CXX_VISIBILITY_PRESET hidden
#   VISIBILITY_INLINES_HIDDEN ON
#   WINDOWS_EXPORT_ALL_SYMBOLS OFF
# )

# # generate export header
# generate_export_header(g_ar_toolkit)

# # Set the Library Filename to include the bitness
# math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
# set_target_properties(g_ar_toolkit PROPERTIES OUTPUT_NAME gartoolkit_${BITNESS}-${g_ar_toolkit_shared_binary_VERSION_MAJOR})
# target_link_libraries(g_ar_toolkit PRIVATE
#   opencv_core
#   opencv_imgproc
#   opencv_calib3d
#   opencv_objdetect
#   opencv_wechat_qrcode
#   opencv_imgcodecs
#   opencv_dnn
#   opencv_text
#   opencv_videoio
#   tinyobjloader::tinyobjloader
#   apriltag::apriltag
#   Eigen3::Eigen
#   ZXing::Core
#   ZXing::ZXing
# )

# Install Shared Library
INSTALL(TARGETS g_ar_toolkit g_ar_toolkit_ar g_ar_toolkit_dnn g_ar_toolkit_fd
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
)