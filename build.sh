#!/bin/zsh
# Build BT60 V2 firmware into firmware/.
#   ./build.sh          ZMK Studio build (normal use)
#   ./build.sh debug    USB logging build (battery/diagnostics; read with ./log.sh). No Studio.
set -euo pipefail
HERE=${0:A:h}; WS=${HERE:h}/workspace
MODE=${1:-studio}
source "$WS/.venv/bin/activate"
export ZEPHYR_SDK_INSTALL_DIR=$(echo "$WS"/zephyr-sdk-*(N[1]))
export CMAKE_POLICY_VERSION_MINIMUM=3.5   # CMake 4 vs older modules in Zephyr 3.5
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
cd "$WS/zmk/app"
case $MODE in
  studio) west build -p auto -d "$WS/build/bt60_v2-studio" -b bt60_v2 -S studio-rpc-usb-uart -- -DZMK_CONFIG="$HERE/config" -DZMK_EXTRA_MODULES="$HERE" ;;
  debug)  west build -p auto -d "$WS/build/bt60_v2-debug" -b bt60_v2 -- -DZMK_CONFIG="$HERE/config" -DZMK_EXTRA_MODULES="$HERE" \
            -DCONFIG_ZMK_STUDIO=n -DCONFIG_ZMK_USB_LOGGING=y ;;
  *) echo "usage: $0 [studio|debug]"; exit 1 ;;
esac
mkdir -p "$HERE/firmware"
cp "$WS/build/bt60_v2-$MODE/zephyr/zmk.uf2" "$HERE/firmware/bt60_v2-$MODE.uf2"
echo "Built firmware/bt60_v2-$MODE.uf2"
