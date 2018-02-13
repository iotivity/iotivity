FROM openjdk:8-jre-alpine

ADD ./target/CloudInterface-0.0.1-SNAPSHOT.jar iotivity/CloudInterface.jar
ADD ./target/lib/* iotivity/lib/

WORKDIR iotivity/

ENV COAP_PORT 5683
ENV TLS_MODE 0
ENV KEEPALIVE_CLOUD 1
ENV HC_PROXY_MODE 0
ENV WEBSOCKET_MODE 0
ENV RESOURCE_DIRECTORY_ADDRESS iotivity-resourcedirectory
ENV RESOURCE_DIRECTORY_PORT 5684
ENV ACCOUNT_SERVER_ADDRESS iotivity-accountserver
ENV ACCOUNT_SERVER_PORT 5685

ENTRYPOINT ["java", "-jar", "CloudInterface.jar"]