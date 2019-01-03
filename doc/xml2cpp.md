dbus-cxx-xml2cpp {#xml2cpp}
==

\par Summary
\c dbus-cxx-xml2cpp generates proxy and adapter interfaces from modified DBus
XML introspection documents.

\par Commands
- \c -x \c --xml : The xml file to process
- \c -p \c --prefix : A prefix to prepend to all output files
- \c -f \c --file : Send output to files with naming <i>class</i>_proxy.h and <i>class</i>_adapter.h
- \c --proxy : Generate a proxy interface
- \c --adapter : Generate an adapter interface
- \c --verbose : Generate verbose processing info

\par Extended DBus XML attributes
\c dbus-cxx-xml2cpp in some cases needs information above and beyond that provided
in a dbus introspection document. This information can be supplied by adding
additional attributes to key nodes.

\par
The extended attributes per XML element are:
- \b node
  - \b cppname - The base name of the c++ class adapter and proxy
  - \b gen-namespace - The namespace the c++ adapter and proxy will be generated into
  - \b orig-namespace - The namespace of the object being adapted
  - \b dest - Used to create a default value for the \c dest parameter of a proxy
  - \b path - Used to create a default value for the \c path parameter of both proxies and adapters
  - \b cppinclude - A free form string that will follow a `#include` line in the adapter
  - \b file-prefix - A per-node filename prefix prepended to the class name on output
- \b interface
  - \b ignored - If set to "1" this interface will not be built for the proxy or adapter
- \b signal
  - \b accessor - The accessor method used to obtain the adaptee's signal
  - \b ignored - If set to "1" this signal will not be built for the proxy or adapter
- \b method
  - \b cppname - Renames the DBus method to the provided name in the proxy and adapter
  - \b const - Makes the proxy method a const method (useful when subclassing virtual methods)
  - \b virtual - Prefixes the method with keyword virtual (useful for improving readability of virtual methods)
- \b arg
 - \b cpptype - Defines the C++ type to be used when creating proxies/adapters for this argument. Useful for wrapping enumerations.
