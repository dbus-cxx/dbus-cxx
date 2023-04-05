add_executable( time-client timeval_dbus.cpp time_client.cpp  )
target_link_libraries( time-client dbus-cxx rt )

add_executable( time-server timeval_dbus.cpp time_server.cpp  )
target_link_libraries( time-server dbus-cxx rt )

