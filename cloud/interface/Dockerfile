FROM openjdk:8-jre-alpine

RUN apk add --update \
    shadow \
    && rm -rf /var/cache/apk/*

RUN groupadd -r iotivity && useradd --no-log-init --system  -g iotivity iotivity

COPY --chown=iotivity:iotivity  ./target/CloudInterface-0.0.1-SNAPSHOT.jar iotivity/CloudInterface.jar
COPY --chown=iotivity:iotivity  ./target/lib/* iotivity/lib/
COPY --chown=iotivity:iotivity  ./start.sh iotivity/

WORKDIR iotivity/

ENV COAP_PORT 5683
ENV TLS_MODE 0
ENV KEEPALIVE_CLOUD 1
ENV HTTP_PROXY_MODE 0
ENV HTTP_PROXY_PORT 80
ENV WEBSOCKET_MODE 0
ENV RESOURCE_DIRECTORY_ADDRESS iotivity-resourcedirectory
ENV RESOURCE_DIRECTORY_PORT 5684
ENV ACCOUNT_SERVER_ADDRESS iotivity-accountserver
ENV ACCOUNT_SERVER_PORT 5685

ENV JMX_PORT 9001
ENV JMX_HOSTNAME ""
ENV JAVA_MEMORY_OPTS ""

EXPOSE 9001

USER iotivity
ENTRYPOINT [ "./start.sh" ]
