#!/bin/bash

cd ${1}

args=''
for i in `seq 3 $#`
do
    eval arg=\$${i}
    args+=' '${arg}
done

echo "./${2} ${args}"
./${2} ${args}
