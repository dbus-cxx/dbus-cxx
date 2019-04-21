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
  * **orig-namespace** - The namespace of the object being adapted
  * **dest** - Used to create a default value for the \c dest parameter of a proxy
  * **path** - Used to create a default value for the \c path parameter of both proxies and adapters
  * **cppinclude** - A free form string that will follow a `#include` line in the adapter
  * **file-prefix** - A per-node filename prefix prepended to the class name on output
* interface
  * **ignored** - If set to "1" this interface will not be built for the proxy or adapter
* signal
  * **accessor** - The accessor method used to obtain the adaptee's signal
  * **ignored** - If set to "1" this signal will not be built for the proxy or adapter
* method
  * **cppname** - Renames the DBus method to the provided name in the proxy and adapter
  * **const** - Makes the proxy method a const method (useful when subclassing virtual methods)
  * **virtual** - Prefixes the method with keyword virtual (useful for improving readability of virtual methods)
* arg
 * **cpptype** - Defines the C++ type to be used when creating proxies/adapters for this argument. Useful for wrapping enumerations.

## Proxy Example

The following is an example of a DBus introspection document from [Avahi][avahi-server]:

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

<node>

 <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="data" type="s" direction="out"/>
    </method>
  </interface>

  <interface name="org.freedesktop.Avahi.Server">

    <method name="GetVersionString">
      <arg name="version" type="s" direction="out"/>
    </method>

    <method name="GetAPIVersion">
      <arg name="version" type="u" direction="out"/>
    </method>

    <method name="GetHostName">
      <arg name="name" type="s" direction="out"/>
    </method>
    <method name="SetHostName">
      <arg name="name" type="s" direction="in"/>
    </method>
    <method name="GetHostNameFqdn">
      <arg name="name" type="s" direction="out"/>
    </method>
    <method name="GetDomainName">
      <arg name="name" type="s" direction="out"/>
    </method>

    <method name="IsNSSSupportAvailable">
      <arg name="yes" type="b" direction="out"/>
    </method>

    <method name="GetState">
      <arg name="state" type="i" direction="out"/>
    </method>

    <signal name="StateChanged">
      <arg name="state" type="i"/>
      <arg name="error" type="s"/>
    </signal>

    <method name="GetLocalServiceCookie">
      <arg name="cookie" type="u" direction="out"/>
    </method>

    <method name="GetAlternativeHostName">
      <arg name="name" type="s" direction="in"/>
      <arg name="name" type="s" direction="out"/>
    </method>

    <method name="GetAlternativeServiceName">
      <arg name="name" type="s" direction="in"/>
      <arg name="name" type="s" direction="out"/>
    </method>

    <method name="GetNetworkInterfaceNameByIndex">
      <arg name="index" type="i" direction="in"/>
      <arg name="name" type="s" direction="out"/>
    </method>
    <method name="GetNetworkInterfaceIndexByName">
      <arg name="name" type="s" direction="in"/>
      <arg name="index" type="i" direction="out"/>
    </method>

    <method name="ResolveHostName">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="name" type="s" direction="in"/>
      <arg name="aprotocol" type="i" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="interface" type="i" direction="out"/>
      <arg name="protocol" type="i" direction="out"/>
      <arg name="name" type="s" direction="out"/>
      <arg name="aprotocol" type="i" direction="out"/>
      <arg name="address" type="s" direction="out"/>
      <arg name="flags" type="u" direction="out"/>
    </method>

    <method name="ResolveAddress">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="address" type="s" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="interface" type="i" direction="out"/>
      <arg name="protocol" type="i" direction="out"/>
      <arg name="aprotocol" type="i" direction="out"/>
      <arg name="address" type="s" direction="out"/>
      <arg name="name" type="s" direction="out"/>
      <arg name="flags" type="u" direction="out"/>
    </method>

    <method name="ResolveService">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="name" type="s" direction="in"/>
      <arg name="type" type="s" direction="in"/>
      <arg name="domain" type="s" direction="in"/>
      <arg name="aprotocol" type="i" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="interface" type="i" direction="out"/>
      <arg name="protocol" type="i" direction="out"/>
      <arg name="name" type="s" direction="out"/>
      <arg name="type" type="s" direction="out"/>
      <arg name="domain" type="s" direction="out"/>
      <arg name="host" type="s" direction="out"/>
      <arg name="aprotocol" type="i" direction="out"/>
      <arg name="address" type="s" direction="out"/>
      <arg name="port" type="q" direction="out"/>
      <arg name="txt" type="aay" direction="out"/>
      <arg name="flags" type="u" direction="out"/>
    </method>

    <method name="EntryGroupNew">
      <arg name="path" type="o" direction="out"/>
    </method>

    <method name="DomainBrowserNew">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="domain" type="s" direction="in"/>
      <arg name="btype" type="i" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="path" type="o" direction="out"/>
    </method>

    <method name="ServiceTypeBrowserNew">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="domain" type="s" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="path" type="o" direction="out"/>
    </method>

    <method name="ServiceBrowserNew">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="type" type="s" direction="in"/>
      <arg name="domain" type="s" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="path" type="o" direction="out"/>
    </method>

    <method name="ServiceResolverNew">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="name" type="s" direction="in"/>
      <arg name="type" type="s" direction="in"/>
      <arg name="domain" type="s" direction="in"/>
      <arg name="aprotocol" type="i" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="path" type="o" direction="out"/>
    </method>

    <method name="HostNameResolverNew">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="name" type="s" direction="in"/>
      <arg name="aprotocol" type="i" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="path" type="o" direction="out"/>
    </method>

    <method name="AddressResolverNew">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="address" type="s" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="path" type="o" direction="out"/>
    </method>

    <method name="RecordBrowserNew">
      <arg name="interface" type="i" direction="in"/>
      <arg name="protocol" type="i" direction="in"/>
      <arg name="name" type="s" direction="in"/>
      <arg name="clazz" type="q" direction="in"/>
      <arg name="type" type="q" direction="in"/>
      <arg name="flags" type="u" direction="in"/>

      <arg name="path" type="o" direction="out"/>
    </method>


  </interface>
