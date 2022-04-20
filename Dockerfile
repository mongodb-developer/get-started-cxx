FROM alpine:3.13

LABEL org.opencontainers.image.source=https://github.com/mongodb-developer/get-started-cxx

ARG DRIVER_VERSION=3.6.6
ARG DRIVER_C_VERSION=1.21.1

RUN apk add --no-cache wget cmake make git tar gcc g++ musl-dev openssl-dev perl
RUN addgroup -S gsgroup && adduser -S gsuser -G gsgroup

ENV CDRIVER_VERSION ${DRIVER_C_VERSION}
ENV LD_LIBRARY_PATH /usr/local/lib64
ENV DRIVER_VERSION ${DRIVER_VERSION}
ENV HOME /home/gsuser

RUN cd ${HOME} && wget https://github.com/mongodb/mongo-c-driver/releases/download/${CDRIVER_VERSION}/mongo-c-driver-${CDRIVER_VERSION}.tar.gz && \
    tar xzf mongo-c-driver-${CDRIVER_VERSION}.tar.gz 

RUN cd ${HOME} && wget https://github.com/mongodb/mongo-cxx-driver/archive/r${DRIVER_VERSION}.tar.gz && \
    tar -xzf r${DRIVER_VERSION}.tar.gz

RUN chown -R gsuser ${HOME} && chmod -R 750 ${HOME}

RUN cd ${HOME}/mongo-c-driver-${CDRIVER_VERSION} && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DENABLE_TESTS=OFF .. && \    
    make && make install 

RUN cd ${HOME}/mongo-cxx-driver-r${DRIVER_VERSION}/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_VERSION=0.0.1 -DCMAKE_PREFIX_PATH=/usr/local -DENABLE_TESTS=OFF .. && \
    make EP_mnmlstc_core && \
    make && make install && \
    for i in `ls /usr/local/lib64/pkgconfig | grep cxx-static.pc`; do sed -i -e 's;libdir=${prefix}/lib;libdir=${prefix}/lib64;g' /usr/local/lib64/pkgconfig/$i; done && \
    apk --update --no-cache del perl

USER gsuser

ENTRYPOINT ["/bin/sh", "-c"]  
