/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef __DBUSXX_DISPATCHER_H
#define __DBUSXX_DISPATCHER_H

#include <map>
#include <list>
#include <set>

#include <sys/select.h>

#include <dbus/dbus.h>
#include <dbus-cxx/connection.h>
#include <dbus-cxx/watch.h>
#include <dbus-cxx/timeout.h>

namespace DBus
{

  /**
   * Handles multi-threaded dispatching of one or more connections.
   *
   * This class provides a way to handle multiple connections with one set of
   * dispatching threads.
   *
   * This dispatcher creates two threads, one to watch I/O file descriptors
   * for activity and the other to handle message dispatching.
   *
   * @ingroup core
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Dispatcher
  {
    public:
      
      typedef DBusCxxPointer<Dispatcher> pointer;

      typedef DBusCxxPointer<const Dispatcher> const_pointer;

      Dispatcher(bool is_running=true);

      static pointer create( bool is_running=true );

      virtual ~Dispatcher();
      
      /** @name Managing Connections */
      //@{

      Connection::pointer create_connection( DBusConnection* cobj = NULL, bool is_private=false );

      Connection::pointer create_connection( BusType type, bool is_private=false );

      Connection::pointer create_connection( const Connection& other );

      bool add_connection( Connection::pointer connection );

      //@}

      bool start();
      
      bool stop();
      
      bool is_running();

      const struct timeval& responsiveness();
      
      void set_responsiveness( const struct timeval& r );
      
      void set_responsiveness( time_t sec, suseconds_t usec );
      
    protected:
      
      typedef std::list<Connection::pointer> Connections;
      Connections m_connections;
      
      bool m_running;
      
      pthread_t m_dispatch_thread;
      pthread_t m_watch_thread;

      pthread_mutex_t m_mutex_read_watches;
      std::map<int, Watch::pointer> m_read_watches;
      std::set<int> m_enabled_read_fds;
      fd_set m_read_fd_set;
      int m_maximum_read_fd;

      void build_read_fd_set();
      
      pthread_mutex_t m_mutex_write_watches;
      std::map<int, Watch::pointer> m_write_watches;
      std::set<int> m_enabled_write_fds;
      fd_set m_write_fd_set;
      int m_maximum_write_fd;

      void build_write_fd_set();
      
      fd_set m_exception_fd_set;
      
      pthread_mutex_t m_mutex_exception_fd_set;
      
      struct timeval m_responsiveness;

      /**
       * This is the maximum number of dispatches that will occur for a
       * connection in one iteration of the dispatch thread.
       *
       * If set to 0, a particular connection will continue to dispatch
       * as long as its status remains DISPATCH_DATA_REMAINS.
       */
      unsigned int m_dispatch_loop_limit;
      
      bool m_initiate_processing;      
      pthread_cond_t m_cond_initiate_processing;
      pthread_mutex_t m_mutex_initiate_processing;
      
      virtual void dispatch_thread_main();
      
      void watch_thread_main();
      
      /**
       * Since pthread_create cannot take a class method as the
       * thread-main parameter this function is called with an
       * argument pointing to the Dispatcher instance to start
       * in the thread.
       */
      static void* proxy_dispatch_thread_main(void*);
      
      /**
       * Since pthread_create cannot take a class method as the
       * thread-main parameter this function is called with an
       * argument pointing to the Dispatcher instance to start
       * in the thread.
       */
      static void* proxy_watch_thread_main(void*);
      
      bool on_add_watch(Watch::pointer);
      
      bool on_remove_watch(Watch::pointer);
      
      bool on_watch_toggled(Watch::pointer);
      
      bool on_add_timeout(Timeout::pointer);
      
      bool on_remove_timeout(Timeout::pointer);
      
      bool on_timeout_toggled(Timeout::pointer);
      
      void on_wakeup_main(Connection::pointer);
      
      void on_dispatch_status_changed(DispatchStatus, Connection::pointer);
  };

}

