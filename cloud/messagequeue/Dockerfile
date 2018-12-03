FROM openjdk:8-jre-alpine

RUN apk add --update \
    shadow \
    && rm -rf /var/cache/apk/*

RUN groupadd -r iotivity && useradd --no-log-init --system  -g iotivity iotivity

COPY --chown=iotivity:iotivity  ./target/CloudMessageQueue-0.0.1-SNAPSHOT.jar iotivity/MessageQueue.jar
COPY --chown=iotivity:iotivity  ./target/lib/* iotivity/lib/
COPY --chown=iotivity:iotivity  ./start.sh iotivity/

WORKDIR iotivity/

ENV COAP_PORT 5686
ENV TLS_MODE 0
ENV ZOOKEEPER_ADDRESS kafka-zookeeper
ENV ZOOKEEPER_PORT 2181
ENV KAFKA_ADDRESS kafka-zookeeper
ENV KAFKA_PORT 9092

ENV JMX_PORT 9003
ENV JMX_HOSTNAME ""
ENV JAVA_MEMORY_OPTS ""

EXPOSE 9003

USER iotivity
ENTRYPOINT [ "./start.sh" ]

