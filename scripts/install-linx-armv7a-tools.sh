#! /bin/sh

sudo apt install -y gcc-arm-linux-gnueabi g++-arm-linux-gnueabi

PKG_SHA512=53f3f935fd7d8d753d6b7fc5c1f3122a3c487a5206f0a38f689bf27a0a3b4841c6256120e786f7cda803950f74e8535fd386a39f1ae8809b39d6cfb7cf67e51d

curl https://feeds.labviewmakerhub.com/debian/binary/lvrt20-schroot_20.0.0-4.deb -Lo trixie.img.xz lvrt-20.deb
echo "${PKG_SHA512} lvrt-20.deb" | sha512sum --check --status
dpkg -x lvrt-20.deb ~/lvrt-20 