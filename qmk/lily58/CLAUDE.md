# Lily58: notes for Claude

splitkb **Aurora Lily58 rev1**, split, Elite-C controllers (ATmega32u4, Atmel DFU bootloader), QMK.
Read README.md first.

## Workflow

- `lily58.json` is the keymap, in QMK Configurator format. Edit it directly, or import it at
  <https://config.qmk.fm/#/splitkb/aurora/lily58/rev1/LAYOUT>, edit, and export it back over the file.
- `./build.sh` compiles through QMK's API (api.qmk.fm), no local toolchain needed. Output: `build/*.hex`.
- `./flash.sh` waits for each half in Atmel DFU and runs `dfu-programmer` erase/flash/launch (`brew install
  dfu-programmer`). Same .hex for both halves (handedness is a pin). The user presses the reset button (side of
  the PCB by the TRRS jack) on the USB-connected half, then moves the cable and does the other half.
- Keep each flashed .hex in `firmware/` (named by date and commit). `build/` is gitignored.

## Layout

- Layer 0 Colemak-DH, layer 1 QWERTY (same thumbs/mods), 2 symbols, 3 nav, 4 RGB + base switching.
- Switch base layout: hold left `Bspc` thumb + right `Space` thumb, then Q (QWERTY) or C (Colemak); saved
  persistently (`PDF()`). QWERTY must stay at layer 1, below the momentary layers, or it would cover them.
- Base was the 18 Jul 2023 Configurator export; the board's previous (unsaved) keymap differed on a few
  layer-0 keys, listed in README.md.

## Things learned

- The pre-2026 firmware had the Atmel DFU security bit set, so it couldn't be read back. The 2026-10-09
  dfu-programmer flash left it clear: `tools/read_firmware.sh` should now work, and `tools/extract_keymap.py`
  turns a dump back into Configurator JSON.
- No VIA/Vial in this build, so the keymap can't be read or changed live. Vial supports this board if wanted.
- Current QMK uses `RM_*` keycodes for this board's RGB matrix (the 2023 `RGB_*` ones were renamed).
