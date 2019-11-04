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
      make

RUN ln -s /usr/bin/cmake3 /usr/bin/cmake

RUN yum clean all && \
      rm -rf /var/cache/yum

WORKDIR /opt
RUN git clone https://github.com/rucio/fuse-posix.git
RUN cd fuse-posix && \
			./build.sh
COPY docker-entrypoint.sh /
ENTRYPOINT ["/docker-entrypoint.sh"]
