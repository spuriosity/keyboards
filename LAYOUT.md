# BT60 layout

How the layers are organised, then a diagram of each one. The diagrams are generated from
`config/bt60_v2.keymap`; after changing the keymap, regenerate them with `python3 tools/layout.py` and paste the
output into the **Layers** section below.

## How it's organised

| Hold | Layer | What it's for |
|---|---|---|
| — | **Base** | QWERTY typing. |
| **Space** | **NAV** (SpaceFn) | Moving around: arrows, word jumps, Page/Home/End on the right hand; F13–F24 on the left hand for your own shortcuts; F1–F12 on the number row. Tap Space for a space. |
| **Fn** | **RAISE** | F-keys, media, Caps Lock, and the old PC keys (Insert, Print Screen, …). |
| **Right Ctrl** | **SYS** | Keyboard control: anything that changes the keyboard itself rather than typing. Right Ctrl is no longer a Ctrl key (left Ctrl is). |

Rules of thumb:

- **Right Ctrl** is the keyboard control layer. Bluetooth, USB/Bluetooth output, bootloader, battery, ZMK Studio and the lights all live here and nowhere else.
- Nothing destructive sits where it can be hit by accident: clearing a Bluetooth pairing is right Ctrl + Esc, at opposite corners.
- Holding a layer key and Shift together passes Shift through, so e.g. Space + Shift + arrows selects text.

### Base

- **Esc / `** (top-left): tap for Esc, hold for `` ` ``.
- **Caps Lock key**: tap for Backspace, hold for Ctrl. Tap then hold quickly to repeat Backspace.
- Bottom row: Ctrl, Alt, Cmd (Mac order), Space (hold for NAV), right Alt, Fn, Menu, right Ctrl (hold for SYS).

### NAV (hold Space)

| Keys | Does |
|---|---|
| `1`–`=` | F1–F12. With Shift: the Mac top-row actions (below) |
| `I` / `J` `K` `L` | ↑ / ← ↓ → |
| `U` / `O` | Word left / right (⌥← / ⌥→) |
| `Y` / `H` | Page Up / Page Down |
| `P` / `;` | Home / End |
| `N` / `M` | Back / forward (⌘[ / ⌘]) |
| `Q` `W` `E` `R` `T` | F13 F14 F15 F16 F17 |
| `A` `S` `D` `F` `G` | F18 F19 F20 F21 F22 |
| `Z` `X` | F23 F24 |
| `Tab` | Next window of the same app (⌘`) |
| `Esc` | `` ` `` |
| `Backspace` | Delete (forward) |

F13–F24 do nothing on a Mac by default, which makes them clean keys to bind to actions in Raycast
(window management, app launching, …) or any app's shortcut settings.

Space + Shift + number row, the Mac top-row actions sent as media keys (they work whatever the Mac's F-key setting):

| Key | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | − | = |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Does | Bright− | Bright+ | Mission Control | Spotlight | F5 | F6 | ⏮ | ⏯ | ⏭ | Mute | Vol− | Vol+ |

Plain Space + number sends F1–F12, but macOS (through Karabiner's virtual keyboard) turns those into brightness/volume
unless "Use F1, F2, etc. keys as standard function keys" is on, globally or for this keyboard in Karabiner.

### RAISE (hold Fn)

| Keys | Does |
|---|---|
| `1`–`=` | F1–F12 |
| `Backspace` | Delete |
| `W` / `A` `S` `D` | ↑ / ← ↓ → |
| Caps Lock key | Caps Lock |
| `Shift` `Z` `X` `C` | ⏮, Volume −, Volume +, Mute |
| `I` / `P` `[` `]` | Insert / Print Screen, Scroll Lock, Pause (PC keys, mostly unused on a Mac) |
| `;` `'` / `.` `/` | Home, Page Up / End, Page Down |

### SYS (hold right Ctrl): keyboard control

| Keys | Does |
|---|---|
| `1`–`5` | Switch to Bluetooth device 1–5 (an empty slot is in pairing mode) |
| `]` / `\` | Previous / next Bluetooth device |
| `Esc` | Clear the current Bluetooth device's pairing |
| `Enter` | Switch typing between USB and Bluetooth |
| `Backspace` | Bootloader, for flashing (`./flash.sh`). No keys work until it's flashed or power-cycled |
| `Right Shift` | Type the battery level, where typing is going and whether it's plugged in, e.g. `94% 4148mV, typing over bluetooth 1, on battery` |
| `U` | Unlock ZMK Studio for editing at <https://zmk.studio> |
| `K` | Key backlight on/off |
| `L` | Underglow on/off |

## Layers

### default_layer

Base (QWERTY). Esc on tap, ` on hold. Caps Lock key: Backspace on tap, Ctrl on hold. Hold Space for NAV, Fn for RAISE, right Ctrl for SYS.

```
 Esc/`     1       2       3       4       5       6       7       8       9       0       -       =     Bspc
  Tab      Q       W       E       R       T       Y       U       I       O       P       [       ]       \
Bsp/Ctl    A       S       D       F       G       H       J       K       L       ;       '     Enter
 Shift     Z       X       C       V       B       N       M       ,       .       /     Shift
 Ctrl     Alt     Cmd   Spc/NAV  RAlt     Fn     Menu     SYS
```

### raise

RAISE (hold Fn): F-keys, media, Caps Lock, Insert/Print Screen/Scroll Lock/Pause.

```
   ·      F1      F2      F3      F4      F5      F6      F7      F8      F9      F10     F11     F12     Del
   ·       ·       ↑       ·       ·       ·       ·       ·      Ins      ·     PrScr   ScrLk   Pause     ·
 Caps      ←       ↓       →       ·       ·       ·       ·       ·       ·     Home    PgUp      ·
   ⏮     Vol-    Vol+    Mute      ·       ·       ·       ·       ·      End    PgDn      ·
   ·       ·       ·       ·       ·       ·       ·       ·
```

### nav

NAV (hold Space): arrows on JKL/IK, word jump on U/O, Cmd+` and Cmd+[ / Cmd+].

```
   `      F1      F2      F3      F4      F5      F6      F7      F8      F9      F10     F11     F12     Del
  ⌘`      F13     F14     F15     F16     F17    PgUp     ⌥←       ↑      ⌥→     Home      ·       ·       ·
   ·      F18     F19     F20     F21     F22    PgDn      ←       ↓       →      End      ·       ·
   ·      F23     F24      ·       ·       ·      ⌘[      ⌘]       ·       ·       ·       ·
   ·       ·       ·       ·       ·       ·       ·       ·
```

### sys

SYS (hold right Ctrl): keyboard control. 1-5 = Bluetooth device, ] / \ = previous / next device,

```
 BTclr    BT1     BT2     BT3     BT4     BT5      ·       ·       ·       ·       ·       ·       ·     Boot
   ·       ·       ·       ·       ·       ·       ·    Studio     ·       ·       ·       ·      BT◀     BT▶
   ·       ·       ·       ·       ·       ·       ·       ·     KeyLt   Glow      ·       ·    USB/BT
   ·       ·       ·       ·       ·       ·       ·       ·       ·       ·       ·     Batt
   ·       ·       ·       ·       ·       ·       ·       ·
```
