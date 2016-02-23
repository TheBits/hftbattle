#!/bin/bash

## g++4.9
echo "Installing g++4.9 ..."
set -e
postfix=".2-10_amd64.deb"
pkgslist="cpp-4.9_4.9 g++-4.9_4.9 gcc-4.9_4.9 gcc-4.9-base_4.9 libasan1_4.9 libatomic1_4.9 libcilkrts5_4.9 libgcc1_4.9 libgcc-4.9-dev_4.9 libgomp1_4.9 libitm1_4.9 liblsan0_4.9 libquadmath0_4.9 libstdc++-4.9-dev_4.9 libstdc++6_4.9
libtsan0_4.9 libubsan0_4.9"
packages=""
for item in $pkgslist; do packages+="$item$postfix "; done
echo "Downloading packages ..."
for package in $packages; do
    if ! [ -f  "$package" ]; then
        wget "http://ftp.us.debian.org/debian/pool/main/g/gcc-4.9/$package";
    fi
done
wget "http://ftp.us.debian.org/debian/pool/main/b/binutils/binutils_2.25-5_amd64.deb"
wget "http://launchpadlibrarian.net/198723917/libc6_2.19-0ubuntu6.6_amd64.deb"
packages="binutils_2.25-5_amd64.deb libc6_2.19-0ubuntu6.6_amd64.deb $packages"
echo "Packages downloaded successfully."
echo "Installing ..."
sudo dpkg --install $packages
sudo apt-get install -y libmpfr4
echo "Packages installed."
echo "Cleaning ..."
for package in $packages; do rm -f $package; done
echo "Updating g++/gcc alternatives..."
set +e
sudo update-alternatives --remove-all g++
sudo update-alternatives --remove-all gcc
set -e
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 10
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 10
echo "g++4.9 - ok"


## cmake
echo "Installing CMake ..."
sudo apt-get install cmake
echo "CMake - ok"
