#! /bin/bash

# Run depmod -a on modules to generate module* files.

# used for testing BASE_DIR=$(pwd)

DEPMOD=$BASE_DIR/host/sbin/depmod

$DEPMOD -a -v $(ls $BASE_DIR/target/lib/modules) -b $BASE_DIR/target >> /dev/null

exit $?



