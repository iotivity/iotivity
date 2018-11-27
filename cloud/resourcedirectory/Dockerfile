FROM openjdk:8-jre-alpine

RUN apk add --update \
    curl \
    && rm -rf /var/cache/apk/*

ADD ./target/CloudResourceDirectory-0.0.1-SNAPSHOT.jar iotivity/ResourceDirectory.jar
ADD ./target/lib/* iotivity/lib/

WORKDIR iotivity/

ENV COAP_PORT 5684
ENV HEALTH_CHECK_ENABLED 1
ENV TLS_MODE 0
ENV MONGODB_ADDRESS mongodb
ENV MONGODB_PORT 27017

ENV JMX_PORT 9004
ENV JAVA_JMX_OPTS "-Dcom.sun.management.jmxremote \
               -Dcom.sun.management.jmxremote.port=${JMX_PORT} \
               -Dcom.sun.management.jmxremote.rmi.port=${JMX_PORT} \
               -Dcom.sun.management.jmxremote.local.only=false \
               -Dcom.sun.management.jmxremote.authenticate=false \
               -Dcom.sun.management.jmxremote.ssl=false \
               -Djava.rmi.server.hostname= \
               -XX:MetaspaceSize=20m -XX:MaxMetaspaceSize=25m \
               -XX:+UseG1GC -XX:MaxGCPauseMillis=100 -server "

EXPOSE 9004

ENTRYPOINT exec java $JAVA_JMX_OPTS $JAVA_MEMORY_OPTS -jar ResourceDirectory.jar
HEALTHCHECK --interval=30s --timeout=2s --retries=3	CMD curl -f http://localhost/api/healthcheck || exit 1
