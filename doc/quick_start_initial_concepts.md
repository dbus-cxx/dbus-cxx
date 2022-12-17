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

\section quick_start_concept_678 Concepts #6, #7: Smart Pointers and create()

DBus-cxx makes extensive use of smart pointers to help manage the memory of a
class.  In order for this scheme to work properly, most classes cannot be
initialized on their own(constructors are private).  To create an instance
of a class, use the `create()` method on that class:

```
std::shared_ptr<DBus::Dispatcher> disp = DBus::StandaloneDispatcher::create();
```

<b>Continue On:</b> \ref quick_start_example_0

<b>Go Back:</b> \ref quick_start_pkgconfig

