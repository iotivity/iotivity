FROM openjdk:8-jre-alpine

ADD ./target/CloudResourceDirectory-0.0.1-SNAPSHOT.jar iotivity/ResourceDirectory.jar
ADD ./target/lib/* iotivity/lib/

WORKDIR iotivity/

ENV COAP_PORT 5684
ENV TLS_MODE 0
ENV MONGODB_ADDRESS mongodb
ENV MONGODB_PORT 27017

ENTRYPOINT ["java", "-jar", "ResourceDirectory.jar"]