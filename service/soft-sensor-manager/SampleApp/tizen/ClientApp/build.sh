#!/bin/bash

# Prevent Support Tool(Defect analysis and commit) Execution Batch File!!

# variables
SDBPATH=/home/iotivity/tizen-sdk/tools/sdb
GBSRPMPATH=/home/iotivity/GBS-ROOT/rsa/local/repos/rsa/armv7l/RPMS
PACKAGE=oic-0.0.1-0.armv7l.rpm

gbs build -A armv7l --include-all --ccache

$SDBPATH root on

$SDBPATH push $GBSRPMPATH/$PACKAGE /root/

$SDBPATH shell rpm -Uvh --force /root/oic-0.0.1-0.armv7l.rpm

$SDBPATH shell /usr/apps/com.samsung.oicapp-test/bin/oicapp-test
