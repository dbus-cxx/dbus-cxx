
set( TEST_INCLUDES ${dbus_INCLUDE_DIRS} ${sigc_INCLUDE_DIRS} ${cppgenerate_INCLUDE_DIRS} )
set( TEST_LINK dbus-cxx ${dbus_LDFLAGS} ${sigc_LDFLAGS} ${popt_LDFLAGS} ${expat_LDFLAGS} -lrt )

link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx 
    ${dbus_INCLUDE_DIRS} 
    ${sigc_INCLUDE_DIRS} )

add_executable( test-callmessage callmessagetests.cpp )
target_link_libraries( test-callmessage ${TEST_LINK} )
target_include_directories( test-callmessage PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-callmessage PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-callmessage PROPERTY CXX_STANDARD 17 )

add_test( NAME Callmessage-Bool COMMAND test-callmessage bool)
add_test( NAME Callmessage-Byte COMMAND test-callmessage byte)
add_test( NAME Callmessage-int16 COMMAND test-callmessage int16)
add_test( NAME Callmessage-uint16 COMMAND test-callmessage uint16)
add_test( NAME Callmessage-int32 COMMAND test-callmessage int32)
add_test( NAME Callmessage-uint32 COMMAND test-callmessage uint32)
add_test( NAME Callmessage-int64 COMMAND test-callmessage int64)
add_test( NAME Callmessage-uint64 COMMAND test-callmessage uint64)
add_test( NAME Callmessage-double COMMAND test-callmessage double)
add_test( NAME Callmessage-cstring COMMAND test-callmessage cstring)
add_test( NAME Callmessage-string COMMAND test-callmessage string)
add_test( NAME Callmessage-array_double COMMAND test-callmessage array_double)
add_test( NAME Callmessage-multiple COMMAND test-callmessage multiple)

add_executable( test-messageiterator messageiteratortests.cpp )
target_link_libraries( test-messageiterator ${TEST_LINK} )
target_include_directories( test-messageiterator PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-messageiterator PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-messageiterator PROPERTY CXX_STANDARD 17 )

add_test( NAME messageiterator-Bool COMMAND test-messageiterator bool)
add_test( NAME messageiterator-Byte COMMAND test-messageiterator byte)
add_test( NAME messageiterator-int16 COMMAND test-messageiterator int16)
add_test( NAME messageiterator-uint16 COMMAND test-messageiterator uint16)
add_test( NAME messageiterator-int32 COMMAND test-messageiterator int32)
add_test( NAME messageiterator-uint32 COMMAND test-messageiterator uint32)
add_test( NAME messageiterator-int64 COMMAND test-messageiterator int64)
add_test( NAME messageiterator-uint64 COMMAND test-messageiterator uint64)
add_test( NAME messageiterator-double COMMAND test-messageiterator double)
add_test( NAME messageiterator-cstring COMMAND test-messageiterator cstring)
add_test( NAME messageiterator-string COMMAND test-messageiterator string)
add_test( NAME messageiterator-signature COMMAND test-messageiterator signature)
add_test( NAME messageiterator-long_int COMMAND test-messageiterator long_int)
add_test( NAME messageiterator-unsigned_long_int COMMAND test-messageiterator unsigned_long_int)
add_test( NAME messageiterator-array_int COMMAND test-messageiterator array_int)
add_test( NAME messageiterator-array_string COMMAND test-messageiterator array_string)
add_test( NAME messageiterator-array_array_string COMMAND test-messageiterator array_array_string)
add_test( NAME messageiterator-filedescriptor COMMAND test-messageiterator filedescriptor)
add_test( NAME messageiterator-multiple COMMAND test-messageiterator multiple)
add_test( NAME messageiterator-struct COMMAND test-messageiterator struct)
add_test( NAME messageiterator-variant COMMAND test-messageiterator variant)
add_test( NAME messageiterator-map-variant COMMAND test-messageiterator map_string_variant)

