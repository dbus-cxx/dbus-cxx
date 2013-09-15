/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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
#include <map>
#include <list>
#include <set>

#include <sys/select.h>

#include <dbus/dbus.h>
#include <dbus-cxx/connection.h>
#include <dbus-cxx/watch.h>
#include <dbus-cxx/timeout.h>

#ifndef DBUSCXX_DISPATCHER_H
#define DBUSCXX_DISPATCHER_H

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
    protected:

      Dispatcher(bool is_running=true);

    public:
      
      typedef DBusCxxPointer<Dispatcher> pointer;

      typedef DBusCxxPointer<const Dispatcher> const_pointer;

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
   * If you want to jump into the code quickly check out the quick start guide:
   * \ref quick_start
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
   * \b Releases - (.bz2, .gz, .zip) can be found <a href="http://sourceforge.net/project/dbus-cxx/files">here</a>
   *
   * \par
   * \b Subversion \b Repository
   * - You can browse the subversion repository at this url:
   *   - <a href="http://sourceforge.net/p/dbus-cxx/code/HEAD/tree/">http://sourceforge.net/p/dbus-cxx/code/HEAD/tree/</a>
   * - You can also check out a copy of the repository with this command:
   *   - \verbatim svn checkout svn://svn.code.sf.net/p/dbus-cxx/code/trunk dbus-cxx-code \endverbatim
   *
   * <br><hr>
   *
   * \par Dependencies... and where to get them
   * \b dbus: <a href="http://dbus.freedesktop.org">http://dbus.freedesktop.org</a> - Should be installed by default (runtime dependency) <br/>
   * \b libsigc++ <a href="http://libsigc.sourceforge.net/">http://libsigc.sourceforge.net/</a> (compile dependency)
   * See the README file for more information on compile-time dependencies
   *
   * <br><hr>
   *
   * \htmlonly <img src="documents-small.png" alt=""/> \endhtmlonly
   * \par Documentation, Tutorials, Guides, Quick Start, et. al.
   *
   * \par Quick Start Guide
   * If you want to jump into the code quickly check out the \ref quick_start guide.
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
   * \par Bugs
   *
   * \par 
   * Please submit all bugs to the mailing list
   *
   * <br><hr>
   *
   * \par Patches
   * Patches are always welcome!!!
   *
   * \par 
   * Please submit all patches to the mailing list
   *
   * <br><hr>
   *
   * \par Feature Requests
   *
   * \par
   * Please submit all feature requests to the mailing list
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

/**
 * \page quick_start Quick Start Guide to dbus-cxx
 *
 * This is intended to be a very succinct introduction to dbus-cxx, and is \b not
 * intended as a general tutorial.
 *
 * \section quick_start_sections Sections:
 *
 *  -# \ref quick_start_pkgconfig
 *  -# \ref quick_start_initial_concepts
 *  -# \ref quick_start_example_0
 *     -# \ref quick_start_server_0
 *        -# \ref quick_start_server_0_code
 *        -# \ref quick_start_server_0_discussion
 *     -# \ref quick_start_client_0
 *        -# \ref quick_start_client_0_code
 *        -# \ref quick_start_client_0_discussion
 */

/**
 * \page quick_start_pkgconfig Using pkgconfig
 *
 * Although this is a bit more in-depth than I planned for a quick start tutorial 
 * I thought I'd include this little blurb on using pkgconfig to help with setting your application to use dbus-cxx.
 * 
 *  dbus-cxx is distributed with two (at the current time) pkgconfig .pc files named dbus-cxx-1.0.pc and dbus-cxx-glibmm-1.0.pc. 
 *  If you installed the Fedora dbus-cxx-devel or dbus-cxx-glibmm-devel packages you'll find these files installed in a directory 
 *  named /pkgconfig under in your architecture's library directory (probably /usr/lib/pkgconfig or /usr/lib64/pkgconfig ).
 *
 * \section quick_start_pkgconfig_autotools Using pkg-config with autotools
 * If you are using autotools,
 *  modify configure.ac (or configure.in ) with the following lines:
 * \code  
 * PKG_CHECK_MODULES(PROJECT_DBUSCXX,[dbus-cxx-1.0 >= 0.7.0])
 * AC_SUBST(PROJECT_DBUSCXX_LIBS)
 * AC_SUBST(PROJECT_DBUSCXX_CFLAGS)
 * \endcode
 *
 * You can then use the symbols PROJECT_DBUSCXX_LIBS and PROJECT_DBUSCXX_CFLAGS in your Makefile.am files.
 * For example, if you have a application named fooapp your Makefile.am might look like this:
 * \code  
 * INCLUDES = $(PROJECT_DBUSCXX_CFLAGS)
 * bin_PROGRAMS = fooapp
 * fooapp_SOURCES = fooapp.cpp
 * fooapp_LDADD = $(PROJECT_DBUSCXX_LIBS)
 * \endcode
 *
 * \section quick_start_pkgconfig_qt Using pkg-config with QT
 * Because of QTs signal/slot mechanism, dbus-cxx will not work directly with QT.  There are a few things that need to be
 * modified in the .pro file of your project.  Add/change the following lines:
 * \code 
 * CONFIG += no_keywords
 * unix:CONFIG += link_pkgconfig
 * unix:PKGCONFIG += dbus-cxx-1.0
 * \endcode
 * At any point in your QT program, you need to use QT signals or slots, use the macros Q_SIGNALS or Q_SLOTS to define 
 * your signals/slots
 *
 * <b>Continue On:</b> \ref quick_start_initial_concepts
 *
 * <b>Go Back:</b> \ref quick_start
 */

