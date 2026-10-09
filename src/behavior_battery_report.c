/*
 * Battery report behavior: on press, reads the battery sensor (zmk,battery) and types
 * "<percent>% <millivolts>mV", or "battery error" if the sensor can't be read.
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_battery_report

#include <stdio.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>

#include "bt60_battery.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    char text[32];
    int percent, millivolts;
    int rc = bt60_read_battery(&percent, &millivolts);

    if (rc) {
        LOG_ERR("Battery read failed: %d", rc);
        snprintf(text, sizeof(text), "battery error %d", -rc);
    } else {
        snprintf(text, sizeof(text), "%d%% %dmV", percent, millivolts);
    }
    bt60_type(&event, text, sizeof(text));
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
