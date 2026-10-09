# Lily58 keymap

splitkb.com **Aurora Lily58 rev1**, Elite-C (ATmega32u4) controllers, QMK firmware.

| File | What it is |
|---|---|
| `lily58.json` | The working keymap. Import into [QMK Configurator](https://config.qmk.fm/#/splitkb/aurora/lily58/rev1/LAYOUT), edit, export back over this file. |
| `archive/` | Original 2023 Configurator exports, kept unchanged. |
| `tools/key-reader/` | Browser page that records what each physical key sends. Run `python3 server.py 8458` and open `http://localhost:8458/keytester.html`; results land in `results.json`. |

## Base

`lily58.json` started from the 18 Jul 2023 export (`archive/2023-07-18_configurator-export.json`) with the
layer 3 `RGB_*` keycodes renamed to `RM_*`, since current QMK drives this board's RGB matrix with those.

Layout: Colemak-DH or QWERTY (switchable), Ctrl-tap-Backspace on Caps, SpaceFn.

- Layer 0: Colemak-DH base
- Layer 1: QWERTY base (only the letters differ; thumbs, mods and layer keys match layer 0)
- Layer 2: symbols and F-keys (hold left `Bspc` thumb)
- Layer 3: navigation (hold right `Enter` or `Space` thumb)
- Layer 4: RGB and base switching (hold left `Bspc` thumb + right `Space` thumb)

### Switching base layout

Hold both layer thumbs (left `Bspc` + right `Space`), then tap:

- **Q** → QWERTY
- **C** → Colemak-DH (C is the same key in both layouts)

The choice is saved on the board (`PDF()`), so it survives unplugging.
QWERTY sits at layer 1, below the symbol/nav layers, so those still work on top of either base.

## Flashing

1. Configurator: **Compile**, then **Firmware** to download the `.hex`.
2. QMK Toolbox: open the `.hex`, tick **Auto-Flash**.
3. Press reset on the USB-connected half, then move the cable and do the other half.

The Elite-C's Atmel DFU bootloader has its security bit set, so firmware can't be read back off the board.
Keep this folder as the source of truth.

## Board vs. this file (Oct 2026)

The firmware on the board when this folder was created was a later, unsaved edit of the 2023 keymap.
A key-reader pass of layer 0 found these differences from `lily58.json`:

| Key | File | Board |
|---|---|---|
| Tab position | `Tab` | `Cmd` (unconfirmed) |
| Left inner (by V) | `Esc` | `` ` `` |
| Right inner (by K) | `` ` `` | `=` |
| Right Shift position | `RShift` | `=` (unconfirmed) |
| Left thumbs, outer to inner | `Alt` `Cmd` `Bspc/L1` `Cmd` | `Cmd` `Alt` `Cmd` `Bspc` |
| Right thumbs, inner to outer | `Enter/L2` `Space/L2` `Bspc` `=` | `Enter` `Space` `_` `=` |
