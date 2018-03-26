FROM openjdk:8-jre-alpine

ADD ./target/CloudMessageQueue-0.0.1-SNAPSHOT.jar iotivity/MessageQueue.jar
ADD ./target/lib/* iotivity/lib/

WORKDIR iotivity/

ENV COAP_PORT 5686
ENV TLS_MODE 0
ENV ZOOKEEPER_ADDRESS kafka-zookeeper
ENV ZOOKEEPER_PORT 2181
ENV KAFKA_ADDRESS kafka-zookeeper
ENV KAFKA_PORT 9092

ENV JMX_PORT 9003
ENV JAVA_JMX_OPTS "-Dcom.sun.management.jmxremote \
               -Dcom.sun.management.jmxremote.port=${JMX_PORT} \
               -Dcom.sun.management.jmxremote.rmi.port=${JMX_PORT} \
               -Dcom.sun.management.jmxremote.local.only=false \
               -Dcom.sun.management.jmxremote.authenticate=false \
               -Dcom.sun.management.jmxremote.ssl=false \
               -Djava.rmi.server.hostname= \
               -XX:MetaspaceSize=20m -XX:MaxMetaspaceSize=25m \
               -XX:+UseG1GC -XX:MaxGCPauseMillis=100 -server "

EXPOSE 9003

ENTRYPOINT exec java $JAVA_JMX_OPTS $JAVA_MEMOTY_OPTS -jar MessageQueue.jar
