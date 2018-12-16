include( ../../examples-common.cmake )

add_executable( filedescriptor-client filedescriptor_client.cpp  )
target_link_libraries( filedescriptor-client ${EXAMPLES_LINK} )

add_executable( filedescriptor-server filedescriptor_server.cpp  )
target_link_libraries( filedescriptor-server ${EXAMPLES_LINK} )
