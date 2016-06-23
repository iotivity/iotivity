
echo "gbs build started"
echo ""
echo $(pwd)

cd extlibs/gtest-1.7.0

mkdir -p bin/tizen
rm -f bin/tizen/*

echo "Initializing Git repository"
echo " "

if [ ! -d .git ]; then
   git init ./
   git config user.email "you@example.com"
   git config user.name "Your Name"
   git add ./
   git commit -m "Initial commit"
fi

echo "Calling core gbs build command"
gbscommand="gbs build -A armv7l -B ~/GBS-ROOT-OIC --include-all"
echo $gbscommand

if eval $gbscommand; then
   echo "GBS build is successful"
else
   echo "GBS build failed."
   echo "deleting tmp resources"
   rm -rf .git/
   exit
fi

echo "Extracting RPM"
rpmPath=$1
rpmName="org.tizen.gtest-0.0.1-1.armv7l.rpm"
extractCommand="rpm2cpio "$rpmPath/$rpmName" | cpio -idmv"
echo $extractCommand

eval "chmod 777 "$rpmPath/$rpmName
sleep 1
eval $extractCommand

echo "Copying gtest lib"
cp usr/bin/libgtest.a bin/tizen

echo "deleting tmp resources"
rm -rf usr/
rm -rf .git/

eval "chmod 777 -R bin/tizen"

cd ../..
