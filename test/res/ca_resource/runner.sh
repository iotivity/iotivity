cd /data/local/tmp
LD_LIBRARY_PATH=. ./$1 '--gtest_filter=*'$2
