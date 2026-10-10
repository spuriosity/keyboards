#!/bin/zsh
# Copy the firmware off the BT60 without flashing anything (flash.sh also does this before every flash).
#   tools/dump.sh      then hold right Ctrl + Backspace (or double-tap reset)
# Afterwards the board sits in the bootloader: run ./flash.sh (re-flashes the current build) or power-cycle it.
set -euo pipefail
HERE=${0:A:h:h}
echo "Waiting for the BT60 bootloader drive..."
until [[ -f /Volumes/CKP/INFO_UF2.TXT ]]; do sleep 0.5; done
grep -q 'Board-ID: nRF52840-rotr-v1' /Volumes/CKP/INFO_UF2.TXT || { echo "Not the BT60 bootloader"; exit 1; }
mkdir -p "$HERE/dumps"
OUT="$HERE/dumps/$(date +%Y-%m-%d_%H%M)_CURRENT.UF2"
cp -X /Volumes/CKP/CURRENT.UF2 "$OUT" && echo "Saved dumps/$(basename $OUT)"
