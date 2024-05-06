
set -x

rm -rf `pwd`/build/*
rm -rf `pwd`/lib/*
cd `pwd`/build &&
        cmake ..&&
        make
cd ..
cp -r `pwd`/src/include  `pwd`/lib
