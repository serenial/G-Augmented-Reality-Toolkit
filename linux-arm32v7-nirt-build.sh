#!/usr/bin/env bash
cd "$(dirname "$0")"

bash vcpkg/bootstrap-vcpkg.sh

cmake --preset=linux-arm32v7-cross && cmake --build --preset=linux-arm32v7-cross-release-build