</node>
~~~

To make a proxy, run `dbus-cxx-xml2cpp` as follows:

    dbus-cxx-xml2cpp  --proxy --xml=/path/to/org.freedesktop.Avahi.Server.xml

The output will be similar to the following:

~~~{.cpp}

#ifndef NONAME_PROXY_H
#define NONAME_PROXY_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
class NONAME_Proxy
 : public DBus::ObjectProxy {
protected:
NONAME_Proxy(DBus::Connection::pointer conn, std::string dest, std::string path ) : DBus::ObjectProxy( conn, dest, path ){
m_method_Introspect = this->create_method<std::string>("org.freedesktop.DBus.Introspectable","Introspect");
m_method_GetVersionString = this->create_method<std::string>("org.freedesktop.Avahi.Server","GetVersionString");
m_method_GetAPIVersion = this->create_method<uint32_t>("org.freedesktop.Avahi.Server","GetAPIVersion");
m_method_GetHostName = this->create_method<std::string>("org.freedesktop.Avahi.Server","GetHostName");
m_method_SetHostName = this->create_method<void,std::string>("org.freedesktop.Avahi.Server","SetHostName");
m_method_GetHostNameFqdn = this->create_method<std::string>("org.freedesktop.Avahi.Server","GetHostNameFqdn");
m_method_GetDomainName = this->create_method<std::string>("org.freedesktop.Avahi.Server","GetDomainName");
m_method_IsNSSSupportAvailable = this->create_method<bool>("org.freedesktop.Avahi.Server","IsNSSSupportAvailable");
m_method_GetState = this->create_method<int32_t>("org.freedesktop.Avahi.Server","GetState");
m_method_GetLocalServiceCookie = this->create_method<uint32_t>("org.freedesktop.Avahi.Server","GetLocalServiceCookie");
m_method_GetAlternativeHostName = this->create_method<std::string,std::string>("org.freedesktop.Avahi.Server","GetAlternativeHostName");
m_method_GetAlternativeServiceName = this->create_method<std::string,std::string>("org.freedesktop.Avahi.Server","GetAlternativeServiceName");
m_method_GetNetworkInterfaceNameByIndex = this->create_method<std::string,int32_t>("org.freedesktop.Avahi.Server","GetNetworkInterfaceNameByIndex");
m_method_GetNetworkInterfaceIndexByName = this->create_method<int32_t,std::string>("org.freedesktop.Avahi.Server","GetNetworkInterfaceIndexByName");
m_method_ResolveHostName = this->create_method<uint32_t,int32_t,int32_t,std::string,int32_t,uint32_t>("org.freedesktop.Avahi.Server","ResolveHostName");
m_method_ResolveAddress = this->create_method<uint32_t,int32_t,int32_t,std::string,uint32_t>("org.freedesktop.Avahi.Server","ResolveAddress");
m_method_ResolveService = this->create_method<uint32_t,int32_t,int32_t,std::string,std::string,std::string,int32_t,uint32_t>("org.freedesktop.Avahi.Server","ResolveService");
m_method_EntryGroupNew = this->create_method<DBus::Path>("org.freedesktop.Avahi.Server","EntryGroupNew");
m_method_DomainBrowserNew = this->create_method<DBus::Path,int32_t,int32_t,std::string,int32_t,uint32_t>("org.freedesktop.Avahi.Server","DomainBrowserNew");
m_method_ServiceTypeBrowserNew = this->create_method<DBus::Path,int32_t,int32_t,std::string,uint32_t>("org.freedesktop.Avahi.Server","ServiceTypeBrowserNew");
m_method_ServiceBrowserNew = this->create_method<DBus::Path,int32_t,int32_t,std::string,std::string,uint32_t>("org.freedesktop.Avahi.Server","ServiceBrowserNew");
m_method_ServiceResolverNew = this->create_method<DBus::Path,int32_t,int32_t,std::string,std::string,std::string,int32_t,uint32_t>("org.freedesktop.Avahi.Server","ServiceResolverNew");
m_method_HostNameResolverNew = this->create_method<DBus::Path,int32_t,int32_t,std::string,int32_t,uint32_t>("org.freedesktop.Avahi.Server","HostNameResolverNew");
m_method_AddressResolverNew = this->create_method<DBus::Path,int32_t,int32_t,std::string,uint32_t>("org.freedesktop.Avahi.Server","AddressResolverNew");
m_method_RecordBrowserNew = this->create_method<DBus::Path,int32_t,int32_t,std::string,uint16_t,uint16_t,uint32_t>("org.freedesktop.Avahi.Server","RecordBrowserNew");

}
public:
std::shared_ptr<NONAME_Proxy> create(DBus::Connection::pointer conn, std::string dest, std::string path ){
return std::shared_ptr<NONAME_Proxy>( new NONAME_Proxy( conn, dest, path ) );

}
std::string Introspect( ){
return (*m_method_Introspect)();

}
std::string GetVersionString( ){
return (*m_method_GetVersionString)();

}
uint32_t GetAPIVersion( ){
return (*m_method_GetAPIVersion)();

}
std::string GetHostName( ){
return (*m_method_GetHostName)();

}
void SetHostName(std::string name ){
return (*m_method_SetHostName)(name);

}
std::string GetHostNameFqdn( ){
return (*m_method_GetHostNameFqdn)();

}
std::string GetDomainName( ){
return (*m_method_GetDomainName)();

}
bool IsNSSSupportAvailable( ){
return (*m_method_IsNSSSupportAvailable)();

}
int32_t GetState( ){
return (*m_method_GetState)();

}
uint32_t GetLocalServiceCookie( ){
return (*m_method_GetLocalServiceCookie)();

}
std::string GetAlternativeHostName(std::string name ){
return (*m_method_GetAlternativeHostName)(name);

}
std::string GetAlternativeServiceName(std::string name ){
return (*m_method_GetAlternativeServiceName)(name);

}
std::string GetNetworkInterfaceNameByIndex(int32_t index ){
return (*m_method_GetNetworkInterfaceNameByIndex)(index);

}
int32_t GetNetworkInterfaceIndexByName(std::string name ){
return (*m_method_GetNetworkInterfaceIndexByName)(name);

}
uint32_t ResolveHostName(int32_t interface, int32_t protocol, std::string name, int32_t aprotocol, uint32_t flags ){
return (*m_method_ResolveHostName)(interface,protocol,name,aprotocol,flags);

}
uint32_t ResolveAddress(int32_t interface, int32_t protocol, std::string address, uint32_t flags ){
return (*m_method_ResolveAddress)(interface,protocol,address,flags);

}
uint32_t ResolveService(int32_t interface, int32_t protocol, std::string name, std::string type, std::string domain, int32_t aprotocol, uint32_t flags ){
return (*m_method_ResolveService)(interface,protocol,name,type,domain,aprotocol,flags);

}
DBus::Path EntryGroupNew( ){
return (*m_method_EntryGroupNew)();

}
DBus::Path DomainBrowserNew(int32_t interface, int32_t protocol, std::string domain, int32_t btype, uint32_t flags ){
return (*m_method_DomainBrowserNew)(interface,protocol,domain,btype,flags);

}
DBus::Path ServiceTypeBrowserNew(int32_t interface, int32_t protocol, std::string domain, uint32_t flags ){
return (*m_method_ServiceTypeBrowserNew)(interface,protocol,domain,flags);

}
DBus::Path ServiceBrowserNew(int32_t interface, int32_t protocol, std::string type, std::string domain, uint32_t flags ){
return (*m_method_ServiceBrowserNew)(interface,protocol,type,domain,flags);

}
DBus::Path ServiceResolverNew(int32_t interface, int32_t protocol, std::string name, std::string type, std::string domain, int32_t aprotocol, uint32_t flags ){
return (*m_method_ServiceResolverNew)(interface,protocol,name,type,domain,aprotocol,flags);

}
DBus::Path HostNameResolverNew(int32_t interface, int32_t protocol, std::string name, int32_t aprotocol, uint32_t flags ){
return (*m_method_HostNameResolverNew)(interface,protocol,name,aprotocol,flags);

}
DBus::Path AddressResolverNew(int32_t interface, int32_t protocol, std::string address, uint32_t flags ){
return (*m_method_AddressResolverNew)(interface,protocol,address,flags);

}
DBus::Path RecordBrowserNew(int32_t interface, int32_t protocol, std::string name, uint16_t clazz, uint16_t type, uint32_t flags ){
return (*m_method_RecordBrowserNew)(interface,protocol,name,clazz,type,flags);

}
protected:
    DBus::MethodProxy<std::string>::pointer  m_method_Introspect;
    DBus::MethodProxy<std::string>::pointer  m_method_GetVersionString;
    DBus::MethodProxy<uint32_t>::pointer  m_method_GetAPIVersion;
    DBus::MethodProxy<std::string>::pointer  m_method_GetHostName;
    DBus::MethodProxy<void,std::string>::pointer  m_method_SetHostName;
    DBus::MethodProxy<std::string>::pointer  m_method_GetHostNameFqdn;
    DBus::MethodProxy<std::string>::pointer  m_method_GetDomainName;
    DBus::MethodProxy<bool>::pointer  m_method_IsNSSSupportAvailable;
    DBus::MethodProxy<int32_t>::pointer  m_method_GetState;
    DBus::MethodProxy<uint32_t>::pointer  m_method_GetLocalServiceCookie;
    DBus::MethodProxy<std::string,std::string>::pointer  m_method_GetAlternativeHostName;
    DBus::MethodProxy<std::string,std::string>::pointer  m_method_GetAlternativeServiceName;
    DBus::MethodProxy<std::string,int32_t>::pointer  m_method_GetNetworkInterfaceNameByIndex;
    DBus::MethodProxy<int32_t,std::string>::pointer  m_method_GetNetworkInterfaceIndexByName;
    DBus::MethodProxy<uint32_t,int32_t,int32_t,std::string,int32_t,uint32_t>::pointer  m_method_ResolveHostName;
    DBus::MethodProxy<uint32_t,int32_t,int32_t,std::string,uint32_t>::pointer  m_method_ResolveAddress;
    DBus::MethodProxy<uint32_t,int32_t,int32_t,std::string,std::string,std::string,int32_t,uint32_t>::pointer  m_method_ResolveService;
    DBus::MethodProxy<DBus::Path>::pointer  m_method_EntryGroupNew;
    DBus::MethodProxy<DBus::Path,int32_t,int32_t,std::string,int32_t,uint32_t>::pointer  m_method_DomainBrowserNew;
    DBus::MethodProxy<DBus::Path,int32_t,int32_t,std::string,uint32_t>::pointer  m_method_ServiceTypeBrowserNew;
    DBus::MethodProxy<DBus::Path,int32_t,int32_t,std::string,std::string,uint32_t>::pointer  m_method_ServiceBrowserNew;
    DBus::MethodProxy<DBus::Path,int32_t,int32_t,std::string,std::string,std::string,int32_t,uint32_t>::pointer  m_method_ServiceResolverNew;
    DBus::MethodProxy<DBus::Path,int32_t,int32_t,std::string,int32_t,uint32_t>::pointer  m_method_HostNameResolverNew;
    DBus::MethodProxy<DBus::Path,int32_t,int32_t,std::string,uint32_t>::pointer  m_method_AddressResolverNew;
    DBus::MethodProxy<DBus::Path,int32_t,int32_t,std::string,uint16_t,uint16_t,uint32_t>::pointer  m_method_RecordBrowserNew;
};
#endif /* NONAME_PROXY_H */
~~~

