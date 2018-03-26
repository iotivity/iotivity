FROM openjdk:8-jre-alpine

ADD ./target/CloudAccount-0.0.1-SNAPSHOT.jar iotivity/AccountServer.jar
ADD ./target/lib/* iotivity/lib/
ADD ./properties/* iotivity/properties/

WORKDIR iotivity/

ENV COAP_PORT 5685
ENV TLS_MODE 0
ENV MONGODB_ADDRESS mongodb
ENV MONGODB_PORT 27017


ENV JMX_PORT 9002
ENV JAVA_JMX_OPTS "-Dcom.sun.management.jmxremote \
               -Dcom.sun.management.jmxremote.port=${JMX_PORT} \
               -Dcom.sun.management.jmxremote.rmi.port=${JMX_PORT} \
               -Dcom.sun.management.jmxremote.local.only=false \
               -Dcom.sun.management.jmxremote.authenticate=false \
               -Dcom.sun.management.jmxremote.ssl=false \
               -Djava.rmi.server.hostname= \
               -XX:MetaspaceSize=30m -XX:MaxMetaspaceSize=40m \
               -XX:+UseG1GC -XX:MaxGCPauseMillis=100 -server "

EXPOSE 9002

ENTRYPOINT exec java $JAVA_JMX_OPTS $JAVA_MEMOTY_OPTS -jar AccountServer.jar
