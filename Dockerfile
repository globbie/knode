FROM debian:9

RUN apt-get update && apt-get -y install \
        gcc \
        cmake \
        make \
        libevent-dev \
        gdb \
        valgrind \
        pkg-config

ADD . /root/src

RUN cd /root/src && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make
