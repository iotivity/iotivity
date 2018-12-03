#!/bin/sh

JAVA_OPTS="$JAVA_OPTS "
if [ ! -z "$JAVA_MEMORY_OPTS" ]; then
    JAVA_OPTS="$JAVA_OPTS $JAVA_MEMORY_OPTS"
fi

JAVA_OPTS="$JAVA_OPTS -Dcom.sun.management.jmxremote "
JAVA_OPTS="$JAVA_OPTS -Dcom.sun.management.jmxremote.port=${JMX_PORT} "
JAVA_OPTS="$JAVA_OPTS -Dcom.sun.management.jmxremote.rmi.port=${JMX_PORT} "
JAVA_OPTS="$JAVA_OPTS -Dcom.sun.management.jmxremote.local.only=false "
JAVA_OPTS="$JAVA_OPTS -Dcom.sun.management.jmxremote.authenticate=false "
JAVA_OPTS="$JAVA_OPTS -Dcom.sun.management.jmxremote.ssl=false "

if [ ! -z "$JMX_HOSTNAME" ]; then
   JAVA_OPTS="$JAVA_OPTS  -Djava.rmi.server.hostname=${JMX_HOSTNAME} "
fi

JAVA_OPTS="$JAVA_OPTS -XX:MetaspaceSize=20m "
JAVA_OPTS="$JAVA_OPTS -XX:MaxMetaspaceSize=30m "
JAVA_OPTS="$JAVA_OPTS -XX:+UseG1GC "
JAVA_OPTS="$JAVA_OPTS -XX:MaxGCPauseMillis=100 "
JAVA_OPTS="$JAVA_OPTS -server "

export JAVA_OPTS
echo "Start resource directory service with option: ${JAVA_OPTS}"
exec java $JAVA_OPTS -jar ResourceDirectory.jar
