#!/bin/zsh
# Show the debug firmware's USB log (battery readings etc.). Ctrl-C to stop.
setopt nullglob
ports=(/dev/cu.usbmodem*)
[[ -n ${ports[1]:-} ]] || { echo "No USB serial port. Is the debug firmware (./build.sh debug) flashed?"; exit 1; }
echo "Reading ${ports[1]}"
exec cat ${ports[1]}