add_test( NAME messageiterator-Bool2 COMMAND test-messageiterator bool-2)
add_test( NAME messageiterator-Byte2 COMMAND test-messageiterator byte-2)
add_test( NAME messageiterator-int16-2 COMMAND test-messageiterator int16-2)
add_test( NAME messageiterator-uint16-2 COMMAND test-messageiterator uint16-2)
add_test( NAME messageiterator-int32-2 COMMAND test-messageiterator int32-2)
add_test( NAME messageiterator-uint32-2 COMMAND test-messageiterator uint32-2)
add_test( NAME messageiterator-int64-2 COMMAND test-messageiterator int64-2)
add_test( NAME messageiterator-uint64-2 COMMAND test-messageiterator uint64-2)
add_test( NAME messageiterator-double-2 COMMAND test-messageiterator double-2)
add_test( NAME messageiterator-cstring-2 COMMAND test-messageiterator cstring-2)
add_test( NAME messageiterator-string-2 COMMAND test-messageiterator string-2)
add_test( NAME messageiterator-signature-2 COMMAND test-messageiterator signature-2)
add_test( NAME messageiterator-long_int-2 COMMAND test-messageiterator long_int-2)
add_test( NAME messageiterator-unsigned_long_int-2 COMMAND test-messageiterator unsigned_long_int-2)
add_test( NAME messageiterator-array_int-2 COMMAND test-messageiterator array_int-2)
add_test( NAME messageiterator-array_string-2 COMMAND test-messageiterator array_string-2)
add_test( NAME messageiterator-filedescriptor-2 COMMAND test-messageiterator filedescriptor-2)
add_test( NAME messageiterator-multiple-2 COMMAND test-messageiterator multiple-2)
add_test( NAME messageiterator-struct-2 COMMAND test-messageiterator struct-2)
add_test( NAME messageiterator-variant-2 COMMAND test-messageiterator variant-2)

add_executable( test-path pathclasstests.cpp )
target_link_libraries( test-path ${TEST_LINK} )
target_include_directories( test-path PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-path PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-path PROPERTY CXX_STANDARD 17 )

add_test( NAME path-valid COMMAND test-path test_valid)
add_test( NAME path-invalid-chars COMMAND test-path test_invalid_characters)
add_test( NAME path-invalid-trail-slash COMMAND test-path test_invalid_trailing_slash)
add_test( NAME path-invalid-empty-path COMMAND test-path test_invalid_empty_path)
add_test( NAME path-invalid-double-slash COMMAND test-path test_invalid_double_slash)
add_test( NAME path-invalid-mising-lead-slash COMMAND test-path test_invalid_missing_leading_slash)
add_test( NAME path-decompose-valid COMMAND test-path decompose_valid)
add_test( NAME path-decompose-root COMMAND test-path decompose_root)
add_test( NAME path-decompose-invalid COMMAND test-path decompose_invalid)
add_test( NAME path-decompose-empty COMMAND test-path decompose_empty)
add_test( NAME path-append-valid COMMAND test-path append_valid)
add_test( NAME path-append-valid-mult COMMAND test-path append_valid_multiple)
add_test( NAME path-append-valid-lead-slash COMMAND test-path append_valid_leading_slash)
add_test( NAME path-append-valid-trail-slash COMMAND test-path append_valid_trailing_slash)
add_test( NAME path-append-invalid-empty COMMAND test-path append_invalid_empty)
add_test( NAME path-append-invalid-chars COMMAND test-path append_invalid_empty)
add_test( NAME path-append-invalid-double_slash COMMAND test-path append_invalid_double_slash)
add_test( NAME path-append-invalid-root COMMAND test-path append_invalid_root)

add_executable( test-connection connectiontests.cpp )
target_link_libraries( test-connection ${TEST_LINK} )
target_include_directories( test-connection PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-connection PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-connection PROPERTY CXX_STANDARD 17 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper.sh ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper.sh COPYONLY)
add_test( NAME connection-proxy-create1 COMMAND dbus-wrapper.sh test-connection create_signal_proxy)
add_test( NAME connection-proxy-get-iface COMMAND dbus-wrapper.sh test-connection get_signal_proxy_by_iface)
add_test( NAME connection-proxy-get-iface-name COMMAND dbus-wrapper.sh test-connection get_signal_proxy_by_iface_and_name)
add_test( NAME connection-proxy-create_signal COMMAND dbus-wrapper.sh test-connection create_void_signal)
add_test( NAME connection-proxy-create_int_signal COMMAND dbus-wrapper.sh test-connection create_int_signal)

