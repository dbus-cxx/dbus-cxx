Summary:          C++ bindings for the DBus library
Name:             dbus-cxx
Version:          PACKAGE_VERSION
Release:          1%{?dist}
License:          GPLv3
URL:              http://dbus-cxx.sourceforge.net
Group:            System Environment/Libraries
Source:           http://downloads.sourceforge.net/dbus-cxx/dbus-cxx-%{version}.tar.bz2
BuildRoot:        %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:    dbus-devel >= 1.2
BuildRequires:    libsigc++20-devel >= 2.0.0
BuildRequires:    pkgconfig
BuildRequires:    m4
BuildRequires:    expat-devel

%description
C++ wrapper for the DBus C reference implementation.

%package          devel
Summary:          C++ development headers for dbus-cxx
Group:            Development/Libraries
Requires:         %{name} = %{version}-%{release}
Requires:         dbus-devel >= 1.2
Requires:         libsigc++20-devel >= 2.0.0
Requires:         expat-devel
Requires:         pkgconfig

%description      devel
This package contains the C++ libraries and header files needed for
developing dbus-cxx applications.

dbus-cxx provides C++ bindings for the DBus library.

%package          tools
Summary:          Tools to support dbus-cxx application development
Group:            Development/Tools
Requires:         %{name} = %{version}-%{release}

%description      tools
This package contains tools to assist with dbus-cxx application development.

* dbus-cxx-xml2cpp:
  This application can be used to create external class proxies and adaptors
  from a DBus Introspection-like xml specification.

* dbus-cxx-introspect:
  Command line utility to simplify introspection

%prep
%setup -q

%build
%configure --enable-static=no --enable-tools
%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} DESTDIR=%{buildroot} INSTALL="%{__install} -p" install
find %{buildroot} -type f -name "*.la" -exec rm -f {} ';'

# Fix documentation installation and put papyrus-gtkmm docs under gtk-doc
%{__mkdir} -p %{buildroot}%{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/reference/html/
%{__install} -p --mode=0664 -t %{buildroot}%{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/reference/html/ doc/reference/html/*
%{__install} -p --mode=0664 -t %{buildroot}%{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/ doc/dbus-cxx-API_VERSION.devhelp

%clean
%{__rm} -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libdbus-cxx.so.*
%doc AUTHORS COPYING

%files devel
%defattr(-,root,root,-)
%{_libdir}/libdbus-cxx.so
%{_libdir}/pkgconfig/dbus-cxx-PKGCONFIG_VERSION.pc
%{_includedir}/dbus-cxx-PACKAGE_RELEASE/
%doc %{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/
%doc ChangeLog

%files tools
%defattr(-,root,root,-)
%{_bindir}/dbus-cxx-xml2cpp
%{_bindir}/dbus-cxx-introspect

%changelog
* Tue May 12 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.1.0-1
- Initial release


