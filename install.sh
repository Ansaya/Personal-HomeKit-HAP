#!/bin/sh

echo 'Installing Avahi'
sudo apt install -y avahi-daemon libavahi-compat-libdnssd-dev

echo 'Compiling HAP library'
mkdir hap-release
cd hap-release
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make -j4
sudo make install
cd ..