#!/bin/bash -e
echo "----------------------------------------"
echo "Activating rucio fuse-posix interface..."
/opt/fuse-posix/cmake-build-debug/bin/rucio-fuse-main
