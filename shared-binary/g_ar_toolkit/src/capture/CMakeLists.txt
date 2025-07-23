if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    target_sources(g_ar_toolkit PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/win/enumerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/win/stream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/win/utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/win/source_reader.cpp
    )

    target_sources(g_ar_toolkit PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/capture/win/utils.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/capture/win/source_reader.hpp
    )
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    target_sources(g_ar_toolkit PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/linux/enumerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/linux/stream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/linux/decoders.cpp
        ${CMAKE_CURRENT_LIST_DIR}/linux/utils.cpp
    )

    target_sources(g_ar_toolkit PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/capture/linux/decoders.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/capture/linux/utils.hpp
    )
endif()

target_sources(g_ar_toolkit PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/stream.cpp
)

target_sources(g_ar_toolkit PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/capture/stream.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/g_ar_toolkit/capture/enumerator.hpp
)
