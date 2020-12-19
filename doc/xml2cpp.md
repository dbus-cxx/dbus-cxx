dbus-cxx-xml2cpp {#xml2cpp}
==

## Summary
dbus-cxx-xml2cpp generates proxy and adapter interfaces from modified DBus
XML introspection documents.  By default, will print out a single(header-only)
class to `stdout`.  When sending to a file with `-f`, will create a .cpp and .h file.

## Commands
* `-x/--xml` : The xml file to process
* `-p/--prefix` : A prefix to prepend to all output files
* `-f/--file` : Send output to files with naming *class*\_proxy.h and *class*\_adapter.h.  
  Does not take any arguments, filename will be based on introspection document.
* `--proxy` : Generate a proxy interface.  Used for when you want to talk to a remote 
  application over the bus.
* `--adapter` : Generate an adapter interface.  Used for when you want to implement an
  interface.
* `--verbose` : Generate verbose processing info

## Extended DBus XML attributes
dbus-cxx-xml2cpp in some cases needs information above and beyond that provided
in a dbus introspection document. This information can be supplied by adding
additional attributes to key nodes.

The extended attributes per XML element are:
* node
  * **cppname** - The base name of the c++ class adapter and proxy
  * **gen-namespace** - The namespace the c++ adapter and proxy will be generated into
  * **dest** - Used to create a default value for the \c dest parameter of a proxy(e.g. `org.freedesktop.Notifications`)
  * **path** - Used to create a default value for the \c path parameter of both proxies and adapters(e.g. `/org/freedesktop/Notifications`)
* interface
  * **ignored** - If set to "true" this interface will not be built for the proxy or adapter
  * **cppname** - The The name of the generated C++ interface class

## Proxy Example

The following is an example of a DBus introspection document from [Avahi][avahi-server-browser] (with the appropriate
dbus-cxx annotations):

~~~{.xml}
<?xml version="1.0" standalone='no'?><!--*-nxml-*-->
<?xml-stylesheet type="text/xsl" href="introspect.xsl"?>
<!DOCTYPE node SYSTEM "introspect.dtd">

<!--
  This file is part of avahi.

  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  avahi is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA.
-->

<node cppname="ServiceBrowser" gen-namespace="Avahi">

  <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="data" type="s" direction="out" />
    </method>
  </interface>

  <interface name="org.freedesktop.Avahi.ServiceBrowser">

    <method name="Free"/>

    <method name="Start"/>

    <signal name="ItemNew">
      <arg name="interface" type="i"/>
      <arg name="protocol" type="i"/>
      <arg name="name" type="s"/>
      <arg name="type" type="s"/>
      <arg name="domain" type="s"/>
      <arg name="flags" type="u"/>
    </signal>

    <signal name="ItemRemove">
      <arg name="interface" type="i"/>
      <arg name="protocol" type="i"/>
      <arg name="name" type="s"/>
      <arg name="type" type="s"/>
      <arg name="domain" type="s"/>
      <arg name="flags" type="u"/>
    </signal>

    <signal name="Failure">
      <arg name="error" type="s"/>
    </signal>

    <signal name="AllForNow"/>

    <signal name="CacheExhausted"/>

  </interface>
</node>
~~~

To make a proxy, run `dbus-cxx-xml2cpp` as follows:

    dbus-cxx-xml2cpp  --proxy --xml=/path/to/org.freedesktop.Avahi.ServiceBrowser.xml --file

Several files will be output for this introspection file, one `ObjectProxy`:

~~~{.cpp}
#ifndef SERVICEBROWSERPROXY_H
#define SERVICEBROWSERPROXY_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
#include "org_freedesktop_Avahi_ServiceBrowserProxy.h"
namespace Avahi {
class ServiceBrowserProxy
 : public DBus::ObjectProxy {
public:
ServiceBrowserProxy(std::shared_ptr<DBus::Connection> conn, std::string dest, std::string path, DBus::ThreadForCalling signalCallingThread = DBus::ThreadForCalling::DispatcherThread );
public:
    static std::shared_ptr<ServiceBrowserProxy> create(std::shared_ptr<DBus::Connection> conn, std::string dest, std::string path, DBus::ThreadForCalling signalCallingThread = DBus::ThreadForCalling::DispatcherThread );
    std::shared_ptr<Avahi::org_freedesktop_Avahi_ServiceBrowserProxy> getorg_freedesktop_Avahi_ServiceBrowserInterface( );
protected:
    std::shared_ptr<Avahi::org_freedesktop_Avahi_ServiceBrowserProxy> m_org_freedesktop_Avahi_ServiceBrowserProxy;
};
} /* namespace Avahi */
#endif /* SERVICEBROWSERPROXY_H */

~~~

And one `InterfaceProxy`:

~~~{.cpp}
#ifndef ORG_FREEDESKTOP_AVAHI_SERVICEBROWSERPROXY_H
#define ORG_FREEDESKTOP_AVAHI_SERVICEBROWSERPROXY_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
namespace Avahi {
class org_freedesktop_Avahi_ServiceBrowserProxy
 : public DBus::InterfaceProxy {
protected:
org_freedesktop_Avahi_ServiceBrowserProxy(std::string name );
public:
    static std::shared_ptr<Avahi::org_freedesktop_Avahi_ServiceBrowserProxy> create(std::string name = "org.freedesktop.Avahi.ServiceBrowser" );
    void Free( );
    void Start( );
    std::shared_ptr<DBus::SignalProxy<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> signal_ItemNew( );
    std::shared_ptr<DBus::SignalProxy<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> signal_ItemRemove( );
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> signal_Failure( );
    std::shared_ptr<DBus::SignalProxy<void()>> signal_AllForNow( );
    std::shared_ptr<DBus::SignalProxy<void()>> signal_CacheExhausted( );
protected:
    std::shared_ptr<DBus::MethodProxy<void()>>  m_method_Free;
    std::shared_ptr<DBus::MethodProxy<void()>>  m_method_Start;
    std::shared_ptr<DBus::SignalProxy<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> m_signalproxy_ItemNew;
    std::shared_ptr<DBus::SignalProxy<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> m_signalproxy_ItemRemove;
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> m_signalproxy_Failure;
    std::shared_ptr<DBus::SignalProxy<void()>> m_signalproxy_AllForNow;
    std::shared_ptr<DBus::SignalProxy<void()>> m_signalproxy_CacheExhausted;
};
} /* namespace Avahi */
#endif /* ORG_FREEDESKTOP_AVAHI_SERVICEBROWSERPROXY_H */
~~~

Each interface that is found in the Introspection XML document will hav a new class created for it,
and added to the main proxy class for use.  The generated methods will act just as normal methods on
an object that exists in the local application, but they will get called ver the bus.  Signals
are prefixed with `signal_` to make it more obvious what they are.  Properties are similar to methods,
in that they do not have a prefix before them.


## Adapter Example

If we instead want to implement a class from an already-existing introspection document, we create an adapter
using the following command:

    dbus-cxx-xml2cpp  --adapter --xml=/path/to/org.freedesktop.Avahi.ServerBrowser.xml --file

Several classes will be generated.  As before, we get one class per interface that is in the instrospection file,
a master Adapter class, plus a pure virtual class that we must implement in order to receive method calls.

The main adapter class, which is what you will need to export on the bus:

~~~{.cpp}
#ifndef SERVICEBROWSERADAPTER_H
#define SERVICEBROWSERADAPTER_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
#include "org_freedesktop_Avahi_ServiceBrowserInterface.h"
namespace Avahi {
class ServiceBrowserAdapter
 : public DBus::Object {
public:
ServiceBrowserAdapter(std::shared_ptr<org_freedesktop_Avahi_ServiceBrowserInterface> _org_freedesktop_Avahi_ServiceBrowserInterface, std::string path );
public:
    static std::shared_ptr<ServiceBrowserAdapter> create(std::shared_ptr<DBus::Connection> connection, std::shared_ptr<org_freedesktop_Avahi_ServiceBrowserInterface> _org_freedesktop_Avahi_ServiceBrowserInterface, std::string path, DBus::ThreadForCalling calling_thread = DBus::ThreadForCalling::DispatcherThread );
};
} /* namespace Avahi */
#endif /* SERVICEBROWSERADAPTER_H */
~~~

The adapter class, whic contains the signals on this interface.  You must also give it a pointerto a pure virtual
class that implements the methods on this interface:

~~~{.cpp}
#ifndef ORG_FREEDESKTOP_AVAHI_SERVICEBROWSERINTERFACE_H
#define ORG_FREEDESKTOP_AVAHI_SERVICEBROWSERINTERFACE_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
#include "org_freedesktop_Avahi_ServiceBrowser.h"
namespace Avahi {
class org_freedesktop_Avahi_ServiceBrowserInterface
 : public DBus::Interface {
private:
org_freedesktop_Avahi_ServiceBrowserInterface(org_freedesktop_Avahi_ServiceBrowser* adaptee, std::string name );
public:
    static std::shared_ptr<org_freedesktop_Avahi_ServiceBrowserInterface> create(org_freedesktop_Avahi_ServiceBrowser* adaptee, std::string name = "org.freedesktop.Avahi.ServiceBrowser" );
    std::shared_ptr<DBus::Signal<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> signal_ItemNew( );
    void ItemNew(int32_t _interface, int32_t protocol, std::string name, std::string type, std::string domain, uint32_t flags );
    std::shared_ptr<DBus::Signal<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> signal_ItemRemove( );
    void ItemRemove(int32_t _interface, int32_t protocol, std::string name, std::string type, std::string domain, uint32_t flags );
    std::shared_ptr<DBus::Signal<void(std::string)>> signal_Failure( );
    void Failure(std::string error );
    std::shared_ptr<DBus::Signal<void()>> signal_AllForNow( );
    void AllForNow( );
    std::shared_ptr<DBus::Signal<void()>> signal_CacheExhausted( );
    void CacheExhausted( );
protected:
    std::shared_ptr<DBus::Signal<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> m_signal_ItemNew;
    std::shared_ptr<DBus::Signal<void(int32_t,int32_t,std::string,std::string,std::string,uint32_t)>> m_signal_ItemRemove;
    std::shared_ptr<DBus::Signal<void(std::string)>> m_signal_Failure;
    std::shared_ptr<DBus::Signal<void()>> m_signal_AllForNow;
    std::shared_ptr<DBus::Signal<void()>> m_signal_CacheExhausted;
};
} /* namespace Avahi */
#endif /* ORG_FREEDESKTOP_AVAHI_SERVICEBROWSERINTERFACE_H */
~~~

And finally, the pure virtual class that must be implemented in order to have methods that are called over the DBus:

~~~{.cpp}
#ifndef ORG_FREEDESKTOP_AVAHI_SERVICEBROWSER_H
#define ORG_FREEDESKTOP_AVAHI_SERVICEBROWSER_H

class org_freedesktop_Avahi_ServiceBrowser
{
public:
    virtual void Free( ) = 0;
    virtual void Start( ) = 0;
};
#endif /* ORG_FREEDESKTOP_AVAHI_SERVICEBROWSER_H */
~~~

The idea behind the pure virtual class is that any class that needs to be called over the DBus needs only a minimal
amount of support in order to add the ability to have methods called over the bus.

[avahi-server-browser]:https://github.com/lathiat/avahi/blob/d1e71b320d96d0f213ecb0885c8313039a09f693/avahi-daemon/org.freedesktop.Avahi.ServiceBrowser.xml
