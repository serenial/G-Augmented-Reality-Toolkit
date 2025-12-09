#!/usr/bin/env bash
cd "$(dirname "$0")"

bash vcpkg/bootstrap-vcpkg.sh

cmake --preset=linux-armv7-a-cross && cmake --build --preset=linux-armv7-a-cross-release-build