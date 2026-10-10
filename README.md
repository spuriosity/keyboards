# BT60 keymap

Polarity Works **BT60 V2** (60% ANSI, CKP controller, nRF52840, wireless), ZMK firmware. ZMK board: `bt60_v2`.
Bootloader: Adafruit UF2 (board ID `nRF52840-rotr-v1`), which mounts as a drive called `CKP`.

| File | What it is |
|---|---|
| `config/bt60_v2.keymap` | The keymap, reconstructed from the board's firmware on 2026-10-10, plus `Fn`+`U` to unlock ZMK Studio and a SYS layer on right Ctrl. |
| `config/bt60_v2.conf` | Build options: ZMK Studio, deep sleep after 15 min, lights off when idle. |
| `src/`, `dts/`, `zephyr/`, `CMakeLists.txt`, `Kconfig` | Custom `&bat_report` behavior that types the battery level. |
| `tools/keylog/` | Browser key logger (`python3 server.py 8459`, open `http://localhost:8459`). |
| `build.sh` / `flash.sh` / `log.sh` | Build firmware, flash it, read the debug log. |
| `firmware/` | Built `.uf2` files. |
| `dumps/` | `CURRENT.UF2` and `INFO_UF2.TXT` copied off the board's bootloader drive on 2026-10-10. |
| `archive/` | Older copies: an older `CURRENT.UF2` backup (byte-identical to the 2026 dump) and a 2025 keymap edit with syntax errors that was never built. |

The original firmware (in `dumps/`) is ZMK on Zephyr 3.0 (2022 era), without ZMK Studio. New builds use ZMK v0.3 with Studio.

## Editing the keymap visually (ZMK Studio)

Once the Studio firmware is on the board:

1. Plug the BT60 in over USB.
2. Open <https://zmk.studio> in Arc (it needs a Chromium browser for Web Serial) and connect to the BT60.
3. Hold `Fn` and tap `U` to unlock the keyboard for editing.
4. Change keys and click **Save**. Changes apply immediately and survive restarts. No rebuild needed.

Studio stores its edits on the keyboard, not in `config/bt60_v2.keymap`. To make the file the source of truth again,
use **Restore Stock Settings** in Studio after flashing a new build.

## Building and flashing

The shared build environment lives in `../workspace` (ZMK v0.3, Zephyr 3.5, Zephyr SDK). Set it up once with
`./setup-workspace.sh` (it lives here but builds the shared `../workspace`, which git ignores).

```
./build.sh            # Studio firmware  -> firmware/bt60_v2-studio.uf2
./flash.sh            # then hold right Ctrl + Backspace (or double-tap reset); copies the .uf2 onto the CKP drive
                      # also the way out of bootloader mode: no keys work there
./build.sh debug      # USB-logging firmware for diagnostics (no Studio)
./build.sh battest    # battery discharge test (see Battery below)
./flash.sh firmware/bt60_v2-debug.uf2 && ./log.sh
./flash.sh dumps/2026-10-10_CURRENT.UF2   # put the original 2022 firmware back
./flash.sh firmware/2026-10-10_3edb05c_bt60_v2-studio.uf2   # the build before the battery test (git tag pre-battest)
```

## Battery

The board has a battery voltage sensor (resistor divider into the nRF52840's ADC, `vbatt` in ZMK's `ckp.dtsi`).
Hold right Ctrl + tap right Shift to type the level and voltage. A charged cell reads about 3700-4200 mV; on USB
the charger can make a missing or dead cell read high, so test unplugged over Bluetooth. ZMK also reports the level
to paired hosts over Bluetooth. First reading (2026-10-10, on USB): `94% 4148mV`.

### Discharge test (battery health)

The board has no fuel gauge, only a voltage sensor, so health shows up in the discharge curve.

1. `./build.sh battest && ./flash.sh firmware/bt60_v2-battest.uf2`, then charge fully.
2. Unplug. A fresh log starts: a voltage reading every 10 minutes, saved to flash after each one (it survives the
   battery running flat). Deep sleep and idle light-off are off and the underglow stays on as a steady load.
3. Plug back in (logging stops). Don't unplug again first: that starts a fresh log.
   Hold right Ctrl + tap Enter in a text box to type the log (in the battest build only).
4. Flash the normal firmware back. Results so far are in `battery-tests/`.

The stock BT60 V2 config never sleeps and turns the underglow on at start-up, which drains the battery.
`config/bt60_v2.conf` turns on deep sleep (15 min idle, any key wakes) and idle light-off.

## Layers

- **Base**: QWERTY. Esc on tap, `` ` `` on hold (top-left). Bottom row: Ctrl, Alt, Cmd, Space, RAlt, Fn, Menu, RCtrl.
- **RAISE** (hold Fn): F-keys, Bluetooth profiles, media, backlight and RGB. `Fn`+`U` unlocks ZMK Studio.
- **SYS** (hold right Ctrl, which is no longer a Ctrl key): `1`-`5` = Bluetooth device 1-5, Backspace = bootloader,
  `\` = next Bluetooth device,
  right Shift = type the battery level (e.g. `94% 4148mV`), Enter = switch typing between USB and Bluetooth
  (e.g. to type over Bluetooth while charging).
- **NAV** (hold Space): arrows on `J K L` / `I`, word jump (`Alt+←/→`) on `U` / `O`, Page Up/Down and Home/End around them,
  `Cmd+`` ` on Tab, `Cmd+[` / `Cmd+]` on `N` / `M`.

## Reading the firmware back

1. Hold right Ctrl + Backspace (or double-tap reset). A drive called `CKP` mounts.
   (The original 2022 firmware uses `Fn`+`Enter` instead.)
2. Copy `CURRENT.UF2` off it. Nothing on the board changes.
3. Eject the drive and press reset once to go back to normal.

## Differences from the 2025 archive keymap

- Top-left key: board has `&mt GRAVE ESC`; the 2025 file had `&kp ESC`.
- Bottom row: board has Ctrl, **Alt, Cmd** (Mac order); the 2025 file had Ctrl, Cmd, Alt.
- NAV layer: the board's version is fuller (Page/Home/End, word jump, `Cmd+`` ` and `Cmd+[ ]`). The 2025 file only had arrows.
- The reset key is `&sys_reset`, its name in current ZMK. The 2025 file used the old `&reset`.
