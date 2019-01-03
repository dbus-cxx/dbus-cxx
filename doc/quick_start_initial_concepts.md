Initial Concepts {#quick_start_initial_concepts}
==

\section quick_start_concepts_1234 Concepts #1, #2, #3 and #4: Objects (#1), methods (#2), signals (#3) and proxies (#4)

\par
A dbus-cxx Object is a collection of methods and signals. A method is an
operation that may be invoked on a object and may or may not return a value.
A signal is emitted by the object and does not have a return value.

\par
A dbus-cxx ObjectProxy provides an abstration of a remote Object to a client
application. The ObjectProxy provides method and signal proxies that have
signatures equivalent to their server-side counterparts.

\par
A method proxy assembles the parameters of the method call into a D-Bus
message and transmits to the server. The response (if any) is returned in
the form of a D-Bus message which is then unpacked and returned to the
application as the return value from the proxy call.

\par
A signal proxy is different from a method proxy in that it watches an Object
for signal events. These signal events arrive as a D-Bus signal message.
When a signal event arrives the parameters are unpacked and emitted locally
through the signal proxy callback.

\section quick_start_concept_5 Concept #5: sigc++ signal and slot library

\par
dbus-cxx relies heavily on \c sigc++'s signal and slot system for both
Objects and their proxies.

\par
On the server side and object's methods rely upon slots to provide the
underlying functionality. When an incoming method call is invoked, the
parameters of the call are unpacked and passed to the slot associated
with the method.

\par
Slots are similar to function pointers in C, but can also be a method
of a specific class. The details of this are available in the \c sigc++
documentation and are beyond the scope of this tutorial.

\section quick_start_concept_678 Concepts #6, #7 and #8: Smart Pointers, pointer and create()

\par
An early design decision was made to focus on the use of smart pointers as
defined in the ANSI C++ committee's \c tr1 (Technical Report 1), which as
of this writing has been accepted by the committee and recommended to ISO
for incorporation as the first revision to the C++ standard. A reference
implementation can be found in the boost library, the \c std::tr1 namespace
of gcc 4.0 or greater, or the \c C++0x draft standard in gcc 4.3 or greater.

\par
Two key concepts are the \c pointer typedef and the \c create method defined
in each dbus-cxx class.

\par
The \c pointer typedef is defined \e within \e each \e class and is
typedefed to a smart pointer to that \e specific class. Thus,
\c DBus::Connection::pointer is a smart pointer to a D-Bus Connection
endpoint, and \c DBus::CallMessage::pointer is a smart pointer to a D-Bus
call message.

\par
Each class also has static \c create() methods with parameters that are
identical to the class' constructors. Class constructors are \c protected,
so in general you cannot access them.

\par
Instead, you will need to use the class' \c create() method to dynamically
creates an instance of the class. This method will returns a smart pointer
of the appropriate type for that class similar to the way the \c new
operator dynamically creates an object and returns a raw pointer to the
newly created object.

\par
Since the \c create() method is static, you do not need an instance of the
class to call it; \c Classname::create() is sufficient. Thus, to create
a new dbus-cxx call message you would use \c DBus::CallMessage::create()
and to create a new dbus-cxx connection you would use
\c DBus::Connection::create().

\par
Naturally, the smart pointer returned from \c create() can be assigned to
the class' \c pointer type, which allows you to use a syntax such as:
\code DBus::CallMessage::pointer my_callmessage = DBus::CallMessage::create(); \endcode

<b>Continue On:</b> \ref quick_start_example_0

<b>Go Back:</b> \ref quick_start_pkgconfig