#endif

  /**
   * \mainpage dbus-cxx Library
   *
   * <br><hr>
   *
   * \par About
   * dbus-cxx is a C++ wrapper for dbus. It exposes the C API to allow direct
   * manipulation and relies on sigc++ to provide an OO interface. Also included
   * is dbus-cxx-xml2cpp to generate proxy and adapter interfaces from DBus XML
   * introspection-like documents.
   * 
   * \htmlonly
   * <script type="text/javascript" src="http://www.ohloh.net/p/330958/widgets/project_users.js?style=red"></script><br>
   * <script type="text/javascript" src="http://www.ohloh.net/p/330958/widgets/project_partner_badge.js"></script>
   * \endhtmlonly
   *
   * <br><hr>
   * 
   * \par dbus-cxx Sourceforge Project Page
   * The dbus-cxx sourceforge project page can be found here:
   *
   * \par
   * <a href="http://sourceforge.net/projects/dbus-cxx">http://sourceforge.net/projects/dbus-cxx</a>
   *
   * \par
   * <a href="http://sourceforge.net/news/?group_id=259994">News</a>
   *
   * <br><hr>
   *
   * <h2>Download dbus-cxx packages or get the source code</h2>
   *
   * 
   * \htmlonly <img src="fedora-logo-tiny.png" alt="Fedora"/> \endhtmlonly
   * \par
   * dbus-cxx is available in Fedora 9 and higher.
   *
   * \par
   * Fedora package description
   * - \b dbus-cxx - Libraries needed to run applications
   * - \b dbus-cxx-devel - Headers and libraries for developing applications with dbus-cxx
   * - \b dbus-cxx-doc - Developer's documentation including devhelp docs
   * - \b dbus-cxx-tools - Development tools, et. al. such as \c dbus-cxx-xml2cpp
   * - \b dbus-cxx-glibmm - Libraries need to run applications that integrate dbus-cxx into glibmm
   * - \b dbus-cxx-glibmm-devel - Headers and libraries for developing applications that will integrate dbus-cxx into glibmm
   *
   * \htmlonly <img src="ubuntu-logo-tiny.png" alt="Ubuntu"/> \endhtmlonly
   * \par
   * dbus-cxx is available for Ubuntu via launchpad.
   *
   * \par 
   * Installing from Launchpad
   * - Here is a useful page on setting up a Launchpad PPA in Ubuntu: 
   *   <a href="http://launchpad.net/+help/soyuz/ppa-sources-list.html">http://launchpad.net/+help/soyuz/ppa-sources-list.html</a>
   * - Here is the Launchpad PPA page: 
   *   <a href="http://launchpad.net/~rvinyard/+archive/ppa">https://launchpad.net/~rvinyard/+archive/ppa</a>
   * - If you just want the basic PPA info:
   *   \code 
   *   deb http://ppa.launchpad.net/rvinyard/ppa/ubuntu jaunty main 
   *   deb-src http://ppa.launchpad.net/rvinyard/ppa/ubuntu jaunty main
   *   \endcode
   * 
   * \par
   * Ubuntu package description
   * - \b dbus-cxx0 - Libraries needed to run applications
   * - \b dbus-cxx-dev - Headers and libraries for developing applications with dbus-cxx
   * - \b dbus-cxx-doc - Developer's documentation including devhelp docs
   * - \b dbus-cxx-tools - Development tools, et. al. such as \c dbus-cxx-xml2cpp
   * - \b dbus-cxx-glibmm0 - Libraries need to run applications that integrate dbus-cxx into glibmm
   * - \b dbus-cxx-glibmm-dev - Headers and libraries for developing applications that will integrate dbus-cxx into glibmm
   *
   * \htmlonly <img src="sourcecode-small.png" alt=""/> \endhtmlonly
   * \par Source Code
   * \htmlonly <img src="download-small.png" alt=""/> \endhtmlonly
   * \b Releases - (.bz2, .gz, .zip) can be found <a href="http://sourceforge.net/project/showfiles.php?group_id=259994">here</a>
   *
   * \par
   * \b Subversion \b Repository
   * - You can browse the subversion repository at this url:
   *   - <a href="http://dbus-cxx.svn.sourceforge.net/viewvc/dbus-cxx/trunk/dbus-cxx">http://dbus-cxx.svn.sourceforge.net/viewvc/dbus-cxx/trunk/dbus-cxx</a>
   * - You can also check out a copy of the repository with this command:
   *   - \verbatim svn co https://dbus-cxx.svn.sourceforge.net/svnroot/dbus-cxx/trunk/dbus-cxx \endverbatim
   *
   * <br><hr>
   *
   * \par Dependencies... and where to get them
   * \b dbus: <a href="http://dbus.freedesktop.org">http://dbus.freedesktop.org</a>
   *
   * <br><hr>
   *
   * \htmlonly <img src="documents-small.png" alt=""/> \endhtmlonly
   * \par Documentation, Tutorials, Guides, Quick Start, et. al.
   *
   * \par dbus-cxx-xml2cpp Reference
   * Documentation on using \ref dbus-cxx-xml2cpp to convert DBus XML introspection
   * documents into C++ proxies and adapters.
   * 
   * \par API Reference
   * The API documentation (including the pages you are reading now) have been generated
   * with Doxygen.
   *
   * \par devhelp
   * The built documentation also includes a devhelp index. The index is installed
   * automatically in the dbus-cxx-doc RPM and can be browsed simply by starting
   * devhelp.
   *
   * \par
   * The most current documentation for dbus-cxx is available online here:
   * - dbus-cxx - <a href="http://dbus-cxx.sourceforge.net">http://dbus-cxx.sourceforge.net</a>
   *
   * \par Key sections within the API reference
   * - Namespaces
   *   - DBus
   * - Detailed class documentation in the <a href="hierarchy.html">Class Hierarchy</a>
   * - <a href="annotated.html">Class List</a>
   * - <a href="examples.html">Examples</a>
   *
   * <br><hr>
   *
   * \htmlonly <img src="envelope-small.png" alt=""/> \endhtmlonly
   * \par Mailing Lists
   * - Users List (for those using dbus-cxx in their applications)
   *   - <a href="http://sourceforge.net/mailarchive/forum.php?forum_name=dbus-cxx-users">Archives</a>
   *   - <a href="http://lists.sourceforge.net/mailman/listinfo/dbus-cxx-users">Subscribe/Unsubscribe</a>
   * - Development List (for discussion related to the development of dbus-cxx itself)
          *   - <a href="http://sourceforge.net/mailarchive/forum.php?forum_name=dbus-cxx-devel">Archives</a>
          *   - <a href="http://lists.sourceforge.net/mailman/listinfo/dbus-cxx-devel">Subscribe/Unsubscribe</a>
   *
   * <br><hr>
   *
   * \par Sourceforge Forums
   * Although mailing lists are preferred, the sourceforge forums are available for discussion as well.
   * - <a href="http://sourceforge.net/forum/forum.php?forum_id=943815">Help forum</a> (for those using dbus-cxx in their applications)
   * - <a href="http://sourceforge.net/forum/forum.php?forum_id=943816">Developers forum</a> (for discussion related to the development of dbus-cxx itself)
   * 
   * <br><hr>
   *
   * \par Bugs
   * \htmlonly <img src="bugs-small.png" alt=""/> \endhtmlonly
   * <a href="http://sourceforge.net/tracker/?atid=1127519&group_id=259994&func=browse">View reported bugs in the bug tracker</a>
   *
   * \par
   * \htmlonly <img src="bug-small.png" alt=""/> \endhtmlonly
   * <a href="http://sourceforge.net/tracker/?atid=1127519&group_id=259994&func=add">Submit a bug report here</a>
   *
   * <br><hr>
   *
   * \par Patches
   * Patches are always welcome!!!
   *
   * \par
   * <a href="http://sourceforge.net/tracker/?atid=1127520&group_id=259994&func=browse">View submitted patches</a>
   * 
   * \par
   * <a href="http://sourceforge.net/tracker/?atid=1127520&group_id=259994&func=add">Submit patches here</a>
   *
   * <br><hr>
   *
   * \par Feature Requests
   *
   * \par
   * <a href="http://sourceforge.net/tracker/?atid=1127521&group_id=259994&func=add">Submit feature requests here</a>
   *
   * <br><hr>
   *
   * \par License
   * dbus-cxx is released under the \htmlonly <a href="http://www.gnu.org/licenses/gpl-3.0-standalone.html"><img src="gplv3.png" alt="GPLv3"/></a> \endhtmlonly
   *
   */

  /**
   * \page xml2cpp dbus-cxx-xml2cpp
   *
   * \par Summary
   * \c dbus-cxx-xml2cpp generates proxy and adapter interfaces from modified DBus
   * XML introspection documents.
   *
   * \par Commands
   * - \c -x \c --xml : The xml file to process
   * - \c -p \c --prefix : A prefix to prepend to all output files
   * - \c -f \c --file : Send output to files with naming <i>class</i>_proxy.h and <i>class</i>_adapter.h
   * - \c --proxy : Generate a proxy interface
   * - \c --adapter : Generate an adapter interface
   * - \c --verbose : Generate verbose processing info
   *
   * \par Extended DBus XML attributes
   * \c dbus-cxx-xml2cpp in some cases needs information above and beyond that provided
   * in a dbus introspection document. This information can be supplied by adding
   * additional attributes to key nodes.
   *
   * \par
   * The extended attributes per XML element are:
   * - \b node
   *   - \b cppname - The base name of the c++ class adapter and proxy
   *   - \b gen-namespace - The namespace the c++ adapter and proxy will be generated into
   *   - \b orig-namespace - The namespace of the object being adapted
   *   - \b dest - Used to create a default value for the \c dest parameter of a proxy
   *   - \b path - Used to create a default value for the \c path parameter of both proxies and adapters
   *   - \b cppinclude - A free form string that will follow a \c #include line in the adapter
   *   - \b file-prefix - A per-node filename prefix prepended to the class name on output
   * - \b interface
   *   - \b ignored - If set to "1" this interface will not be built for the proxy or adapter
   * - \b signal
   *   - \b accessor - The accessor method used to obtain the adaptee's signal
   *   - \b ignored - If set to "1" this signal will not be built for the proxy or adapter
   * - \b method
   *   - \b cppname - Renames the DBus method to the provided name in the proxy and adapter
   *   - \b const - Makes the proxy method a const method (useful when subclassing virtual methods)
   *   - \b virtual - Prefixes the method with keyword virtual (useful for improving readability of virtual methods)
   * - \b arg
   *   - \b cpptype - Defines the C++ type to be used when creating proxies/adapters for this argument. Useful for wrapping enumerations.
   */
