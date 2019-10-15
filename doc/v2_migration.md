dbus-cxx 2.0 Migration Guide
===

# Migration
Migrating code to the 2.0 version of dbus-cxx should be pretty straightforward.
The library must now be built with the libsigc++ version 3.0.  This also means
that you need a compiler that supports C++17.

This now means that argument lists of an arbitrary length can be used, instead
of the previous hard limit of 7.

The syntax for declaring methods has now also changed slightly: instead of
having the first template parameter be the return value, the types now correspond
to method signatures.  For example, given the following code with version 1:
```
    DBus::Object::pointer object = conn->create_object("/dbuscxx/example/Calculator");

    object->create_method<double,double,double>("Calculator.Basic", "add", sigc::ptr_fun(add) );
```

This code now becomes:
```
    DBus::Object::pointer object = conn->create_object("/dbuscxx/example/Calculator");

    object->create_method<double(double,double)>("Calculator.Basic", "add", sigc::ptr_fun(add) );
```