/**
 * \page quick_start_initial_concepts Initial Concepts
 * 
 * \section quick_start_concepts_1234 Concepts #1, #2, #3 and #4: Objects (#1), methods (#2), signals (#3) and proxies (#4)
 *
 * \par
 * A dbus-cxx Object is a collection of methods and signals. A method is an
 * operation that may be invoked on a object and may or may not return a value.
 * A signal is emitted by the object and does not have a return value.
 *
 * \par
 * A dbus-cxx ObjectProxy provides an abstration of a remote Object to a client
 * application. The ObjectProxy provides method and signal proxies that have
 * signatures equivalent to their server-side counterparts.
 *
 * \par
 * A method proxy assembles the parameters of the method call into a D-Bus
 * message and transmits to the server. The response (if any) is returned in
 * the form of a D-Bus message which is then unpacked and returned to the
 * application as the return value from the proxy call.
 *
 * \par
 * A signal proxy is different from a method proxy in that it watches an Object
 * for signal events. These signal events arrive as a D-Bus signal message.
 * When a signal event arrives the parameters are unpacked and emitted locally
 * through the signal proxy callback.
 *
 * \section quick_start_concept_5 Concept #5: sigc++ signal and slot library
 * 
 * \par
 * dbus-cxx relies heavily on \c sigc++'s signal and slot system for both
 * Objects and their proxies.
 *
 * \par
 * On the server side and object's methods rely upon slots to provide the
 * underlying functionality. When an incoming method call is invoked, the
 * parameters of the call are unpacked and passed to the slot associated
 * with the method.
 *
 * \par
 * Slots are similar to function pointers in C, but can also be a method
 * of a specific class. The details of this are available in the \c sigc++
 * documentation and are beyond the scope of this tutorial.
 * 
 * \section quick_start_concept_678 Concepts #6, #7 and #8: Smart Pointers, pointer and create()
 * 
 * \par
 * An early design decision was made to focus on the use of smart pointers as
 * defined in the ANSI C++ committee's \c tr1 (Technical Report 1), which as
 * of this writing has been accepted by the committee and recommended to ISO
 * for incorporation as the first revision to the C++ standard. A reference
 * implementation can be found in the boost library, the \c std::tr1 namespace
 * of gcc 4.0 or greater, or the \c C++0x draft standard in gcc 4.3 or greater.
 *
 * \par
 * Two key concepts are the \c pointer typedef and the \c create method defined
 * in each dbus-cxx class.
 *
 * \par
 * The \c pointer typedef is defined \e within \e each \e class and is
 * typedefed to a smart pointer to that \e specific class. Thus,
 * \c DBus::Connection::pointer is a smart pointer to a D-Bus Connection
 * endpoint, and \c DBus::CallMessage::pointer is a smart pointer to a D-Bus
 * call message.
 *
 * \par
 * Each class also has static \c create() methods with parameters that are
 * identical to the class' constructors. Class constructors are \c protected,
 * so in general you cannot access them.
 *
 * \par
 * Instead, you will need to use the class' \c create() method to dynamically
 * creates an instance of the class. This method will returns a smart pointer
 * of the appropriate type for that class similar to the way the \c new
 * operator dynamically creates an object and returns a raw pointer to the
 * newly created object.
 *
 * \par
 * Since the \c create() method is static, you do not need an instance of the
 * class to call it; \c Classname::create() is sufficient. Thus, to create
 * a new dbus-cxx call message you would use \c DBus::CallMessage::create()
 * and to create a new dbus-cxx connection you would use
 * \c DBus::Connection::create().
 *
 * \par
 * Naturally, the smart pointer returned from \c create() can be assigned to
 * the class' \c pointer type, which allows you to use a syntax such as:
 * \code DBus::CallMessage::pointer my_callmessage = DBus::CallMessage::create(); \endcode
 *
 * <b>Continue On:</b> \ref quick_start_example_0
 *
 * <b>Go Back:</b> \ref quick_start_pkgconfig
 */

