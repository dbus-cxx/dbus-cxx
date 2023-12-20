Multiple Return Values {#multiple-return}
==

## Multiple Return Values

dbus-cxx has support for multiple return values from functions.  In order to
utilize this, you must use the DBus::MultipleReturn class.  This class is
essentially a wrapper around `std::tuple`, but provides the types needed by
the library to properly handle the logic with multiple return values.

Because this is basically a wrapper around `std::tuple`, in order to get the values
is a two step process:

1. Get the tuple
2. Use `std::get` in order to get the values

Example:

```{.cpp}
    DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>> val;

    // Call the method that returns multiple values
    // This must have been previously created, and would look something like:
    // std::shared_ptr<DBus::MethodProxy<DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>>()>> method_proxy;
    // Notice how the return value is defined as a DBus::MultipleReturn, and there are no arguments to the method proxy
    val = (*method_proxy)();

    std::tuple<int32_t, int32_t, std::string, std::vector<int>> d = val.m_data;

    std::get<0>(d);  // Get the first int32_t in our tuple d
    std::get<1>(d);  // Get the second int32_t in our tuple d
    std::get<2>(d);  // Get the std::string in our tuple d
    // ... etc ...
```

Note that if you name arguments(using the `DBus::MethodBase.set_arg_name()` method),
the earlier argument names are the return values.  For example, if your multiple
return has two arguments, `set_arg_name(0, "name")` and  `set_arg_name(1, "name")`
are the way to set the arguments.  The remaining arg names are for parameters to
the function.

