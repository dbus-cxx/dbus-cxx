include( ../../examples-common.cmake )

add_executable( struct-client struct_client.cpp  )
target_link_libraries( struct-client ${EXAMPLES_LINK} )

add_executable( struct-server struct_server.cpp  )
target_link_libraries( struct-server ${EXAMPLES_LINK} )

