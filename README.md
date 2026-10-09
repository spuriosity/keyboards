# BT60 keymap

CKP **BT60** (60%, nRF52840, wireless), ZMK firmware. Bootloader: Adafruit UF2 (board ID `nRF52840-rotr-v1`),
which mounts as a drive called `CKP`.

| File | What it is |
|---|---|
| `bt60.keymap` | The keymap, reconstructed from the firmware on the board on 2026-10-10. Matches the board exactly. |
| `dumps/` | `CURRENT.UF2` and `INFO_UF2.TXT` copied off the board's bootloader drive on 2026-10-10. |
| `archive/` | Older copies: an older `CURRENT.UF2` backup (byte-identical to the 2026 dump) and a 2025 keymap edit with syntax errors that was never built. |

The firmware is ZMK on Zephyr 3.0 (2022 era). There's no ZMK Studio, so the keymap is compiled in.

## Layers

- **Base**: QWERTY. Esc on tap, `` ` `` on hold (top-left). Bottom row: Ctrl, Alt, Cmd, Space, RAlt, Fn, Menu, RCtrl.
- **RAISE** (hold Fn): F-keys, Bluetooth profiles, media, backlight and RGB. `Fn`+`Enter` enters the bootloader.
- **NAV** (hold Space): arrows on `J K L` / `I`, word jump (`Alt+←/→`) on `U` / `O`, Page Up/Down and Home/End around them,
  `Cmd+`` ` on Tab, `Cmd+[` / `Cmd+]` on `N` / `M`.

## Reading the firmware back

1. `Fn`+`Enter` (or double-tap reset). A drive called `CKP` mounts.
2. Copy `CURRENT.UF2` off it. Nothing on the board changes.
3. Eject the drive and press reset once to go back to normal.

## Differences from the 2025 archive keymap

- Top-left key: board has `&mt GRAVE ESC`; the 2025 file had `&kp ESC`.
- Bottom row: board has Ctrl, **Alt, Cmd** (Mac order); the 2025 file had Ctrl, Cmd, Alt.
- NAV layer: the board's version is fuller (Page/Home/End, word jump, `Cmd+`` ` and `Cmd+[ ]`). The 2025 file only had arrows.
- The reset key is `&sys_reset`, its name in current ZMK. The 2025 file used the old `&reset`.
