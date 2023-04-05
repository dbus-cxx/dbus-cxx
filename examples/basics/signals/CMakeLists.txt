add_executable( signal-emitter-c signal_emitter_c.cpp  )
target_link_libraries( signal-emitter-c dbus-cxx rt )

add_executable( signal-receiver-c signal_receiver_c.cpp  )
target_link_libraries( signal-receiver-c dbus-cxx rt )

add_executable( signal-emitter-raw signal_emitter_raw.cpp  )
target_link_libraries( signal-emitter-raw dbus-cxx rt )

add_executable( signal-receiver-raw signal_receiver_raw.cpp  )
target_link_libraries( signal-receiver-raw dbus-cxx rt )

add_executable( signal-emitter signal_emitter.cpp  )
target_link_libraries( signal-emitter dbus-cxx rt )

add_executable( signal-emitter2 signal_emitter_2.cpp  )
target_link_libraries( signal-emitter2 dbus-cxx rt )

add_executable( signal-emitter3 signal_emitter_3.cpp  )
target_link_libraries( signal-emitter3 dbus-cxx rt )

add_executable( signal-receiver signal_receiver.cpp  )
target_link_libraries( signal-receiver dbus-cxx rt )

add_executable( signal-receiver2 signal_receiver_2.cpp  )
target_link_libraries( signal-receiver2 dbus-cxx rt )

add_executable( signal-emitter-object signal_emitter_object.cpp  )
target_link_libraries( signal-emitter-object dbus-cxx rt )

add_executable( signal-receiver-object signal_receiver_object.cpp  )
target_link_libraries( signal-receiver-object dbus-cxx rt )

add_executable( signal-emitter-array signal_emitter_array.cpp  )
target_link_libraries( signal-emitter-array dbus-cxx rt )

add_executable( signal-receiver-array signal_receiver_array.cpp  )
target_link_libraries( signal-receiver-array dbus-cxx rt )

