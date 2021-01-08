#!/bin/bash
MONGODB_URI=${1}
if [ -z ${MONGODB_URI} ]
then
    read -p "MONGODB URI (Required): " MONGODB_URI
fi 

DRIVER_VERSION=${2:-3.6.2}
echo "Executing ... "
docker run --rm -e MONGODB_URI=${MONGODB_URI} \
    -v "$(pwd)":/workspace \
    -w /workspace/cxx start-cxx \
    "c++ --std=c++11 ./getstarted.cpp -o getstarted -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi -L/usr/local/lib -lmongocxx -lbsoncxx; \
    ./getstarted"
