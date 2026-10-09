/*
 * Battery report behavior: on press, reads the battery sensor (zmk,battery) and types
 * "<percent>% <millivolts>mV", or "battery error" if the sensor can't be read.
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_battery_report

#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <dt-bindings/zmk/keys.h>

#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define TAP_MS 10

static const struct device *const battery = DEVICE_DT_GET(DT_CHOSEN(zmk_battery));

static void tap(const struct zmk_behavior_binding_event *event, uint32_t keycode) {
    const struct zmk_behavior_binding kp = {
        .behavior_dev = DEVICE_DT_NAME(DT_INST(0, zmk_behavior_key_press)),
        .param1 = keycode,
    };
    zmk_behavior_queue_add(event, kp, true, TAP_MS);
    zmk_behavior_queue_add(event, kp, false, TAP_MS);
}

static uint32_t keycode_for(char c) {
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
    case 'V':
        return LS(V);
    default:
        return 0;
    }
}

static int read_battery(int *percent, int *millivolts) {
    struct sensor_value soc, voltage;

    if (!device_is_ready(battery)) {
        return -ENODEV;
    }
    // The battery-voltage-divider driver only supports the GAUGE_* channels
    int rc = sensor_sample_fetch_chan(battery, SENSOR_CHAN_ALL);
    if (!rc) {
        rc = sensor_channel_get(battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE, &soc);
    }
    if (!rc) {
        rc = sensor_channel_get(battery, SENSOR_CHAN_GAUGE_VOLTAGE, &voltage);
    }
    if (rc) {
        return rc;
    }
    *percent = soc.val1;
    *millivolts = voltage.val1 * 1000 + voltage.val2 / 1000;
    return 0;
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    char text[32];
    int percent, millivolts;
    int rc = read_battery(&percent, &millivolts);

    if (rc) {
        LOG_ERR("Battery read failed: %d", rc);
        snprintf(text, sizeof(text), "battery error %d", -rc);
    } else {
        snprintf(text, sizeof(text), "%d%% %dmV", percent, millivolts);
    }
    for (const char *c = text; *c; c++) {
        uint32_t keycode = keycode_for(*c);
        if (keycode) {
            tap(&event, keycode);
        }
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_battery_report_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_battery_report_driver_api);

#endif
