#!/bin/zsh
# One-time setup of the shared ZMK build workspace (ZMK v0.3 + Zephyr 3.5 + Zephyr SDK) for all boards in this folder.
set -euo pipefail
ZMK_DIR=${0:A:h:h}   # ~/dev/keyboards/zmk; the workspace is shared by all ZMK boards
WS="$ZMK_DIR/workspace"
ZMK_REF=v0.3-branch
SDK_VER=0.16.9
mkdir -p "$WS"; cd "$WS"

[[ -d .venv ]] || /opt/homebrew/bin/python3.12 -m venv .venv
source .venv/bin/activate
pip install -q --upgrade pip west

[[ -d zmk ]] || git clone -q --depth 1 --branch "$ZMK_REF" https://github.com/zmkfirmware/zmk.git zmk
cd zmk
[[ -d .west ]] || west init -l app
west update --narrow --fetch-opt=--depth=1
west zephyr-export
pip install -q -r zephyr/scripts/requirements-base.txt
pip install -q "setuptools<81" protobuf grpcio-tools   # nanopb (ZMK Studio) needs these

# Zephyr SDK: minimal bundle + the ARM toolchain only
cd "$WS"
if [[ ! -d "zephyr-sdk-$SDK_VER" ]]; then
  curl -sSL -o sdk.tar.xz "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v$SDK_VER/zephyr-sdk-${SDK_VER}_macos-aarch64_minimal.tar.xz"
  tar xf sdk.tar.xz && rm sdk.tar.xz
  command -v wget >/dev/null || brew install wget   # the SDK setup needs wget
  cd "zephyr-sdk-$SDK_VER" && ./setup.sh -t arm-zephyr-eabi -c
fi
echo "Workspace ready: $WS"
