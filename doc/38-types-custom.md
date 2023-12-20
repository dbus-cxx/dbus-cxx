Custom types {#custom-types}
===

Sometimes an API that you are interfacing with would be more convenient
if you could pass the parameters as a struct, or perhaps you want to
serialize a struct to DBus as a collection of basic types.

DBus-cxx has the ability to help you out, by letting you define custom
types that you can send across the bus.  Note that all types must still
consist of only standard DBus types; there is no way to add another
type that can be marshalled on the bus(see the [dbus types](dbus-types.html)
page for more details on what types can be sent and received).

In order to serialize a custom type, we need three things:

1. The signature of the type.  For example, `ii` would be two INT32 values.  This method must be named `signature` in the DBus namespace.
2. An `operator>>` override for the `DBus::MessageIterator` class
3. An `operator<<` override for the `DBus::MessageAppendIterator` class

There are some caveats with doing this:

* The inclusion order of the headers is very important.  Because the
signature method needs to exist before any dbus-cxx headers, the exact order
in which you define and use the headers is very important.

See the examples/types folder for a complete example.