/**
 * \page quick_start_example_0 Quick start example 0: A simple server and client
 * 
 * \par
 * This example will demonstrate a simple server and client pair.
 *
 * \par
 * The server will only offer one method named \c add that will take two
 * \c double parameters and return the sum as a \c double.
 *
 * \par
 * The client will use a proxy object for the server as well as a proxy for the
 * \c add method.
 *
 * \section quick_start_server_0 A simple server
 *
 * \par
 * This section provides an example of a simple server. The server will run
 * for 10 seconds and then automatically shut-down.
 *
 * \par Summary
 * This is an overview of the code we will need in our example server:
 * -# Include headers
 * -# Initialize the dbus-cxx library
 * -# Create a dispatcher to manage threads, timeouts and I/O watches
 * -# Create a connection to the D-Bus session bus
 * -# Request a name for our application on the bus
 * -# Create an object on the bus
 * -# Add a method named \c add to our object
 * -# Wait for incoming calls
 * 
 * \section quick_start_server_0_code Simple server 0 code
 * 
 * \par
 * Here is the code in full, followed by a section-by-section discussion.
 * 
 * \include quickstart_server_0.cpp
 *
 * \section quick_start_server_0_discussion Simple server 0 code discussion
 *
 * \par Include headers
 * First, we need to include the main dbus-cxx library header:
 * \code
 * #include <dbus-cxx.h>
 * \endcode
 *
 * \par
 * Then we will define a function that will be our workhorse on the server side.
 * This function will be the actual function that will be called when the dbus
 * \c add method is invoked, so we will name our function similarly. But, note
 * that the actual name of the function in our program and the name in our
 * dbus server do not have to match.
 * \code double add( double param1, double param2 )      { return param1 + param2; } \endcode
 *
 * \par
 * And now for the beginning of our \c main() function. We'll also declare a
 * variable \c ret that can be used to check the return value of functions.
 * \code{.cpp}
 * int main()
 * {
 *   int ret;
 * \endcode
 *
 * \par Initialize the dbus-cxx library
 * Before we do anything in dbus-cxx we must call the library's \c init() method
 * which will take care of various initializations including the threading system.
 * \code DBus::init(); \endcode
 *
 * \par Create a dispatcher to manage threads, timeouts and I/O watches
 * Now, we will create a dispatcher to handle incoming and outgoing messages.
 * Handling incoming and outgoing messages can be messy and the Dispatcher
 * class handles much of this for us.
 * \code DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create(); \endcode
 *
 * \par Create a connection to the D-Bus session bus
 * Now that we have a dispatcher we need to create a connection to the session bus.
 * \code DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION); \endcode
 *
 * \par Request a name for our application on the bus
 * Next, we need to request a name that will identify our application on the
 * session bus.
 * \code
 *   ret = conn->request_name( "dbuscxx.quickstart_0.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
 *   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;
 * \endcode
 *
 * \par Create an object on the bus
 * Now that our application has a name on the bus we need to add an object with
 * it's path.
 * \code DBus::Object::pointer object = conn->create_object("/dbuscxx/quickstart_0"); \endcode
 *
 * \par Add a method named \c add to our object
 * We will now create a method named \c add for our object. The functionality
 * of the method will be provided by the function we declared above also named
 * \c add(). We must add this to an interface, and the D-Bus specification
 * required that interface names must contain at least one \b . (period)
 * character so we will use the interface name \c "dbuscxx.Quickstart".
 *
 * \par
 * Note that our object's method named \c add has nothing to do with C++
 * class methods. In essence we are creating virtual objects on the D-Bus
 * and can choose to provide their functionality with either C++ class methods
 * and/or plain functions.
 * \code object->create_method<double,double,double>("dbuscxx.Quickstart", "add", sigc::ptr_fun(add) ); \endcode
 *
 * \par Wait for incoming calls
 * Finally, we need to wait for incoming calls. We will sleep for 10 seconds in
 * our \c main() function and the calls will be handled in the dispatcher's
 * threads.
 * \code
 * sleep(10);
 *   
 *   return 0;
 * }
 * \endcode
 *
 * <b>Continue On:</b> \ref quick_start_client_0
 *
 * <b>Go Back:</b> \ref quick_start
 */

