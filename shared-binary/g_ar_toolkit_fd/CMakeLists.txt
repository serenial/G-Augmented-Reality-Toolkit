target_sources(g_ar_toolkit_fd PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/feature_detection_apriltag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/feature_detection_contours.cpp
    ${CMAKE_CURRENT_LIST_DIR}/feature_detection_hough.cpp
    ${CMAKE_CURRENT_LIST_DIR}/feature_detection_qr_encoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/feature_detection_qr_detector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/feature_detection_zxing.cpp
)