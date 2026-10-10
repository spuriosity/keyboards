# Settings to add later

## Purple underglow (keep it)

The purple from the battery-test firmware. It's the BT60 V2's built-in default, so it shows whenever the underglow is on
and hasn't been changed with the Fn RGB keys.

| Setting | Value | Meaning |
|---|---|---|
| `CONFIG_ZMK_RGB_UNDERGLOW_HUE_START` | `262` | Purple (hue 0–359) |
| `CONFIG_ZMK_RGB_UNDERGLOW_SAT_START` | `100` | Full colour, no white mixed in |
| `CONFIG_ZMK_RGB_UNDERGLOW_BRT_START` | `100` | Full brightness |
| `CONFIG_ZMK_RGB_UNDERGLOW_EFF_START` | `0` | Solid (no animation) |
| `CONFIG_ZMK_RGB_UNDERGLOW_ON_START` | `y` | On at power-up |

The strip has 12 LEDs. To pin the look explicitly, add the lines above to `config/bt60_v2.conf`.

The normal firmware already turns the underglow on at start-up, in this purple, but switches it off after 30 seconds
idle (`CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_IDLE=y`) to save battery. To keep it glowing:

- **Only on USB**: `CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB=y` with `AUTO_OFF_IDLE` removed turns it off on battery
  and keeps it on while plugged in. Probably the best of both.
- **Always on**: remove `AUTO_OFF_IDLE`. Costs a lot of battery: 12 LEDs at full brightness are the biggest power draw
  on the board. A lower `BRT_START` (e.g. 40) helps.

Fn + `\` toggles the underglow on and off at any time. Changes made with the Fn RGB keys are saved on the keyboard
and override these defaults.

## Other ideas

- Low-battery warning (brief red underglow flash under ~20%) and a protective auto power-off at ~3.3 V.
- Space layer, Mac line navigation: swap Home/End for ⌘← / ⌘→ (Home/End scroll the document in most Mac apps),
  and add ⌘↑ / ⌘↓ for top/bottom of the document.
- Space layer, editing keys on the left hand: delete word (⌥⌫), delete to line start (⌘⌫), undo/redo, cut/copy/paste.
- Fn layer: replace the PC keys (Insert, Print Screen, Scroll Lock, Pause) with Mac screenshots (⌘⇧3 / ⌘⇧4 / ⌘⇧5).
- Base: the Menu key does nothing on a Mac; could become right Cmd/Option, or a Meh/Hyper key.
- Plain F-keys from Space + number: needs the Mac/Karabiner "standard function keys" setting (see LAYOUT.md).
- Window keys on the Space layer, tried 2026-10-10 and replaced by F13–F24: Q/E previous/next tab (⌘⇧[ / ⌘⇧]),
  W Mission Control (⌃↑), S app windows (⌃↓), A/D desktop left/right (⌃← / ⌃→), T last app (⌘Tab).
  Could come back as Raycast bindings on F13–F24 instead.
- Meh (⌃⌥⇧) + key on the Space layer's unbound keys ([ ] \ ' Enter C V B , . /), proposed 2026-10-10, parked.
