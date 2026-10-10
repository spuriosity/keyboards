# Discharge test, 2026-10-10 (overnight)

Firmware: `./build.sh battest`. Unplugged with underglow on (12 LEDs, purple, full brightness), no deep sleep,
connected over Bluetooth. One reading every 10 minutes, saved to flash.

Raw log as typed by the keyboard:

```
batlog every 10min, 41 readings, mV: 4146 3988 3958 3936 3912 3886 3864 3846 3818 3796 3788 3762 3746 3730 3716 3700 3692 3674 3666 3656 3646 3640 3630 3626 3618 3606 3600 3590 3576 3562 3556 3538 3510 3450 3356 3240 3100 2902 2650 2414 2894
```

| Time since unplug | mV |
|---|---|
| 0h00 | 4146 |
| 0h10 | 3988 |
| 0h20 | 3958 |
| 0h30 | 3936 |
| 0h40 | 3912 |
| 0h50 | 3886 |
| 1h00 | 3864 |
| 1h10 | 3846 |
| 1h20 | 3818 |
| 1h30 | 3796 |
| 1h40 | 3788 |
| 1h50 | 3762 |
| 2h00 | 3746 |
| 2h10 | 3730 |
| 2h20 | 3716 |
| 2h30 | 3700 |
| 2h40 | 3692 |
| 2h50 | 3674 |
| 3h00 | 3666 |
| 3h10 | 3656 |
| 3h20 | 3646 |
| 3h30 | 3640 |
| 3h40 | 3630 |
| 3h50 | 3626 |
| 4h00 | 3618 |
| 4h10 | 3606 |
| 4h20 | 3600 |
| 4h30 | 3590 |
| 4h40 | 3576 |
| 4h50 | 3562 |
| 5h00 | 3556 |
| 5h10 | 3538 |
| 5h20 | 3510 |
| 5h30 | 3450 |
| 5h40 | 3356 |
| 5h50 | 3240 |
| 6h00 | 3100 |
| 6h10 | 2902 |
| 6h20 | 2650 |
| 6h30 | 2414 |
| 6h40 | 2894 |

## Reading it

- **0h00 → 0h10, 4146 → 3988 mV**: the load (underglow) pulling the voltage down plus surface charge settling. Normal.
- **0h10 → 5h20, 3988 → 3538 mV**: a steady, even slope, the long middle part of a lithium cell's discharge.
- **5h30 onwards**: the knee. The cell is empty and the voltage falls fast (3450 → 2414 mV in 70 minutes).
- **Last reading, 2894 mV**: higher than the one before, so the load dropped out (LEDs browning out, or a reset)
  and the voltage bounced back. The useful run ended around 6h40.

**Verdict:** the battery works and the curve has a normal, healthy shape. There's no early collapse or large sag
of the kind a worn cell shows. About 5.5 hours to empty (~3.45 V, where ZMK reports 0%) and ~6.5 hours until
it gave out, at the board's heaviest load. Without knowing the exact load current or the cell's rated capacity this
can't be turned into a precise "% health", but a worn-out cell would not have lasted this long under full underglow.

**Caution:** the cell was taken down to ~2.4 V under load, well below the ~3.0 V lithium cells should stop at.
Recharge it promptly, and don't repeat full-to-empty tests often.
