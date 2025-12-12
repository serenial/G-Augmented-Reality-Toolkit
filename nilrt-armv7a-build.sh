#!/usr/bin/env bash
cd "$(dirname "$0")"


cmake --preset=linux-armv7a-cross --trace && cmake --build --preset=linux-armv7a-cross-release-build --trace