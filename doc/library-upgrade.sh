#!/bin/bash

FILE=$1

# Error changes(throws raw error, not shared_ptr)
sed -i 's/Error::pointer/Error/g' $FILE
sed -i 's/\(Error[a-zA-Z]*\)::create/\1/g' $FILE
