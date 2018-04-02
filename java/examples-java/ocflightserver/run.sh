# cd <iotivity>/out/linux/x86_64/release/java
java -Djava.library.path=.. -jar ocflightserver.jar  "$1" "$2" "$3"

# using the same 'name' will use the same uuid from names.prop file

# java -Djava.library.path=.. -jar ocflightserver.jar  "name" "powerState" "dimmingSetting"
# java -Djava.library.path=.. -jar ocflightserver.jar  Kitchen on 75