As you can see, while we do have methods and they are named appropriately, the class name is `NONAME`
and not very useful at that point.  So, we will modify the XML with a few attributes, so that our
generated class has a better name(Full output omitted for brevity):

~~~{.xml}
<node cppname="Avahi">

 <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="data" type="s" direction="out"/>
    </method>
  </interface>

  <interface name="org.freedesktop.Avahi.Server">

    <method name="GetVersionString">
      <arg name="version" type="s" direction="out"/>
    </method>

    <method name="GetAPIVersion">
      <arg name="version" type="u" direction="out"/>
    </method>

    ...

  </interface>
</node>
~~~

Now the code looks like the following:

~~~{.cpp}
#ifndef AVAHIPROXY_H
#define AVAHIPROXY_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
class AvahiProxy
 : public DBus::ObjectProxy {
protected:
AvahiProxy(DBus::Connection::pointer conn, std::string dest, std::string path ) : DBus::ObjectProxy( conn, dest, path ){
m_method_Introspect = this->create_method<std::string>("org.freedesktop.DBus.Introspectable","Introspect");
m_method_GetVersionString = this->create_method<std::string>("org.freedesktop.Avahi.Server","GetVersionString");
m_method_GetAPIVersion = this->create_method<uint32_t>("org.freedesktop.Avahi.Server","GetAPIVersion");

}
public:
std::shared_ptr<AvahiProxy> create(DBus::Connection::pointer conn, std::string dest, std::string path ){
return std::shared_ptr<AvahiProxy>( new AvahiProxy( conn, dest, path ) );

}
std::string Introspect( ){
return (*m_method_Introspect)();

}
std::string GetVersionString( ){
return (*m_method_GetVersionString)();

}
uint32_t GetAPIVersion( ){
return (*m_method_GetAPIVersion)();

}
protected:
    DBus::MethodProxy<std::string>::pointer  m_method_Introspect;
    DBus::MethodProxy<std::string>::pointer  m_method_GetVersionString;
    DBus::MethodProxy<uint32_t>::pointer  m_method_GetAPIVersion;
};
#endif /* AVAHIPROXY_H */
~~~

