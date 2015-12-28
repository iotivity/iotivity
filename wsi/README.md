# Web Service Interface 

## Description

The Web Service Interface of IoTivity aims to simplify representation of web based services in an network of IoTivity devices. It will act as a conceptual bridging layer between the IoTivity network and web based services.

Bridging the two, needs a few common features to be supported.

Service specific provisioning and configuration.
Platform specific abstractions.
A simplified app framework for developing new kinds of IoT based applications.


## Installation and Usage


    sudo apt-get install npm nodejs nodejs-lecgacy mongodb libkrb5-dev
    git clone https://github.com/otcshare/iotivity-node.git
    cd iotivity-node
    export IOTIVITY_DIR="/home/developer/demo/iotivity/"
    export OCTBSTACK_CFLAGS="-I$IOTIVITY_DIR/resource/csdk/stack/include -I${IOTIVITY_DIR}/resource/c_common -I${IOTIVITY_DIR}/resource/c_common/ocrandom/include"
    export OCTBSTACK_LIBS="-L${IOTIVITY_DIR}/out/linux/x86_64/release/ -loctbstack -Wl,-rpath=${IOTIVITY_DIR}/out/linux/x86_64/release/"
    npm install
    cd ..
    npm install
    node server.js

from browser

    localhost:8080

Use the web service interface to integrate iotivity and web services.

