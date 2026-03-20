echo "Running test $1"
cd cmake-build-debug
cmake --build .
cd ..
./cmake-build-debug/$1