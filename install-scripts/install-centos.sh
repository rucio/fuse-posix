#!/usr/bin/env bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

echo
echo -e "\033[1;33m/////////////////////////////////////////////////////////////////////////////////////////////////////////////////\033[0m"
echo
echo "Perfoming Rucio FUSE module setup and installation for CentOS"
echo
echo -e "\033[1;33m 1. Configuring and updating yum\033[0m"
echo
yum install -y epel-release
yum update -y
echo
echo -e "\033[1;33m 2. Installing dependencies\033[0m"
echo
yum install -y fuse
yum install -y gfal2 gfal2-util gfal2-all gfal2-python3
yum install -y gcc-c++ make cmake3 python3 python-pip
ln -s /usr/bin/cmake3 /usr/bin/cmake
yum install -y fuse-devel libcurl-devel
yum install -y xrootd-libs xrootd-client globus-proxy-utils
echo
echo -e "\033[1;33m 3. Switching to gcc8\033[0m"
echo
yum install -y centos-release-scl scl-utils
yum install -y devtoolset-8-gcc devtoolset-8-gcc-c++
echo
echo -e "\033[1;33m 4. Setting up python3\033[0m"
echo
pip3 install --upgrade pip setuptools wheel
pip3 install rucio-clients
echo
echo -e "\033[1;33m 5. Building\033[0m"
echo
scl enable devtoolset-8 ${DIR}/../build.sh
echo
echo -e "\033[1;33m 6. Starting fuse\033[0m"
echo
modprobe fuse
echo
echo -e "\033[1;33m/////////////////////////////////////////////////////////////////////////////////////////////////////////////////\033[0m"
echo
