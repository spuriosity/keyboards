# BT60: notes for Claude

Polarity Works **BT60 V2**, 60% ANSI, CKP controller (nRF52840), wireless. ZMK v0.3, board `bt60_v2`,
physical layout `layout_60_ansi` (61 keys, rows of 14/14/13/12/8). Bootloader: Adafruit UF2, drive `CKP`,
Board-ID `nRF52840-rotr-v1`. Read README.md (workflow) and LAYOUT.md (every key) first.

## Workflow

```
./setup-workspace.sh   # once: ZMK v0.3 + Zephyr 3.5 + SDK 0.16.9 into ../workspace (shared, gitignored)
./build.sh             # normal firmware (ZMK Studio)  -> firmware/bt60_v2-studio.uf2
./flash.sh             # then hold right Ctrl + Backspace; backs up CURRENT.UF2 to dumps/, then flashes
python3 tools/layout.py   # regenerate the diagrams in LAYOUT.md after any keymap change
```

Run `./flash.sh` in the background and tell the user which keys to press (they do the physical steps). After flashing, commit the new
`dumps/*_CURRENT.UF2` backup. Commit keymap/config changes before flashing.

## Layer conventions

- **Right Ctrl = keyboard control layer (SYS).** Every keyboard control lives there and nowhere else: Bluetooth
  (1-5 select, `]`/`\` prev/next, Esc clear), Enter = USB/Bluetooth, Backspace = bootloader, right Shift = battery
  report, U = Studio unlock, K/L = light on/off. Right Ctrl is not a Ctrl key any more.
- **Space = NAV (SpaceFn).** Number row F1-F12 (mod-morph: with Shift, the Mac actions as media keys); left letters
  F13-F24 (for Raycast bindings); right hand navigation; Space + Backspace = Delete.
- **Fn = RAISE.** F-keys, media, Caps Lock, PC keys. No Bluetooth or LED keys.
- Caps Lock key = `&ctl_bspc` hold-tap (tap Backspace, hold Ctrl, quick-tap repeats).
- No press-together combos (the timing proved unreliable in practice). No LED brightness/colour controls.

## Things learned the hard way

- In bootloader mode no keys work. Get out by flashing (`./flash.sh` re-flashes the current build) or power-cycling.
- Build env: CMake 4 needs `ZEPHYR_TOOLCHAIN_VARIANT=zephyr` (set in build.sh); nanopb (Studio) needs
  `setuptools<81`, `protobuf`, `grpcio-tools`; the SDK installer needs `wget`. All handled in setup-workspace.sh.
- The battery driver (`zmk,battery-voltage-divider`) only supports `SENSOR_CHAN_GAUGE_*` channels.
- `zmk_behavior_queue_add` has a bounded queue (`CONFIG_ZMK_BEHAVIORS_QUEUE_SIZE`, set to 160): typing long text
  must stay under it or be batched (see `src/battery_log.c`).
- Deep sleep (15 min) is System OFF: RAM lost, Bluetooth off, any key wakes it (that key isn't typed). The active
  Bluetooth profile is saved to flash 60 s after switching, so it reconnects to the same device.
- macOS turns plain F-keys into brightness/volume via Karabiner's virtual keyboard unless "Use F1, F2, etc. as
  standard function keys" is on (globally or per device in Karabiner). Not changed yet; ask before changing it.
- The stock board config never sleeps and turns the underglow on at start; config/bt60_v2.conf fixes that.
- Key tests: Karabiner remaps some keys (Tab held = Hyper), so a browser key logger sees post-Karabiner events.
  `tools/keylog/` is a logger page (`python3 server.py 8459`).

## Custom code

- `src/behavior_battery_report.c` (`&bat_report`): types `94% 4148mV, typing over usb, plugged in`.
- `src/battery_log.c` (`&bat_log`): discharge logger, active only in `./build.sh battest`
  (see README › Battery). First test: battery-tests/2026-10-10_discharge.md, ~6.5 h at full underglow, healthy.
- The repo is a Zephyr module (`zephyr/module.yml`, `Kconfig`, `CMakeLists.txt`, `dts/bindings`), passed to
  the build with `-DZMK_EXTRA_MODULES`.

## History

- Original firmware (2022, ZMK on Zephyr 3.0): `dumps/2026-10-10_CURRENT.UF2`, identical to an older backup.
  `tools/extract_keymap.py` recovered the keymap from it. To restore: `./flash.sh dumps/2026-10-10_CURRENT.UF2`.
- Git tag `pre-battest`: the first ZMK v0.3 build with deep sleep (`firmware/2026-10-10_3edb05c_bt60_v2-studio.uf2`).
