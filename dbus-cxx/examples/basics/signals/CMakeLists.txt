include( ../../examples-common.cmake )

add_executable( signal-emitter-c signal_emitter_c.cpp  )
target_link_libraries( signal-emitter-c ${EXAMPLES_LINK} )

add_executable( signal-receiver-c signal_receiver_c.cpp  )
target_link_libraries( signal-receiver-c ${EXAMPLES_LINK} )

add_executable( signal-emitter-raw signal_emitter_raw.cpp  )
target_link_libraries( signal-emitter-raw ${EXAMPLES_LINK} )

add_executable( signal-receiver-raw signal_receiver_raw.cpp  )
target_link_libraries( signal-receiver-raw ${EXAMPLES_LINK} )

add_executable( signal-emitter signal_emitter.cpp  )
target_link_libraries( signal-emitter ${EXAMPLES_LINK} )

add_executable( signal-emitter2 signal_emitter_2.cpp  )
target_link_libraries( signal-emitter2 ${EXAMPLES_LINK} )

add_executable( signal-emitter3 signal_emitter_3.cpp  )
target_link_libraries( signal-emitter3 ${EXAMPLES_LINK} )

add_executable( signal-receiver signal_receiver.cpp  )
target_link_libraries( signal-receiver ${EXAMPLES_LINK} )

add_executable( signal-receiver2 signal_receiver_2.cpp  )
target_link_libraries( signal-receiver2 ${EXAMPLES_LINK} )

add_executable( signal-emitter-object signal_emitter_object.cpp  )
target_link_libraries( signal-emitter-object ${EXAMPLES_LINK} )

add_executable( signal-receiver-object signal_receiver_object.cpp  )
target_link_libraries( signal-receiver-object ${EXAMPLES_LINK} )

add_executable( signal-emitter-array signal_emitter_array.cpp  )
target_link_libraries( signal-emitter-array ${EXAMPLES_LINK} )

add_executable( signal-receiver-array signal_receiver_array.cpp  )
target_link_libraries( signal-receiver-array ${EXAMPLES_LINK} )

