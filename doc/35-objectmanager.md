ObjectManager support {#object-manager}
===

As of version 2.6.0, dbus-cxx has support for the ObjectManager interface.
This interface allows for interfaces and their properties to be queried
for using just one method call.

This is required for certain applications, notably BlueZ when creating
a GATT server.

Because this is an uncommon interface to use, it must be enabled manually
at a given level.  In order to activate it, call `DBus::Object::set_has_objectmanager`
on the object that should implement this interface.

This support is somewhat experimental and may have some issues in the real
world.  More testing in real-world applications is appreciated; if you find
issues, please open an issue on Github.
