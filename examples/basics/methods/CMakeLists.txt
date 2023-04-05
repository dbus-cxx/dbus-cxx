pkg_check_modules( popt REQUIRED popt )

add_executable( caller-c caller_c.cpp  )
target_link_libraries( caller-c dbus-cxx rt ${popt_LDFLAGS} )

add_executable( callee-c callee_c.cpp  )
target_link_libraries( callee-c dbus-cxx rt ${popt_LDFLAGS} )

add_executable( caller-raw caller_raw.cpp  )
target_link_libraries( caller-raw dbus-cxx rt ${popt_LDFLAGS} )

add_executable( callee-raw callee_raw.cpp  )
target_link_libraries( callee-raw dbus-cxx rt ${popt_LDFLAGS} )

add_executable( caller caller.cpp  )
target_link_libraries( caller dbus-cxx rt ${popt_LDFLAGS} )

add_executable( callee callee.cpp  )
target_link_libraries( callee dbus-cxx rt ${popt_LDFLAGS} )

add_executable( callee-2 callee_2.cpp  )
target_link_libraries( callee-2 dbus-cxx rt ${popt_LDFLAGS} )

add_executable( callee-object callee_object.cpp  )
target_link_libraries( callee-object dbus-cxx rt ${popt_LDFLAGS} )

add_executable( caller-object caller_object.cpp  )
target_link_libraries( caller-object dbus-cxx rt ${popt_LDFLAGS} )

add_executable( callee-object-inherited callee_object_inherited.cpp  )
target_link_libraries( callee-object-inherited dbus-cxx rt ${popt_LDFLAGS} )

add_executable( callee-dispatched callee_dispatched.cpp  )
target_link_libraries( callee-dispatched dbus-cxx rt ${popt_LDFLAGS} )

add_executable( caller-dispatched caller_dispatched.cpp  )
target_link_libraries( caller-dispatched dbus-cxx rt ${popt_LDFLAGS} )

#add_executable( quickstart-server quickstart_server_0.cpp  )
#target_link_libraries( quickstart-server ${EXAMPLES_LINK} )
