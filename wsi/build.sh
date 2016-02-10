#!/bin/bash

echo 'Fetch iotivity-node and build against iotivity 1.0.0'
V=1 npm install iotivity-node
mv ./node_modules/iotivity-node/  ./

echo 'Install wsi dependencies'
npm install

echo 'Setting running environment'
export LD_LIBRARY_PATH=$PWD/iotivity-node/deps/iotivity/lib/

echo 'Build complete!'