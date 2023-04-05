
add_executable( filedescriptor-client filedescriptor_client.cpp  )
target_link_libraries( filedescriptor-client dbus-cxx rt )

add_executable( filedescriptor-server filedescriptor_server.cpp  )
target_link_libraries( filedescriptor-server dbus-cxx rt )
