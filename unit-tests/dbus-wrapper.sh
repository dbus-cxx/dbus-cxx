#!/bin/sh

# launch up an instance of the dbus and source the data output
# to see what the address is and what the PID is
TMPFILE=$(mktemp)
dbus-launch > $TMPFILE

. $TMPFILE
export DBUS_SESSION_BUS_ADDRESS
export PATH=$PATH:$(pwd)

# exec whatever was passed to us
$@
# get the exit code 
EXIT_CODE=$?

kill $DBUS_SESSION_BUS_PID
rm $TMPFILE

exit $EXIT_CODE
