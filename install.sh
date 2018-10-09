#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

echo 'Installing Avahi'
sudo apt install -y avahi-daemon libavahi-compat-libdnssd-dev

echo 'Compiling HAP library'
cd ${DIR}
mkdir -p hap-release
cd hap-release
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
sudo make install
cd ..