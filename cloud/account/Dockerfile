FROM openjdk:8-jre-alpine

ADD ./target/CloudAccount-0.0.1-SNAPSHOT.jar iotivity/AccountServer.jar
ADD ./target/lib/* iotivity/lib/
ADD ./properties/* iotivity/properties/

WORKDIR iotivity/

ENV COAP_PORT 5685
ENV TLS_MODE 0
ENV MONGODB_ADDRESS mongodb
ENV MONGODB_PORT 27017

ENTRYPOINT ["java", "-jar", "AccountServer.jar"]