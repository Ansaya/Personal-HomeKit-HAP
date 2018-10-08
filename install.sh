#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

echo 'Installing Avahi'
sudo apt install -y avahi-daemon libavahi-compat-libdnssd-dev

echo 'Compiling HAP library'
chroot ${DIR}
mkdir hap-release
cd hap-release
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
sudo make install
cd ..
exit