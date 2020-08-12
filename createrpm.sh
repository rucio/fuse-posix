#!/usr/bin/env bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

CMAKE_EXE=cmake
CMAKE_VERSION=$(${CMAKE_EXE} --version | tr -d "cmake version")
CMAKE_VERSION_ARR=($(echo $CMAKE_VERSION | tr '.' "\n"))
CMAKE_MAIN_VERSION=${CMAKE_VERSION_ARR[0]}

if [ ${CMAKE_MAIN_VERSION} -lt 3 ]
then
    echo "CMake >= 3 required. Aborting."
else
    echo
    echo -e "\033[1;33m/////////////////////////////////////////////////////////////////////////////////////////////////////////////////\033[0m"
    echo
    echo "Building the Rucio posix-fuse interface and creating the rpm package."
    echo -e "\
    Please note that this requires
           - cmake>=3
           - libcurl (yum install libcurl-devel)
           - libfuse (yum install fuse-devel)

    To install them please run the following command:

        yum install -y cmake3 libcurl-devel libfuse-devel
        apt-get install cmake3 libfuse-dev libcurl4-gnutls-dev

    Or the corresponding package manager invocation.
    "

    echo
    echo -e "\033[1;33m-> Updating git submodules \033[0m"
    (cd ${DIR}; git submodule update --init --recursive)
    (cd ${DIR}; git submodule update --recursive --remote)

    echo
    echo -e "\033[1;33m-> Creating build directory: $DIR/cmake-build-debug \033[0m"
    mkdir -p ${DIR}/cmake-build-debug

    WORKDIR="$DIR/cmake-build-debug"

    echo
    echo -e "\033[1;33m-> Configuring with CMake \033[0m"
    (cd ${WORKDIR}; ${CMAKE_EXE} ..)

    echo
    echo -e "\033[1;33m-> Creating RPM with CPack \033[0m"
    (cd ${WORKDIR}; cpack)

    echo
    echo -e "\033[1;33m/////////////////////////////////////////////////////////////////////////////////////////////////////////////////\033[0m"
    echo
fi