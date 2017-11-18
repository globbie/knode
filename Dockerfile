FROM debian:9

ADD . /root/src

RUN apt-get update && apt-get -y install \
        gcc \
        cmake \
        make \
        libevent-dev

RUN cd /root/src && mkdir -p build && cd build && cmake .. && make