If we want to namespace the class, we can do that as well:

~~~{.xml}
<node cppname="Avahi" gen-namespace="AvahiNamespace">

 <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="data" type="s" direction="out"/>
    </method>
  </interface>

  <interface name="org.freedesktop.Avahi.Server">

    <method name="GetVersionString">
      <arg name="version" type="s" direction="out"/>
    </method>

    <method name="GetAPIVersion">
      <arg name="version" type="u" direction="out"/>
    </method>

    ...

  </interface>
</node>
~~~

This results in the following:

~~~~~{.xml}

#ifndef AVAHIPROXY_H
#define AVAHIPROXY_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
namespace AvahiNamespace {
class AvahiProxy
 : public DBus::ObjectProxy {
protected:
AvahiProxy(DBus::Connection::pointer conn, std::string dest, std::string path ) : DBus::ObjectProxy( conn, dest, path ){
m_method_Introspect = this->create_method<std::string>("org.freedesktop.DBus.Introspectable","Introspect");
m_method_GetVersionString = this->create_method<std::string>("org.freedesktop.Avahi.Server","GetVersionString");
m_method_GetAPIVersion = this->create_method<uint32_t>("org.freedesktop.Avahi.Server","GetAPIVersion");

}
public:
std::shared_ptr<AvahiProxy> create(DBus::Connection::pointer conn, std::string dest, std::string path ){
return std::shared_ptr<AvahiProxy>( new AvahiProxy( conn, dest, path ) );

}
std::string Introspect( ){
return (*m_method_Introspect)();

}
std::string GetVersionString( ){
return (*m_method_GetVersionString)();

}
uint32_t GetAPIVersion( ){
return (*m_method_GetAPIVersion)();

}
protected:
    DBus::MethodProxy<std::string>::pointer  m_method_Introspect;
    DBus::MethodProxy<std::string>::pointer  m_method_GetVersionString;
    DBus::MethodProxy<uint32_t>::pointer  m_method_GetAPIVersion;
};
} /* namespace AvahiNamespace */
#endif /* AVAHIPROXY_H */
~~~~~

