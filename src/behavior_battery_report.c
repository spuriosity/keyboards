/*
 * Battery report behavior: on press, reads the battery sensor (zmk,battery) and types the level, where typing
 * is going, and whether USB power is connected, e.g.
 *   "94% 4148mV, typing over usb, plugged in"
 *   "81% 3932mV, typing over bluetooth 2, on battery"
 * or "battery error <n>" if the sensor can't be read.
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_battery_report

#include <stdio.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/usb.h>

#include "bt60_battery.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    char text[64];
    int percent, millivolts, len;
    int rc = bt60_read_battery(&percent, &millivolts);

    if (rc) {
        LOG_ERR("Battery read failed: %d", rc);
        len = snprintf(text, sizeof(text), "battery error %d", -rc);
    } else {
        len = snprintf(text, sizeof(text), "%d%% %dmV", percent, millivolts);
    }

    struct zmk_endpoint_instance endpoint = zmk_endpoints_selected();
    if (endpoint.transport == ZMK_TRANSPORT_USB) {
        len += snprintf(&text[len], sizeof(text) - len, ", typing over usb");
    } else {
        len += snprintf(&text[len], sizeof(text) - len, ", typing over bluetooth %d%s",
                        endpoint.ble.profile_index + 1,
                        zmk_ble_active_profile_is_connected() ? "" : " not connected");
    }
    snprintf(&text[len], sizeof(text) - len, ", %s", zmk_usb_is_powered() ? "plugged in" : "on battery");
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
