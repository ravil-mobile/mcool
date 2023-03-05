ARG UID=1000
ARG GID=1000

FROM ubuntu:20.04

ENV TZ=Europe/Berlin

RUN apt-get update -y && \
ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && \
apt-get install -y software-properties-common apt-utils build-essential wget git curl && \
apt-get install -y pkg-config cmake cmake-data tree vim && \
apt-get install -y python3 python3-dev python3-pip && \
apt-get install -y flex bison && \
add-apt-repository ppa:ubuntu-toolchain-r/test && \
apt-get install -y gcc g++

RUN export VERSION=1.12.0 && \
git clone --depth 1 --branch release-${VERSION} https://github.com/google/googletest && \
mkdir -p googletest/build && cd googletest/build && \
cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=ON -Dgtest_disable_pthreads=ON -DBUILD_SHARED_LIBS=ON && \
make -j $(nproc) && \
make install && \
cd ../.. && rm -rf ./googletest

RUN export VERSION=12 && \
wget https://apt.llvm.org/llvm.sh && \
chmod +x llvm.sh && \
./llvm.sh ${VERSION} && \
ln -s /usr/bin/llc-12 /usr/bin/llc && \
ln -s /usr/bin/opt-12 /usr/bin/opt && \
ln -s /usr/bin/clang-12 /usr/bin/clang && \
ln -s /usr/bin/clang++-12 /usr/bin/clang++ && \
apt-get -y install zlib1g-dev

RUN apt-get clean && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/*

COPY mcool /mcool
COPY examples /mcool-examples

RUN mkdir -p /mcool/build && cd /mcool/build && \
cmake .. -DWITH_TESTS=OFF && make -j $(nproc) && \
make install && cd / && rm -rf /mcool

ARG UID
ARG GID

RUN mkdir -p /home/user && \
echo "user:x:${UID}:${GID}:Developer,,,:/home/user:/bin/bash" >> /etc/passwd && \
echo "user:x:${UID}:" >> /etc/group && \
mkdir /etc/sudoers.d && chmod 0644 /etc/sudoers.d && \
echo "user ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/user && \
chmod 0440 /etc/sudoers.d/user && \
chown ${UID}:${GID} -R /home/user && mkdir -p /home/user/workspace

USER user
ENV HOME /home/user

WORKDIR /home/user/workspace