## Adapter Example

If we instead want to implement a class from an already-existing introspection document, we create an adapter
using the following command:

    dbus-cxx-xml2cpp  --adapter --xml=/path/to/org.freedesktop.Avahi.Server.xml -f

Note: Because this creates two classes(an adapter and an adaptee), we must send it to a file.

The generated files look like this:

AvahiAdaptee.h:
~~~{.cpp}
#ifndef AVAHIADAPTEE_H
#define AVAHIADAPTEE_H

class AvahiAdaptee
{
public:
virtual std::string Introspect( ) = 0;
virtual std::string GetVersionString( ) = 0;
virtual uint32_t GetAPIVersion( ) = 0;
};
#endif /* AVAHIADAPTEE_H */
~~~

AvahiAdapter.h:
~~~{.cpp}
#ifndef AVAHIADAPTER_H
#define AVAHIADAPTER_H

#include <dbus-cxx.h>
#include <memory>
#include <stdint.h>
#include <string>
#include "AvahiAdaptee.h"
class AvahiAdapter
 : public DBus::Object {
protected:
AvahiAdapter(AvahiAdaptee* adaptee, std::string path );
public:
    static std::shared_ptr<AvahiAdapter> create(AvahiAdaptee* adaptee, std::string path );
};
#endif /* AVAHIADAPTER_H */
~~~

