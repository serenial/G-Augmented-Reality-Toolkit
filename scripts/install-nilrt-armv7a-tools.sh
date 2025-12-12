#! /bin/sh

TOOLCHAIN_SCRIPT_SHA512="bfa9f07d8a024aa742eab81be4815156fec19f83aec35955cd8e559635f0503a481f6842ae65b6bd7d618a4dd177c1af9d9de4fa9f0fcf7f673263ae93c13239"
OPKG_UTILS_TAR_SHA512="441ee5ed416c3565617ae5fc413846ebc53e33876f9cce5e721afef2b8d9cd68723231ea12c8a2effaba2ccb33a36dc6e180994a57e6871f34d671c03ab36b7b"

curl https://download.ni.com/support/softlib/labview/labview_rt/2018/Linux%20Toolchains/linux/oecore-x86_64-core2-64-toolchain-6.0.sh -Lo install-tc.sh
echo "${TOOLCHAIN_SCRIPT_SHA512} install-tc.sh" | sha512sum --check --status
chmod +x ./install-tc.sh
sudo ./install-tc.sh -y


curl https://git.yoctoproject.org/opkg-utils/snapshot/opkg-utils-0.7.0.tar.gz -Lo opkg-utils.tar.gz
echo "${OPKG_UTILS_TAR_SHA512} opkg-utils.tar.gz" | sha512sum --check --status
mkdir opkg-utils
tar -zxf opkg-utils.tar.gz -C opkg-utils --strip-components=1
cd opkg-utils
make
sudo make install

