pkg_check_modules( dbus REQUIRED IMPORTED_TARGET dbus-1 )
pkg_check_modules( popt REQUIRED IMPORTED_TARGET popt )

set( TEST_LINK dbus-cxx PkgConfig::dbus PkgConfig::sigc PkgConfig::popt PkgConfig::expat ${LIBRT} )

#
# Add a test that generates code from the XML file and then compiles it
#
# param1: The XML file to run the code generator against
# param2: must be either "proxy" or "adapter"
# param3: The list of files the generator will create
# param2: The test .cpp file that needs the generated code
function(add_xml_test xml_file proxy_or_adapter gen_files test_compile)
    set(test_binary test-${test_compile})
    set(generated_files_target ${test_compile}-generated-files)
    set(test_name ${test_compile})

    add_custom_command(
        OUTPUT ${gen_files}
        COMMAND dbus-cxx-xml2cpp --file --${proxy_or_adapter} --xml=${CMAKE_CURRENT_SOURCE_DIR}/${xml_file}
        VERBATIM )

    # The generated files will be in CMAKE_CURRENT_BINARY_DIR
    foreach(single_file ${gen_files})
        set(real_generated ${real_generated} ${CMAKE_CURRENT_BINARY_DIR}/${single_file})
    endforeach(single_file)

    # Add all of our generated .cpp files to the test binary
    foreach(single_file ${gen_files})
        string(FIND ${single_file} ".cpp" is_cpp)
        if( ${is_cpp} GREATER 0 )
            set(cpp_generated ${cpp_generated} ${single_file})
        endif( ${is_cpp} GREATER 0 )
    endforeach(single_file)

    # All of the binaries should link with the library in our binary directory
    #link_directories( ${CMAKE_BINARY_DIR} )

    # The we need a test_compile-generate executable to generate the files needed for this test
    #add_executable( ${test_binary} ${test_compile}.cpp )
    #target_link_libraries( ${test_compile} dbus-cxx )
    #target_include_directories( ${test_compile} PUBLIC ${CMAKE_SOURCE_DIR} )

    # The main test
    add_executable( ${test_binary} ${test_name}.cpp ${cpp_generated} )
    target_link_libraries( ${test_binary} ${TEST_LINK} )
    target_include_directories( ${test_binary} PUBLIC ${CMAKE_SOURCE_DIR} )
    target_include_directories( ${test_binary} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
    set_property( TARGET ${test_binary} PROPERTY CXX_STANDARD 17 )

    # We need a custom target that depends on the generated files
    add_custom_target( ${generated_files_target} DEPENDS ${real_generated})

    # The test must depend on the generator and the generated files
    add_dependencies( ${test_binary} ${generated_files_target} )

    # Finally, add a test for cmake
    add_test( NAME ${test_name} COMMAND dbus-run-session ./${test_binary} )
endfunction()

add_xml_test( "some.thing.xml" proxy "some_nameProxy.h;some_nameProxy.cpp;some_interfaceProxy.h;some_interfaceProxy.cpp" some-name-proxy )
add_xml_test( tacointrospection.xml proxy "DbusTacoProxy.h;DbusTacoProxy.cpp;com_amazon_josharenson_TacoOrdererInterfaceProxy.h;com_amazon_josharenson_TacoOrdererInterfaceProxy.cpp" taco-proxy )
add_xml_test( tacointrospection.xml adapter "DbusTacoAdapter.h;DbusTacoAdapter.cpp;com_amazon_josharenson_TacoOrdererInterface.h;com_amazon_josharenson_TacoOrdererInterface.cpp;com_amazon_josharenson_TacoOrdererInterfaceInterface.h;com_amazon_josharenson_TacoOrdererInterfaceInterface.cpp" taco-adapter )
add_xml_test( containertypes.xml adapter "structtypesAdapter.h;structtypesAdapter.cpp;test_struct_interface.h;test_struct_interface.cpp;test_struct_interfaceInterface.h;test_struct_interfaceInterface.cpp" struct-adapter )
add_xml_test( simpletypes.xml adapter "SimpleTypesAdapter.h;SimpleTypesAdapter.cpp;test_simpletype_interfaceInterface.h;test_simpletype_interfaceInterface.cpp;test_simpletype_interface.h;test_simpletype_interface.cpp" simple-adapter )
add_xml_test( signal-interface.xml adapter "signalNameAdapter.h;signalNameAdapter.cpp;signal_name_interfaceInterface.h;signal_name_interfaceInterface.cpp;signal_name_interface.h;signal_name_interface.cpp" signal-adapter )
add_xml_test( signal-interface.xml proxy "signalNameProxy.h;signalNameProxy.cpp;signal_name_interfaceProxy.h;signal_name_interfaceProxy.cpp" signal-proxy )
add_xml_test( org.freedesktop.NetworkManager.xml proxy "NetworkManagerProxy.h;NetworkManagerProxy.cpp;org_freedesktop_NetworkManagerProxy.h;org_freedesktop_NetworkManagerProxy.cpp" nm-proxy )
add_xml_test( org.freedesktop.Avahi.Server.xml proxy "AvahiServerProxy.h;AvahiServerProxy.cpp;org_freedesktop_Avahi_ServerProxy.h;org_freedesktop_Avahi_ServerProxy.cpp;org_freedesktop_Avahi_Server2Proxy.h;org_freedesktop_Avahi_Server2Proxy.cpp" avahi-proxy )
add_xml_test( multireturn.xml proxy "test_multireturn_interfaceProxy.h;test_multireturn_interfaceProxy.cpp;MultiReturnProxy.h;MultiReturnProxy.cpp" test-multireturn )

#
# Make sure that our generated code will work
#
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-signal.sh ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-signal.sh COPYONLY)

add_test( NAME signal-emits-and-rx-generated COMMAND ./dbus-wrapper-signal.sh )