AvahiAdapter.cpp:
~~~{.cpp}
#include "AvahiAdapter.h"

AvahiAdapter::AvahiAdapter(AvahiAdaptee* adaptee, std::string path ) : DBus::Object( path ){
DBus::MethodBase::pointer temp_method;
temp_method = this->create_method<std::string>( "org.freedesktop.DBus.Introspectable", "Introspect",sigc::mem_fun( adaptee, &AvahiAdaptee::Introspect ) );
temp_method = this->create_method<std::string>( "org.freedesktop.Avahi.Server", "GetVersionString",sigc::mem_fun( adaptee, &AvahiAdaptee::GetVersionString ) );
temp_method = this->create_method<uint32_t>( "org.freedesktop.Avahi.Server", "GetAPIVersion",sigc::mem_fun( adaptee, &AvahiAdaptee::GetAPIVersion ) );

}
std::shared_ptr<AvahiAdapter> AvahiAdapter::create(AvahiAdaptee* adaptee, std::string path ){
return std::shared_ptr<AvahiAdapter>( new AvahiAdapter( adaptee, path ) );

}
~~~

Now we simply includ our `AvahiAdaptee.h` and implement the methods as appropriate:

~~~{.cpp}
#include "AvahiAdapter.h"

class MyAdaptee : public AvahiAdaptee {
public:
MyAdaptee(){}
virtual std::string Introspect( ){
    return "introspection-document-here";
}
virtual std::string GetVersionString( ){
    return "1.0";
}
virtual uint32_t GetAPIVersion( ){
    return 1;
}
};

int main(){
    MyAdaptee adaptee;

    std::shared_ptr<AvahiAdapter> adapter = AvahiAdapter::create( &adaptee, "path" );

    /* Eventually, we export the object using adapter->register_with_connection( DBus::Connection ); */
}
~~~


[avahi-server]:https://github.com/lathiat/avahi/blob/b902ea8f8615605e12ed6a3b64e7a11b71a759eb/avahi-daemon/org.freedesktop.Avahi.Server.xml
