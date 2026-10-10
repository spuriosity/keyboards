#!/bin/zsh
# Flash both Lily58 halves in turn: wait for Atmel DFU, erase + flash + launch, then wait for the next half.
# Same .hex for both halves (handedness comes from a pin). Needs: brew install dfu-programmer
#   ./flash.sh                 build/splitkb_aurora_lily58_rev1_lily58.hex (from ./build.sh)
#   ./flash.sh firmware/<file>.hex
# Enter the bootloader: press the reset button (side of the PCB, next to the TRRS jack) on the USB-connected half,
# or hold its top outer key while plugging USB in. Then move the cable to the other half and repeat.
HEX="${1:-${0:A:h}/build/splitkb_aurora_lily58_rev1_lily58.hex}"
in_dfu() { ioreg -p IOUSB -l -w0 | grep -q '"idProduct" = 12276' && ioreg -p IOUSB -l -w0 | grep -q '"idVendor" = 1003'; }
for half in 1 2; do
  echo "[$(date +%T)] Waiting for half $half in bootloader mode..."
  for i in {1..1800}; do in_dfu && break; sleep 0.5; done
  in_dfu || { echo "timed out waiting for half $half"; exit 2; }
  sleep 1
  echo "[$(date +%T)] Half $half found, flashing"
  dfu-programmer atmega32u4 erase --force || { echo "erase failed on half $half"; exit 3; }
  dfu-programmer atmega32u4 flash "$HEX" || { echo "FLASH FAILED on half $half (still in bootloader; safe to retry)"; exit 4; }
  dfu-programmer atmega32u4 launch
  sleep 3
  echo "[$(date +%T)] Half $half done. USB now shows: $(ioreg -p IOUSB -l -w0 | grep '"USB Product Name"' | grep -v Apple | sed 's/.*= //')"
  # wait until it has left the bootloader before looking for the next half
  for i in {1..60}; do in_dfu || break; sleep 0.5; done
done
echo "[$(date +%T)] Both halves flashed."
