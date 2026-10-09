/*
 * Shared helpers for the BT60 battery behaviors: read the battery sensor and type text.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#include <dt-bindings/zmk/keys.h>

#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>

#define BT60_TAP_MS 10

static const struct device *const bt60_battery = DEVICE_DT_GET(DT_CHOSEN(zmk_battery));

// The battery-voltage-divider driver only supports the GAUGE_* channels
static inline int bt60_read_battery(int *percent, int *millivolts) {
    struct sensor_value soc, voltage;

    if (!device_is_ready(bt60_battery)) {
        return -ENODEV;
    }
    int rc = sensor_sample_fetch_chan(bt60_battery, SENSOR_CHAN_ALL);
    if (!rc) {
        rc = sensor_channel_get(bt60_battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE, &soc);
    }
    if (!rc) {
        rc = sensor_channel_get(bt60_battery, SENSOR_CHAN_GAUGE_VOLTAGE, &voltage);
    }
    if (rc) {
        return rc;
    }
    *percent = soc.val1;
    *millivolts = voltage.val1 * 1000 + voltage.val2 / 1000;
    return 0;
}

static inline uint32_t bt60_keycode_for(char c) {
    switch (c) {
    case '0':
        return N0;
    case '1' ... '9':
        return N1 + (c - '1');
    case 'a' ... 'z':
        return A + (c - 'a');
    case '%':
        return PERCENT;
    case ' ':
        return SPACE;
    case ',':
        return COMMA;
    case ':':
        return COLON;
    case 'V':
        return LS(V);
    default:
        return 0;
    }
}

// Queues one key tap per character. The behavior queue holds CONFIG_ZMK_BEHAVIORS_QUEUE_SIZE
// entries (two per character), so callers must keep each batch short.
static inline void bt60_type(const struct zmk_behavior_binding_event *event, const char *text,
                             size_t len) {
    for (size_t i = 0; i < len && text[i]; i++) {
        const struct zmk_behavior_binding kp = {
            .behavior_dev = DEVICE_DT_NAME(DT_INST(0, zmk_behavior_key_press)),
            .param1 = bt60_keycode_for(text[i]),
        };
        if (kp.param1) {
            zmk_behavior_queue_add(event, kp, true, BT60_TAP_MS);
            zmk_behavior_queue_add(event, kp, false, BT60_TAP_MS);
        }
    }
}
