#!/bin/bash -e
echo "----------------------------------------"
echo "Activating rucio fuse-posix interface..."
/fuse-posix/cmake-build-debug/bin/rucio-fuse-main
