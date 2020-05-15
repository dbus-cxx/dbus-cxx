#!/bin/sh

# launch up an instance of the dbus and source the data output
# to see what the address is and what the PID is
TMPFILE=$(mktemp)
dbus-launch > $TMPFILE

. $TMPFILE
export DBUS_SESSION_BUS_ADDRESS
export PATH=$PATH:$(pwd)

./test-robustness-rx &
CLIENT_PID=$!
sleep .1
./test-robustness-sender
# get the exit code 
EXIT_CODE=$?

kill -0 $CLIENT_PID
CLIENT_EXISTS=$?

if [ "$CLIENT_EXISTS" -ne "0" ]
then
	EXIT_CODE=2
else
	kill $CLIENT_PID
fi


kill $DBUS_SESSION_BUS_PID
rm $TMPFILE

exit $EXIT_CODE
