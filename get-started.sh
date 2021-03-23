#!/bin/bash
MONGODB_URI=${1}
DOCKER_IMAGE=ghcr.io/mongodb-developer/get-started-cxx

if [ -z ${MONGODB_URI} ]
then
    read -p "MONGODB URI (Required): " MONGODB_URI
fi

echo "Executing ... "
docker run --rm -e MONGODB_URI=${MONGODB_URI} \
    -v "$(pwd)":/workspace \
    -w /workspace/cxx ${DOCKER_IMAGE} \
    "c++ --std=c++11 ./getstarted.cpp -o getstarted -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi -L/usr/local/lib64 -lmongocxx -lbsoncxx; \
    ./getstarted"
