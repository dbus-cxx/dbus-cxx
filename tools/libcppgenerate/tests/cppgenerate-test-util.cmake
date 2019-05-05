#
# Add a test for libcppgenerate
#
# param1: generator_name - this is the name of the .cpp that generates the files needed for this test
# param2: generated_files - this is the list of files that the generator creates
# param3: test_name - The name of the test(.cpp file)
function(add_cppgenerate_test generator_name generated_files test_name)
    set(generator_binary ${generator_name}-generate)
    set(test_binary test-${test_name})
    set(generated_files_target ${generator_name}-generated-files)

    add_custom_command( 
        OUTPUT ${generated_files}
        COMMAND ${generator_binary}
        VERBATIM )

    # The generated files will be in CMAKE_CURRENT_BINARY_DIR
    foreach(single_file ${generated_files})
        set(real_generated ${real_generated} ${CMAKE_CURRENT_BINARY_DIR}/${single_file})
    endforeach(single_file)

    # Add all of our generated .cpp files to the test binary
    foreach(single_file ${generated_files})
        string(FIND ${single_file} ".cpp" is_cpp)
        if( ${is_cpp} GREATER 0 )
            set(cpp_generated ${cpp_generated} ${single_file})
        endif( ${is_cpp} GREATER 0 )
    endforeach(single_file)
message("cppgenerated ${cpp_generated}")

    # All of the binaries should link with the library in our binary directory
    link_directories( ${CMAKE_BINARY_DIR} )

    # The we need a generator_name-generate executable to generate the files needed for this test
    add_executable( ${generator_binary} ${generator_name}.cpp )
    target_link_libraries( ${generator_binary} cppgenerate )
    target_include_directories( ${generator_binary} PUBLIC ${CMAKE_SOURCE_DIR} )

    # The main test
    add_executable( ${test_binary} ${test_name}.cpp ${cpp_generated} )
    target_link_libraries( ${test_binary} cppgenerate )
    target_include_directories( ${test_binary} PUBLIC ${CMAKE_SOURCE_DIR} )
    target_include_directories( ${test_binary} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )

    # We need a custom target that depends on the generated files
    add_custom_target( ${generated_files_target} DEPENDS ${real_generated})

    # The test must depend on the generator and the generated files
    add_dependencies( ${test_binary} ${generator_binary} ${generated_files_target} )

    # Finally, add a test for cmake
    add_test( NAME ${test_name} COMMAND ${test_binary} )

endfunction()

#
# Add a simple test that does not need any code to be generated
#
# param1: test_name - the name of the test(.cpp file name)
function(add_cppgenerate_test_simple test_name)
    link_directories( ${CMAKE_BINARY_DIR} )

    add_executable( test-${test_name} ${test_name}.cpp )
    target_link_libraries( test-${test_name} cppgenerate )
    target_include_directories( test-${test_name} PUBLIC ${CMAKE_SOURCE_DIR} )
    target_include_directories( test-${test_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
    
    add_test( NAME ${test_name} COMMAND test-${test_name} )
endfunction()