/**
 * \page quick_start_client_0 Quick Start Client 0
 *
 * \section quick_start_server_0 A simple client
 *
 * \par
 * This section provides an example of a simple client for our simple server.
 *
 * \par Summary
 * This is an overview of the code we will need in our example client:
 * -# Include headers
 * -# Initialize the dbus-cxx library
 * -# Create a dispatcher to manage threads, timeouts and I/O watches
 * -# Create a connection to the D-Bus session bus
 * -# Create a proxy object for the server's \c quickstart_0 object
 * -# Create a proxy \c add method for our proxy object
 * -# Call our proxy method
 * -# Print out the results
 *
 * \section quick_start_client_0_code Simple client 0 code
 *
 * \par
 * Here is the code in full, followed by a section-by-section discussion.
 *
 * \include quickstart_client_0.cpp
 *
 * \section quick_start_client_0_discussion Simple client 0 code discussion
 *
 * \par Include headers
 * First, we need to include the main dbus-cxx library header. We'll also
 * include the \c iostream header since we'll print out the results
 * of our addition.
 * \code
 * #include <dbus-cxx.h>
 * #include <iostream>
 * \endcode
 *
 * \par
 * And now for the beginning of our \c main() function.
 * \code
 * int main()
 * {
 * \endcode
 *
 * \par Initialize the dbus-cxx library
 * Before we do anything in dbus-cxx we must call the library's \c init() method
 * which will take care of various initializations including the threading system.
 * \code DBus::init(); \endcode
 *
 * \par Create a dispatcher to manage threads, timeouts and I/O watches
 * Now, we will create a dispatcher to handle incoming and outgoing messages.
 * Handling incoming and outgoing messages can be messy and the Dispatcher
 * class handles much of this for us.
 * \code DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create(); \endcode
 *
 * \par Create a connection to the D-Bus session bus
 * Now that we have a dispatcher we need to create a connection to the session bus.
 * \code DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION); \endcode
 *
 * \par
 * \par Request a name for our application on the bus
 * Next, we need to request a name that will identify our application on the
 * session bus.
 * \code
 *   ret = conn->request_name( "dbuscxx.quickstart_0.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
 *   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;
 * \endcode
 *
 * \par Create a proxy object for the server's \c quickstart_0 object
 * You'll notice that up to this point the code for the client and the server
 * has been nearly identical. This is the point at which they will diverge.
 * We will now create a proxy object for the object on the server.
 * \code
 *   DBus::ObjectProxy::pointer object;
 *   object = connection->create_object_proxy("dbuscxx.quickstart_0.server", "/dbuscxx/quickstart_0");
 * \endcode
 * Note that we must use the name the server requested as well as the object
 * path the server used when it created it's object when we create the
 * proxy object.
 *
 * \par Create a proxy \c add method for our proxy object
 * Now we create a proxy \c add method for the server object's \c add method.
 * This will allow us to use function notation to call the method. When
 * creating the proxy method we must use the same interface
 * (\c "dbuscxx.Quickstart" ) and method name ( \c "add" ) as the server used.
 * \code
 *   DBus::MethodProxy<double,double,double>& add_proxy
 *     = *(object->create_method<double,double,double>("dbuscxx.Quickstart","add"));
 * \endcode
 *
 * \par Call our proxy method
 * Now we can use our proxy method to call the method on the server using
 * function notation. We'll keep things simple and just add \c 1.1 \c + \c 2.2 .
 * \code
 *   double answer;
 *   answer = add_proxy( 1.1, 2.2 );
 * \endcode
 *
 * \par Print out the results
 * Finally, we can print out the results.
 * \code
 *   std::cout << "1.1 + 2.2 = " << answer << std::endl;
 *
 *   return 0;
 * }
 * \endcode
 *
 * <b>Go Back:</b> \ref quick_start
 */
