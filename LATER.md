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

- USB / Bluetooth output switch (`&out OUT_TOG`), so the board can type over Bluetooth while charging.
- After the battery test: decide what right Ctrl + Enter should do (it currently types the battery log).
