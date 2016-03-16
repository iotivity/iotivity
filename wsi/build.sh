#!/bin/bash

echo 'Fetch iotivity-node and build against iotivity 1.0.0'
#V=1 npm install iotivity-node
#mv ./node_modules/iotivity-node/  ./

cd <IOTIVITY_DIR>
git checkout -b 1.0.1 1.0.1
scons
git checkout wsi-dev
cd <WSI_DIR>
git clone https://github.com/otcshare/iotivity-node.git
cd iotivity-node
npm install
cd <WSI_DIR>
echo 'Install wsi dependencies'
npm install

#echo 'Move iotivity-node back to node_modules directory'
#mv iotivity-node ./node_modules/

echo 'Build complete!'
