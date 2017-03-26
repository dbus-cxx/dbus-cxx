Summary:          C++ bindings for the DBus library
Name:             dbus-cxx
Version:          PACKAGE_VERSION
Release:          1%{?dist}
License:          GPLv3
URL:              http://dbus-cxx.sourceforge.net/
Group:            System Environment/Libraries
Source0:          http://downloads.sourceforge.net/dbus-cxx/%{name}-%{version}.tar.bz2
Source1:          http://downloads.sourceforge.net/dbus-cxx/%{name}-%{version}-doc.tar.bz2
BuildRoot:        %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:    dbus-devel >= 1.1
BuildRequires:    libsigc++20-devel >= 2.0.0
BuildRequires:    glibmm24-devel >= 2.4.0
BuildRequires:    pkgconfig
BuildRequires:    m4
BuildRequires:    expat-devel
%if 0%{?fedora} > 8 || 0%{?rhel} > 5
BuildRequires:    popt-devel
%else
BuildRequires:    popt
%endif

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

%package          doc
Summary:          Developer's documentation for the dbus-cxx library
Group:            Documentation
Requires:         gtk-doc
Requires:         devhelp
%if 0%{?fedora} > 9 || 0%{?rhel} > 5
BuildArch:        noarch
%endif

%description      doc
This package contains developer's documentation for the dbus-cxx
library. dbus-cxx provides C++ based bindings for the C based
dbus library.

The documentation can be viewed either through the devhelp
documentation browser or through a web browser. 

If using a web browser the documentation is installed in the gtk-doc
hierarchy and can be found at /usr/share/gtk-doc/html/dbus-cxx-API_VERSION

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

%package          glibmm
Summary:          Adds support to dbus-cxx (C++ bindings for dbus) for glibmm integration
Group:            System Environment/Libraries
Requires:         dbus-cxx = %{version}-%{release}

%description      glibmm
Adds support to dbus-cxx (C++ bindings for dbus) for glibmm integration.

%package          glibmm-devel
Summary:          Headers for developing programs that will use dbus-cxx-glibmm
Group:            Development/Libraries
Requires:         dbus-cxx-glibmm = %{version}-%{release}
Requires:         dbus-cxx-devel = %{version}-%{release}
Requires:         glibmm24-devel >= 2.4

%description      glibmm-devel
dbus-cxx provides C++ bindings for the dbus library. dbus-cxx-glibmm
provides dbus-cxx a means of integrating with glibmm.

Since gtkmm relies on glibmm for main-loop operations this library
can also be used to integrate dbus-cxx with gtkmm applications.

This package contains the libraries and header files needed for
developing dbus-cxx-glibmm applications.

%prep
%setup -q -b 1

%build
%configure --enable-static=no --enable-tools --enable-glibmm
%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} DESTDIR=%{buildroot} INSTALL="%{__install} -p" install
find %{buildroot} -type f -name "*.la" -exec rm -f {} ';'

%{__mkdir} -p %{buildroot}%{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/reference/html/
%{__install} -p --mode=0664 -t %{buildroot}%{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/reference/html/ doc/reference/html/*
%{__install} -p --mode=0664 -t %{buildroot}%{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/ doc/dbus-cxx-API_VERSION.devhelp

%clean
%{__rm} -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post glibmm -p /sbin/ldconfig

%postun glibmm -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libdbus-cxx.so.*
%doc AUTHORS COPYING

%files devel
%defattr(-,root,root,-)
%{_libdir}/libdbus-cxx.so
%{_libdir}/pkgconfig/dbus-cxx-PKGCONFIG_VERSION.pc
%dir %{_includedir}/dbus-cxx-PACKAGE_RELEASE
%{_includedir}/dbus-cxx-PACKAGE_RELEASE/dbus-cxx.h
%{_includedir}/dbus-cxx-PACKAGE_RELEASE/dbus-cxx/
%doc ChangeLog

%files doc
%defattr(-,root,root,-)
%doc %{_datadir}/gtk-doc/html/dbus-cxx-API_VERSION/

%files tools
%defattr(-,root,root,-)
%{_bindir}/dbus-cxx-xml2cpp
%{_bindir}/dbus-cxx-introspect

%files glibmm
%defattr(-,root,root,-)
%{_libdir}/libdbus-cxx-glibmm.so.*

%files glibmm-devel
%defattr(-,root,root,-)
%{_libdir}/libdbus-cxx-glibmm.so
%{_libdir}/pkgconfig/dbus-cxx-glibmm-PKGCONFIG_VERSION.pc
%{_includedir}/dbus-cxx-PACKAGE_RELEASE/dbus-cxx-glibmm.h
%{_includedir}/dbus-cxx-PACKAGE_RELEASE/dbus-cxx-glibmm/

%changelog
* Thu Mar 18 2010 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.7.0-1
- New release

* Fri Jan 22 2010 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.6.0-1
- New release

* Mon Jan  4 2010 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.5.1-1
- New release

* Tue Sep 29 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.5.0-1
- New release

* Mon Sep 21 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.4.3-1
- New release

* Mon Aug 24 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.4.2-1
- New release
- Made docs noarch

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 0.4.1-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Mon Jul  6 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.4.1-1
- New release

* Thu Jun 18 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.4.0-1
- New release
- Added glibmm subpackage

* Wed Jun 17 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.3.4-1
- New release

* Tue Jun 16 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.3.3-1
- New release

* Mon Jun 15 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.3.2-1
- New release

* Fri Jun 12 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.3.1-1
- New release

* Mon Jun  8 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.3.0-1
- New release

* Thu Jun  4 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.2.0-1
- New release

* Fri May 29 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.1.2-1
- New release

* Thu May 14 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.1.1-1
- New release
- Added doc subpackage

* Tue May 12 2009 Rick L Vinyard Jr <rvinyard@cs.nmsu.edu> - 0.1.0-1
- Initial release


