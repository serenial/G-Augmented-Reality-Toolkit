target_sources(g_ar_toolkit PUBLIC
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

target_sources(g_ar_toolkit PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/lv_functions.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_error.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_image.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_u32_colour.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_str.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_enums.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_termination_critera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_point_correspondence.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_camera_parameters.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_vec_types.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lv_mat.cpp
)