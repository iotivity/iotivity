#!/bin/bash

echo "simpeserver start"
./../../../out/linux/x86_64/release/resource/examples/simpleserver &>/dev/null &

echo "garageserver start"
./../../../out/linux/x86_64/release/resource/examples/garageserver &>/dev/null &

echo -e "wait servers starting\n"
sleep 3

echo -e "test1 : findresource all and get and put\n"
node test1.js
sleep 7

echo -e "test2 : findresource resourcetype and get and put\n"
node test2.js
sleep 7

echo "kill test server"
ps -ef | grep simpleserver | awk '{print $2}' | grep -v 'grep' | xargs kill -9
ps -ef | grep garageserver | awk '{print $2}' | grep -v 'grep' | xargs kill -9