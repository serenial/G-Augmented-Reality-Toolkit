#! /bin/sh

TOOLCHAIN_SCRIPT_SHA512="31833d2ddc49b18d740332d9c2e8680ab3834b0f0a8e5555511323d6d6a819f01cd43725ad6c79c1571e47a0b23fa8209c367d40bb90b755fc896d51cf41cd7d"

curl https://download.ni.com/support/softlib/labview/labview_rt/2018/Linux%20Toolchains/linux/oecore-x86_64-cortexa9-vfpv3-toolchain-6.0.sh -Lo install-tc.sh
echo "${TOOLCHAIN_SCRIPT_SHA512} install-tc.sh" | sha512sum --check --status
chmod +x ./install-tc.sh
sudo ./install-tc.sh -y
