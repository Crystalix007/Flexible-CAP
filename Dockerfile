FROM michaelkuc6/tup:release-fuse

RUN apt-get install -y git clang bison flex cmake libboost-dev libc++-dev
WORKDIR /tmp
RUN git clone https://github.com/no1msd/mstch.git && cd mstch && mkdir build && cd build && cmake .. && make && make install
COPY . fcap
WORKDIR fcap/generator
RUN git clean -fXd
RUN ./build.sh && cp fcap /usr/bin
WORKDIR /home
ENTRYPOINT [ "fcap" ]
