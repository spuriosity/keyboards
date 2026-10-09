#!/bin/zsh
# Flash a .uf2 to the BT60: waits for its bootloader drive (hold right Ctrl + Backspace, or double-tap reset), checks it's the
# BT60's bootloader, then copies the file. The board reboots into the new firmware by itself.
#   ./flash.sh                         firmware/bt60_v2-studio.uf2
#   ./flash.sh firmware/bt60_v2-debug.uf2
#   ./flash.sh dumps/2026-10-10_CURRENT.UF2   (put the 2022 firmware back)
set -euo pipefail
HERE=${0:A:h}
UF2=${1:-$HERE/firmware/bt60_v2-studio.uf2}
[[ -f $UF2 ]] || { echo "No such file: $UF2 (run ./build.sh first)"; exit 1; }
echo "Waiting for the BT60 bootloader drive... (hold right Ctrl + Backspace, or double-tap reset)"
for i in {1..600}; do
  [[ -f /Volumes/CKP/INFO_UF2.TXT ]] && break; sleep 0.5
done
[[ -f /Volumes/CKP/INFO_UF2.TXT ]] || { echo "Timed out"; exit 2; }
grep -q 'Board-ID: nRF52840-rotr-v1' /Volumes/CKP/INFO_UF2.TXT || { echo "Drive CKP is not the BT60 bootloader:"; cat /Volumes/CKP/INFO_UF2.TXT; exit 3; }
echo "Copying $(basename $UF2)..."
cp -X "$UF2" /Volumes/CKP/ 2>/dev/null || true   # the drive disconnects mid-copy when the board reboots; that's normal
sleep 3
ioreg -p IOUSB -l -w0 | grep -q '"USB Product Name" = "BT60' && echo "Done: BT60 is back on USB." || echo "Copied. If the keyboard doesn't come back, press reset once."
