FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ARG DRIVER_VERSION=3.6.2
ARG DRIVER_C_VERSION=1.17.3
ARG MONGODB_URI

RUN apt-get update && apt-get install -y \
	nano \
	sudo \
    build-essential \
    wget \
    cmake \
    git \
    python3.9 \
    python3.9-distutils \
    pkg-config \
    libssl-dev \
    libsasl2-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN export uid=1000 gid=1000 && \
    mkdir -p /home/ubuntu && mkdir /workspace && \
    echo "ubuntu:x:${uid}:${gid}:Developer,,,:/home/ubuntu:/bin/bash" >> /etc/passwd && \
    echo "ubuntu:x:${uid}:" >> /etc/group && \
    echo "ubuntu ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/ubuntu && \
    chmod 0440 /etc/sudoers.d/ubuntu && \
    chown ${uid}:${gid} -R /home/ubuntu

ENV HOME /home/ubuntu
ENV WORKSPACE /workspace
ENV CDRIVER_VERSION ${DRIVER_C_VERSION}
ENV LD_LIBRARY_PATH /usr/local/lib
ENV DRIVER_VERSION ${DRIVER_VERSION}
ENV MONGODB_URI ${MONGODB_URI}

WORKDIR ${HOME}

RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/${CDRIVER_VERSION}/mongo-c-driver-${CDRIVER_VERSION}.tar.gz && \
    tar xzf mongo-c-driver-${CDRIVER_VERSION}.tar.gz 

RUN cd ${HOME}/mongo-c-driver-${CDRIVER_VERSION} && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. && \
    make && make install 

RUN cd ${HOME}

RUN wget https://github.com/mongodb/mongo-cxx-driver/archive/r${DRIVER_VERSION}.tar.gz && \
    tar -xzf r${DRIVER_VERSION}.tar.gz

RUN cd ${HOME}/mongo-cxx-driver-r${DRIVER_VERSION}/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_VERSION=0.0.1 -DCMAKE_PREFIX_PATH=/usr/local .. && \
    make EP_mnmlstc_core && \
    make && make install 

RUN mkdir ${HOME}/cxx
COPY ./cxx/getstarted.cpp ${HOME}/cxx/getstarted.cpp

RUN chown -R ubuntu ${HOME}/cxx && chmod -R 750 ${HOME}/cxx

USER ubuntu

WORKDIR ${WORKSPACE}/cxx

ENTRYPOINT ["/bin/bash", "-c"]  
