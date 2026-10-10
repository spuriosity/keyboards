# Keyboards

Keymaps, firmware and tools for my keyboards, grouped by firmware. Built and maintained with Claude Code.

| Keyboard | Firmware | Folder | Status |
|---|---|---|---|
| splitkb Aurora **Lily58** rev1 (split, Elite-C / ATmega32u4) | QMK | [`qmk/lily58`](qmk/lily58) | Colemak-DH + QWERTY (switchable), SpaceFn. Flashed 2026-10-09. |
| Polarity Works **BT60 V2** (60% ANSI, wireless, nRF52840) | ZMK v0.3 | [`zmk/bt60`](zmk/bt60) | QWERTY, SpaceFn, right Ctrl = keyboard control layer, ZMK Studio. Flashed 2026-10-10. |

Each keyboard folder has a README (how to build, flash and read the firmware back), a record of what's on the
board, and notes for Claude Code (`CLAUDE.md`). `zmk/workspace/` is the shared ZMK build environment (not in git; recreate with
`zmk/bt60/setup-workspace.sh`).

Older material (the original BT60 firmware backup, the 2023 Lily58 Configurator exports) is in each keyboard's
`archive/` folder.
