#! /bin/sh

TOOLCHAIN_SCRIPT_SHA256="550a2f7353d40bd0d99209c09bb99508f0d252148e53d9f66a6b827433135ad2"

curl https://github.com/j-medland/generic-armv7a-open-embedded/releases/download/2.7-Warrior-1.0/oecore-x86_64-armv7a-vfp-toolchain-nodistro.0.sh -Lo install-tc.sh
echo "${TOOLCHAIN_SCRIPT_SHA256} install-tc.sh" | sha256sum --check --status
chmod +x ./install-tc.sh
sudo ./install-tc.sh -y
