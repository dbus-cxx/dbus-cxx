Multiple Return Values {#multiple-return}
==

## Multiple Return Values

dbus-cxx has support for multiple return values from functions.  In order to
utilize this, you must use the DBus::MultipleReturn class.  This class is
essentially a wrapper around std::tuple, but provides the types needed by
the library to properly handle the logic with multiple return values.

Note that if you name arguments(using the `DBus::MethodBase.set_arg_name()` method),
the earlier argument names are the return values.  For example, if your multiple
return has two arguments, `set_arg_name(0, "name")` and  `set_arg_name(1, "name")`
are the way to set the arguments.  The remaining arg names are for parameters to
the function.

