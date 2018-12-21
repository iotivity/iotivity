FROM openjdk:8-jre-alpine

RUN apk add --update \
    curl \
    shadow \
    && rm -rf /var/cache/apk/*

RUN groupadd -r iotivity && useradd --no-log-init --system  -g iotivity iotivity

COPY --chown=iotivity:iotivity ./target/CloudResourceDirectory-0.0.1-SNAPSHOT.jar iotivity/ResourceDirectory.jar
COPY --chown=iotivity:iotivity ./target/lib/* iotivity/lib/
COPY --chown=iotivity:iotivity ./start.sh iotivity/

WORKDIR iotivity/

ENV COAP_PORT 5684
ENV HEALTH_CHECK_ENABLED 1
ENV TLS_MODE 0
ENV MONGODB_ADDRESS mongodb
ENV MONGODB_PORT 27017

ENV JMX_PORT 9004
ENV JMX_HOSTNAME ""
ENV JAVA_MEMORY_OPTS ""

EXPOSE 9004

USER iotivity
ENTRYPOINT [ "./start.sh" ]
HEALTHCHECK --interval=30s --timeout=2s --retries=3	CMD curl -f http://localhost:8000/api/healthcheck || exit 1