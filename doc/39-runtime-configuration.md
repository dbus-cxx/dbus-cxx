Runtime configuration of dbus-cxx {#runtime-configuration}
==

There are several things that influence dbus-cxx that can be configured at runtime.

# Default message endianess

By default, dbus-cxx will send messages in big-endian order.  This can lead to issues with certain applications
that only expect data in little-endian format.  In order to change this to little-endian by default,
either call DBus::set_default_endianess or set the environment variable DBUSCXX_ENDIANESS to either 'B' or 'l'
for big or little endian.

# Logging

The dbus-cxx library contains a simple logging mechanism in order to help debug.  In order to use this feature,
you may either set the logging function to a default implementation that will log to stderr, or you may
install your own logging handler in order to route messages as you feel appropriate.

The relevant methods are DBus::set_logging_function and DBus::log_std_err.
