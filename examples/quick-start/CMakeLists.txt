include( ../examples-common.cmake )

add_executable( quickstart-client quickstart_client_0.cpp  )
target_link_libraries( quickstart-client ${EXAMPLES_LINK} )

add_executable( quickstart-server quickstart_server_0.cpp  )
target_link_libraries( quickstart-server ${EXAMPLES_LINK} )
