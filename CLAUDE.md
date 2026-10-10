# Keyboards: notes for Claude

Two keyboards, each in its own folder; read that folder's CLAUDE.md and README before changing anything.

- `qmk/lily58`: splitkb Aurora Lily58 rev1, QMK, keymap in QMK Configurator JSON (`lily58.json`).
- `zmk/bt60`: Polarity Works BT60 V2, ZMK v0.3 (board `bt60_v2`), keymap in `config/bt60_v2.keymap`.
- `zmk/workspace/`: shared local ZMK build environment (gitignored, recreate with `zmk/bt60/setup-workspace.sh`).
- `local/` (gitignored): owner- and machine-specific notes. Read `local/NOTES.md` if it exists.

## Conventions

- This repo is the source of truth for both boards: commit every keymap change, and keep the docs (README,
  LAYOUT.md, LATER.md) in step with the keymap.
- Ideas that aren't taken up go in the board's `LATER.md`.
- Back up what's on a board before flashing it (the flash scripts do this). The user does the physical steps
  (bootloader keys, reset buttons, unplugging); say exactly which keys to press.
- No timing-based combos (press-together chords); use held layer keys.
- This repo is public: keep anything personal or machine-specific in `local/`.
- macOS setup: Karabiner-Elements runs on the host (it can remap keys before macOS sees them, which confuses
  key tests) and Raycast is used for global shortcuts. Ask before changing Karabiner or macOS settings.
- ZMK Studio (https://zmk.studio) needs a Chromium browser (Web Serial).
