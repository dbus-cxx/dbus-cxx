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

========
# NOTES FOR REVIEW
* Should we allow appending of float values?  Only double values can be sent
* Should we allow appending of int8_t?  Only uint8_t can be sent
* Check all examples to make sure they all work
* Do we need to have `long long int` and/or `long int` as a type we can send out?
 It seems like you should just used the fixed-sized types if you want this.
* What should the template of signals be?  Currently, it is similar to v1, except
 that the first template arg(retrun val) has been removed.  This is because DBus
 signals can't have a return value.  However, this makes them very different from
 methods, which take the method type as the template param.
