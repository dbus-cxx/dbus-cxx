#!/bin/sh

# launch up an instance of the dbus and source the data output
# to see what the address is and what the PID is
TMPFILE=$(mktemp)
dbus-launch > $TMPFILE

. $TMPFILE
export DBUS_SESSION_BUS_ADDRESS
export PATH=$PATH:$(pwd)

./test-filedescriptor server $1 &
SERVER_PID=$!
sleep .1
./test-filedescriptor client $1
# get the exit code 
EXIT_CODE=$?

# wait for server to be done
wait $SERVER_PID

kill $DBUS_SESSION_BUS_PID
rm $TMPFILE

exit $EXIT_CODE
