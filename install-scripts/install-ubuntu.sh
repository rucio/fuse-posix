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
echo "Perfoming Rucio FUSE module setup and installation"
echo
echo -e "\033[1;33m 1. Configuring and updating apt\033[0m"
echo
add-apt-repository ppa:ubuntu-toolchain-r/test -y
apt-get update -y
echo
echo -e "\033[1;33m 2. Installing dependencies\033[0m"
echo
apt-get install -y fuse
apt-get install -y gcc-8 g++-8 make cmake python3 python-pip
apt-get install -y libfuse libcurl
apt-get install -y globus-proxy-utils
#TODO: xrootd-libs xrootd-client gfal2 gfal2-util gfal2-all gfal2-python3
echo
echo -e "\033[1;33m 4. Setting up python3\033[0m"
echo
pip3 install --upgrade pip setuptools wheel
pip3 install rucio-clients
echo
echo -e "\033[1;33m 5. Building\033[0m"
echo
bash ${DIR}/../build.sh
echo
echo -e "\033[1;33m 6. Starting fuse\033[0m"
echo
modprobe fuse
echo
echo -e "\033[1;33m/////////////////////////////////////////////////////////////////////////////////////////////////////////////////\033[0m"
echo
