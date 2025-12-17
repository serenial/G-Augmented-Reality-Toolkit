! /bin/sh

sudo apt install -y xz-utils gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

IMG_SHA512="89f9a96531533de65626686cfb0ff579f6fc823db35b80f198297fa8bfe2daa432564755b605dc32f04c2639406be0e08c770cfa934872ae76360389cc7b1f3d"
MOUNTPI_SHA512="b7a8186e1dffe55639c4b941ef4c8abce87c522d824bc59d09b621718eec12ed66941ba7292fc79810e07954c2983e258879db7e1b5f14290e305d6bb48ebbf2"

curl https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2025-12-04/2025-12-04-raspios-trixie-arm64-lite.img.xz -Lo trixie.img.xz
echo "${IMG_SHA512} trixie.img.xz" | sha512sum --check --status
unxz trixie.img.xz

curl https://github.com/novamostra/mountpi/archive/f7fa1e8cc7c86217a0acdb70e5eff00aab885bd4.zip -Lo mountpi.zip
echo "${MOUNTPI_SHA512} mountpi.zip" | sha512sum --check --status
unzip -j mountpi.zip -d mountpi
chmod +x mountpi/mountpi.sh
mkdir ~/trixie-mnt
sudo mountpi/mountpi.sh -i trixie.img -m ~/trixie-mnt -p 1

