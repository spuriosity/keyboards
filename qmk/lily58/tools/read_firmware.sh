#!/bin/zsh
# Read the firmware off the USB-connected half (read-only), then restart it.
#   tools/read_firmware.sh [out.bin]     then press that half's reset button
# Works now: the 2026-10-09 dfu-programmer flash left the Atmel DFU security bit clear. (The pre-2026 firmware
# had it set and refused reads.) Turn the dump back into a keymap with tools/extract_keymap.py.
setopt nullglob
OUT=${1:-dump.bin}
echo "Waiting for a half in bootloader mode..."
until ioreg -p IOUSB -l -w0 | grep -q '"idProduct" = 12276'; do sleep 0.5; done
sleep 1
dfu-programmer atmega32u4 read --bin > "$OUT" && echo "Saved $OUT ($(stat -f%z "$OUT") bytes)"
dfu-programmer atmega32u4 launch
