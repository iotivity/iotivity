#!/bin/bash

echo 'Fetch iotivity-node and build against iotivity 1.0.0'
V=1 npm install iotivity-node
mv ./node_modules/iotivity-node/  ./

echo 'Install wsi dependencies'
npm install

echo 'Move iotivity-node back to node_modules directory'
mv iotivity-node ./node_modules/

echo 'Build complete!'