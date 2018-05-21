FROM openjdk:8-jre-alpine

ADD ./target/CloudInterface-0.0.1-SNAPSHOT.jar iotivity/CloudInterface.jar
ADD ./target/lib/* iotivity/lib/

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
ENV JAVA_JMX_OPTS "-Dcom.sun.management.jmxremote \
                -Dcom.sun.management.jmxremote.port=${JMX_PORT} \
                -Dcom.sun.management.jmxremote.rmi.port=${JMX_PORT} \
                -Dcom.sun.management.jmxremote.local.only=false \
                -Dcom.sun.management.jmxremote.authenticate=false \
                -Dcom.sun.management.jmxremote.ssl=false \
                -Djava.rmi.server.hostname= \
                -XX:MetaspaceSize=20m -XX:MaxMetaspaceSize=25m \
                -XX:+UseG1GC -XX:MaxGCPauseMillis=100 -server "

EXPOSE 9001

ENTRYPOINT exec java $JAVA_JMX_OPTS $JAVA_MEMORY_OPTS -jar CloudInterface.jar
