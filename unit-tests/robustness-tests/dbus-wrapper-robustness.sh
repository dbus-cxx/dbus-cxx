#!/bin/sh

# launch up an instance of the dbus and source the data output
# to see what the address is and what the PID is
TMPFILE=$(mktemp)
dbus-launch > $TMPFILE

. $TMPFILE
export DBUS_SESSION_BUS_ADDRESS
export PATH=$PATH:$(pwd)

dbus-monitor --pcap > /tmp/dbus-mon.pcap &
DBUS_MON_PID=$!

./test-robustness-rx > /tmp/dbus-rx.txt 2>&1 &
CLIENT_PID=$!
sleep .1
./test-robustness-sender > /tmp/dbus-tx.txt 2>&1
# get the exit code 
EXIT_CODE=$?

kill -0 $CLIENT_PID
CLIENT_EXISTS=$?

if [ "$CLIENT_EXISTS" -ne "0" ]
then
	EXIT_CODE=2
else
	sleep 6
	kill -0 $CLIENT_PID
	CLIENT_EXISTS=$?
	if [ "$CLIENT_EXISTS" -ne "0" ]
	then
		echo "Killing client!"
		kill $CLIENT_PID
	fi
fi


kill $DBUS_MON_PID
kill $DBUS_SESSION_BUS_PID
rm $TMPFILE

exit $EXIT_CODE
