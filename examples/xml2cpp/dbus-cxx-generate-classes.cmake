# Example usage:
# add_target( target_name ... )
# dbus_cxx_generate_classes( XMLFILE introspect.xml
#                           TARGET target_name
#                           GEN_CPP introspectProxy.cpp
#                           GEN_PROXY )
#
# XMLFILE is the location of the XML file to use
# TARGET is the target to add the generated code to so that it will get compiled
# GEN_CPP is the name of the auto-generated CPP file - this is 
#     dependent on the 'cppname' attribute in the XML file
# GEN_PROXY tells it to generate a proxy class
# GEN_ADAPTER tells it to generate an adapter class

function(dbus_cxx_generate_classes)
  set(options GEN_PROXY GEN_ADAPTER)
  set(oneValueArgs XMLFILE TARGET)
  set(multiValueArgs GEN_CPP GEN_H)

  cmake_parse_arguments(DBUS_CXX_GEN_CLASSES 
                        "${options}" 
                        "${oneValueArgs}" 
                        "${multiValueArgs}" 
                        ${ARGN} )

  set( xml2cpp_args "--file" )
  if( ${DBUS_CXX_GEN_CLASSES_GEN_PROXY} )
      set( xml2cpp_args ${xml2cpp_args} "--proxy" )
  endif( ${DBUS_CXX_GEN_CLASSES_GEN_PROXY} )

  if( ${DBUS_CXX_GEN_CLASSES_GEN_ADAPTER} )
      set( xml2cpp_args ${xml2cpp_args} "--adapter" )
  endif( ${DBUS_CXX_GEN_CLASSES_GEN_ADAPTER} )

  set( xml_file ${CMAKE_CURRENT_SOURCE_DIR}/${DBUS_CXX_GEN_CLASSES_XMLFILE} )
  set( xml2cpp_args ${xml2cpp_args} --xml=${xml_file} )

  # Add a custom command which calls xml2cpp to generate the headers/cpp
  add_custom_command(
      OUTPUT
          ${DBUS_CXX_GEN_CLASSES_GEN_CPP}
      COMMAND 
          dbus-cxx-xml2cpp ${xml2cpp_args}
      DEPENDS
          ${xml_file}
      WORKING_DIRECTORY 
          ${CMAKE_CURRENT_BINARY_DIR}
  )

  # The custom target tells cmake that we need to generate this
  add_custom_target( generate-${DBUS_CXX_GEN_CLASSES_GEN_CPP} 
       DEPENDS
          ${DBUS_CXX_GEN_CLASSES_GEN_CPP} 
  )

  add_dependencies( ${DBUS_CXX_GEN_CLASSES_TARGET}
          generate-${DBUS_CXX_GEN_CLASSES_GEN_CPP} )

  target_sources( ${DBUS_CXX_GEN_CLASSES_TARGET}
          PUBLIC ${DBUS_CXX_GEN_CLASSES_GEN_CPP} )

endfunction()
