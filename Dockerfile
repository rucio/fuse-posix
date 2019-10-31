# Rucio: fuse-posix development container
# (because James is too lazy to install packages locally)
# Author:
# - James Alexander Clark, <james.clark@ligo.org>, 2019

FROM centos:7

LABEL name="Rucio: fuse-posix" \
      maintainer="James Alexander Clark <james.clark@ligo.org>" \
      date="20191031"

RUN yum install -y epel-release.noarch
RUN yum upgrade -y

# Yum dependencies
RUN yum install -y cmake3 \
      gcc-c++ \
      git \
      fuse-devel \
      libcurl-devel \
      make \
      vim

RUN ln -s /usr/bin/cmake3 /usr/bin/cmake

RUN yum clean all && \
      rm -rf /var/cache/yum

# Build fuse-posix from source
RUN mkdir fuse-posix
COPY .gitmodules build.sh CMakeLists.txt main.cxx fuse-posix/
RUN mkdir fuse-posix/CMake fuse-posix/include fuse-posix/source fuse-posix/submodules fuse-posix/tests fuse-posix/.git
COPY .git/ fuse-posix/.git/
COPY CMake/ fuse-posix/CMake/
COPY include/ fuse-posix/include/
COPY submodules/ fuse-posix/submodules/
COPY source/ fuse-posix/source/
COPY tests/ fuse-posix/tests/

RUN cd fuse-posix && \
			./build.sh
COPY docker-entrypoint.sh /
ENTRYPOINT ["/docker-entrypoint.sh"]
