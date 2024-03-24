include( ../../examples-common.cmake )

pkg_check_modules( popt REQUIRED IMPORTED_TARGET popt )

if( ${dbus_FOUND} )
	message("libdbus found, building libdbus examples")
	add_executable( caller-c caller_c.cpp  )
	target_link_libraries( caller-c ${EXAMPLES_LINK} PkgConfig::popt PkgConfig::dbus )

	add_executable( callee-c callee_c.cpp  )
	target_link_libraries( callee-c ${EXAMPLES_LINK} PkgConfig::popt PkgConfig::dbus )
endif( ${dbus_FOUND} )

add_executable( caller caller.cpp  )
target_link_libraries( caller ${EXAMPLES_LINK} PkgConfig::popt )

add_executable( callee callee.cpp  )
target_link_libraries( callee ${EXAMPLES_LINK} PkgConfig::popt )

add_executable( callee-2 callee_2.cpp  )
target_link_libraries( callee-2 ${EXAMPLES_LINK} PkgConfig::popt )

add_executable( callee-object callee_object.cpp  )
target_link_libraries( callee-object ${EXAMPLES_LINK} PkgConfig::popt )

add_executable( caller-object caller_object.cpp  )
target_link_libraries( caller-object ${EXAMPLES_LINK} PkgConfig::popt )

add_executable( callee-object-inherited callee_object_inherited.cpp  )
target_link_libraries( callee-object-inherited ${EXAMPLES_LINK} PkgConfig::popt )

add_executable( caller-dispatched caller_dispatched.cpp  )
target_link_libraries( caller-dispatched ${EXAMPLES_LINK} PkgConfig::popt )
