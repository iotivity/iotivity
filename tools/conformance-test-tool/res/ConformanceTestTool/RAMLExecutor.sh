#!/usr/bin/env bash

##############################################################################
##
##  RAMLExecutor start up script for UNIX
##
##############################################################################

echo "RAMLExecutor starts..."

ramlParser="./RAMLParser"
defaultPath="../ramls"
dutFile="DUTDescriptor.json"
dutCopyPath="../libs/"
allRamls="/*.raml"

if [ ! -d "$dutCopyPath" ]; then
	echo "$dutCopyPath directory is creating..."
	libDirCommand="mkdir $dutCopyPath"

	if eval $libDirCommand; then
		echo "$dutCopyPath created successfully.."
	else
		echo "Can't create $dutCopyPath directory..."
	fi
fi

postCondition()
{
	copyDUTDes="cp $dutFile $dutCopyPath"
	if eval $copyDUTDes;then
		echo "DUTDescriptor file copied successfully to $dutCopyPath directory.."
		removeDUT="rm $dutFile"
		if eval $removeDUT; then
			echo "$dutFile file deleted from current directory.."
		else
			echo "$dutFile file can't be deleted from current directory.."
		fi

		changeMode="chmod 777 ../libs/$dutFile"
		if eval $changeMode; then
			echo "$dutFile file mode is changed.."
		else
			echo "Can't change the mode of $dutFile file.."
		fi
	else
		echo "Can't copy the descriptor file to $dutCopyPath directory.."
	fi
}

if [ $# -eq 0 ]
  then
	executeAllCommand="$ramlParser $defaultPath$allRamls"
    if eval $executeAllCommand; then
		echo "Successfully executed for All Raml Parse"
		postCondition
	else
		echo "Can't execute command: $executeAllCommand"
	fi
	exit 0
fi

args=""

for i in `seq 1 $#` 
do
	eval arg=\$$i
	arg=${arg// /+}
    args+=$arg" "
done

arg_parts=(${args//,/ })

len=${#arg_parts[@]}

fileNames=""

i=0
while [ $i -lt $len ]
do
	fileNames="$fileNames $defaultPath/${arg_parts[$i]}"
	((i++))
done

command="$ramlParser$fileNames"

echo "Executing starts for RAMLParser with command: $command"

if eval $command; then
	echo "Successfully Created Json"
	postCondition
else
	echo "Failed to Create the DUTDescriptor file..."
fi

