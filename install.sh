#!/bin/bash

# Check if a package is already installed
check_package() {
    if dpkg -s "$1" &> /dev/null; then
        echo "Package $1 is already installed"
        return 0
    else
        echo "Package $1 is not installed"
        return 1
    fi
}

# Install a package
install_package() {
    if sudo apt-get install -y "$1"; then
        echo "Successfully installed package $1"
        return 0
    else
        echo "Failed to install package $1"
        return 1
    fi
}

# Check and install a package
check_and_install_package() {
    check_package "$1"
    if [ $? -ne 0 ]; then
        install_package "$1"
    fi
}

# Check and install packages
check_and_install_package "gcc-arm-none-eabi"
check_and_install_package "libncursesw5"
check_and_install_package "putty"

# Add Repository
if ! grep -q "^deb .*ppa:deadsnakes/ppa" /etc/apt/sources.list /etc/apt/sources.list.d/*; then
    sudo add-apt-repository ppa:deadsnakes/ppa
    sudo apt update
fi

# Check and install packages
check_and_install_package "python3.8"
check_and_install_package "lsb-core"

# GNU toolchain for ARM 32 bit processor
GCC_VER=12.2.rel1
TOOLCHAIN=arm-none-eabi
TOOLCHAIN_URL=https://developer.arm.com/-/media/Files/downloads/gnu

# download ARM toolchain
wget -q \
    ${TOOLCHAIN_URL}/${GCC_VER}/binrel/arm-gnu-toolchain-${GCC_VER}-x86_64-${TOOLCHAIN}.tar.xz

tar Jxvf arm-gnu-toolchain-${GCC_VER}-x86_64-${TOOLCHAIN}.tar.xz
GDB_PATH=$(which arm-none-eabi-gcc)
sudo cp arm-gnu-toolchain-${GCC_VER}-x86_64-${TOOLCHAIN}/bin/${TOOLCHAIN}-gdb $GDB_PATH
rm -rf arm-gnu-toolchain-${GCC_VER}-x86_64-${TOOLCHAIN}.tar.xz arm-gnu-toolchain-${GCC_VER}-x86_64-${TOOLCHAIN}


# install OpenOCD from github
cd
git clone git://git.code.sf.net/p/openocd/code openocd
sudo apt-get install build-essential pkg-config autoconf automake libtool libusb-dev libusb-1.0-0-dev libhidapi-dev
sudo apt-get install libtool libsysfs-dev
cd openocd && ./bootstrap
./configure
make
sudo make install

# install stlink from github
cd
sudo apt-get install git cmake libusb-1.0-0-dev
git clone https://github.com/stlink-org/stlink && cd stlink
cmake .
make
cd bin && sudo cp st-* /usr/local/bin
cd ../lib && sudo cp *.so* /lib32
cd ../config/udev/rules.d/ && sudo cp 49-stlinkv* /etc/udev/rules.d/