#
# Object Tests
#
add_executable( object-tests objecttests.cpp )
target_link_libraries( object-tests ${TEST_LINK} )
target_include_directories( object-tests PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( object-tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET object-tests PROPERTY CXX_STANDARD 17 )

add_test( NAME create-object-proxy COMMAND dbus-wrapper.sh object-tests proxy_create)
add_test( NAME object-proxy-create-method COMMAND dbus-wrapper.sh object-tests proxy_create_method1)
add_test( NAME export-method COMMAND dbus-wrapper.sh object-tests export_method)

#
# Data Sending tests - make sure we can actually send data across the bus correctly
#
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-data-tests.sh ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-data-tests.sh COPYONLY)
add_executable( data-tests datatests.cpp custom-type.cpp )
target_link_libraries( data-tests ${TEST_LINK} )
target_include_directories( data-tests PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( data-tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET data-tests PROPERTY CXX_STANDARD 17 )

add_test( NAME send-integers COMMAND dbus-wrapper-data-tests.sh send_integers)
add_test( NAME call-void-method COMMAND dbus-wrapper-data-tests.sh void_method)
add_test( NAME call-void-custom-method COMMAND dbus-wrapper-data-tests.sh void_custom)
add_test( NAME call-int-custom-method COMMAND dbus-wrapper-data-tests.sh send_intcustom)
add_test( NAME call-int-vec-custom-method COMMAND dbus-wrapper-data-tests.sh send_intveccustom)

#
# Signal tests - make sure we can tx and rx singals correctly
#
add_executable( signal-tests signaltests.cpp )
target_link_libraries( signal-tests ${TEST_LINK} )
target_include_directories( signal-tests PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( signal-tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET signal-tests PROPERTY CXX_STANDARD 17 )

add_test( NAME create-signal COMMAND dbus-wrapper.sh signal-tests create)
add_test( NAME signal-tx-rx COMMAND dbus-wrapper.sh signal-tests tx_rx)
add_test( NAME void-signal-tx-rx COMMAND dbus-wrapper.sh signal-tests void_txrx)
add_test( NAME path-match-rx COMMAND dbus-wrapper.sh signal-tests path_match_only)
add_test( NAME interface-match-rx COMMAND dbus-wrapper.sh signal-tests interface_match_only)
add_test( NAME member-match-rx COMMAND dbus-wrapper.sh signal-tests member_match_only)
add_test( NAME multiple-handlers COMMAND dbus-wrapper.sh signal-tests multiple_handlers)
add_test( NAME remove-handler COMMAND dbus-wrapper.sh signal-tests remove_handler)

#
# Introspection Tests - make sure that we can introspect and get the correct data back
#
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-introspection-tests.sh ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-introspection-tests.sh COPYONLY)
add_executable( introspection-tests introspection-test.cpp )
target_link_libraries( introspection-tests ${TEST_LINK} ${expat_LDFLAGS} )
target_include_directories( introspection-tests PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( introspection-tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET introspection-tests PROPERTY CXX_STANDARD 17 )

add_test( NAME basic-introspect COMMAND dbus-wrapper-introspection-tests.sh basic_introspect)
add_test( NAME validate-xml COMMAND dbus-wrapper-introspection-tests.sh valid_xml)
add_test( NAME validate-interfaces COMMAND dbus-wrapper-introspection-tests.sh validate_interfaces)
add_test( NAME add-method-found COMMAND dbus-wrapper-introspection-tests.sh add_method_under_foowhat)
add_test( NAME first-argname-correct COMMAND dbus-wrapper-introspection-tests.sh first_argname_correct)
add_test( NAME second-argname-correct COMMAND dbus-wrapper-introspection-tests.sh second_argname_correct)
add_test( NAME signal-found COMMAND dbus-wrapper-introspection-tests.sh signal_found)

#
# Custom DBus address test - make sure that we can connect to an arbitary bus, not just the default session bus
#
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-customaddress.sh ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-customaddress.sh COPYONLY)
add_executable( customaddress-tests customaddress-test.cpp )
target_link_libraries( customaddress-tests ${TEST_LINK} )
target_include_directories( customaddress-tests PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( customaddress-tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET customaddress-tests PROPERTY CXX_STANDARD 17 )

add_test( NAME custom-address COMMAND dbus-wrapper-customaddress.sh customaddress-tests)

#
# Signature tests - make sure that given a signature, we can validate it and iterate over it
#
add_executable( test-signature signaturetests.cpp )
target_link_libraries( test-signature ${TEST_LINK} )
target_include_directories( test-signature PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-signature PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-signature PROPERTY CXX_STANDARD 17 )

add_test( NAME signature-create COMMAND test-signature create)
add_test( NAME signature-arrayint COMMAND test-signature iterate_arrayint)
add_test( NAME signature-arrayint2 COMMAND test-signature iterate_arrayint2)
add_test( NAME signature-nested-array COMMAND test-signature iterate_nested_array)
add_test( NAME signature-multiple-types COMMAND test-signature iterate_multiple_types)
add_test( NAME signature-dictionary COMMAND test-signature iterate_dictionary)
add_test( NAME signature-struct COMMAND test-signature iterate_struct)
