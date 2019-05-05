set( EXAMPLES 
    cpp_class1.cpp 
    cpp_class2.cpp
    cpp_class3.cpp
    cpp_class4.cpp
    cpp_inheritance.cpp
)

foreach( example ${EXAMPLES} )
    string( LENGTH ${example} example_len )
    math(EXPR example_len "${example_len}-4")
    string( SUBSTRING ${example} 0 ${example_len} example_name )

    link_directories( ${CMAKE_BINARY_DIR} )
    add_executable( ${example_name} ${example} )
    target_link_libraries( ${example_name} cppgenerate )
    target_include_directories( ${example_name} PUBLIC ${CMAKE_SOURCE_DIR} )
endforeach( example )

