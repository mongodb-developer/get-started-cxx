# Get-Started C++

Repository to help getting started with MongoDB C++ driver connecting to MongoDB Atlas.

## Information

This Get-Started project uses [MongoDB C++ driver](https://docs.mongodb.com/drivers/cxx) version 3.6.2 by default. Although you can change the driver version, the provided code example was only tested against the default version of MongoDB driver. There is no guarantee that the code sample will work for all possible versions of the driver.

## Pre-requisites 

### Docker 

Have Docker running on your machine. You can download and install from: https://docs.docker.com/install/

### MongoDB Atlas

In order to execute the code example, you need to specify `MONGODB_URI` environment variable to connect to a MongoDB cluster. If you don't have any you can create one by signing up [MongoDB Atlas Free-tier M0](https://docs.atlas.mongodb.com/getting-started/). 


##  Execution Step

Execute the helper shell script followed by the MongoDB URI that you would like to connect to. 
```
./get-started.sh "mongodb+srv://usr:pwd@example.mongodb.net/dbname?retryWrites=true"
```

## Changing The Driver Version 

The Docker image has been built with a specific version. If you would like to use a different version, you could build a new image. Example: 

```
docker build . -t start-cxx --build-arg DRIVER_VERSION=3.6.0 
```

This will build a docker image with a tag name `start-cxx`. 
Open the helper script `get-started.sh` in an editor of your choice, and change the value of `DOCKER_IMAGE` to `start-cxx` and save the file. Execute the helper script followed by the MongoDB URI. 


## Tutorials

* [MongoDB C++ driver documentation](http://mongocxx.org/mongocxx-v3/tutorial/)

## About 

This project is part of the MongoDB Get-Started code examples. Please see [get-started-readme](https://github.com/mongodb-developer/get-started-readme) for more information. 
