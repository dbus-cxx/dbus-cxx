configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-robustness.sh
    ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-robustness.sh COPYONLY)

add_executable( test-robustness-sender send.cpp )
target_link_libraries( test-robustness-sender ${TEST_LINK} )
target_include_directories( test-robustness-sender PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-robustness-sender PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-robustness-sender PROPERTY CXX_STANDARD 17 )

add_executable( test-robustness-rx receive.cpp )
target_link_libraries( test-robustness-rx ${TEST_LINK} )
target_include_directories( test-robustness-rx PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-robustness-rx PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-robustness-rx PROPERTY CXX_STANDARD 17 )

add_test( NAME robust-test COMMAND dbus-wrapper-robustness.sh)
