/*
 * Battery discharge log.
 *
 * With CONFIG_BT60_BATTERY_LOG, unplugging USB starts a fresh log: the battery voltage is sampled
 * every CONFIG_BT60_BATTERY_LOG_INTERVAL_MIN minutes and saved to flash after each sample, so the
 * log survives the battery running flat. Plugging USB back in stops logging.
 *
 * The &bat_log behavior (any build) types the saved log, e.g.
 * "batlog every 10min, 3 readings, mV: 4180 4152 4141".
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_battery_log

#include <stdio.h>
#include <string.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <drivers/behavior.h>

#include <zmk/event_manager.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/rgb_underglow.h>
#include <zmk/usb.h>

#include "bt60_battery.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define MAX_SAMPLES 288 // 48 hours at 10 minutes
#define TYPE_BATCH 24   // characters per batch; 2 queue entries each, under the 64-entry queue

struct battery_log {
    uint16_t interval_min;
    uint16_t count;
    uint16_t mv[MAX_SAMPLES];
};

static struct battery_log batlog;

static int batlog_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;

    if (!settings_name_steq(name, "batlog", &next) || next) {
        return -ENOENT;
    }
    if (len > sizeof(batlog)) {
        return -EINVAL;
    }
    memset(&batlog, 0, sizeof(batlog));
    int rc = read_cb(cb_arg, &batlog, len);
    if (rc < 0) {
        return rc;
    }
    batlog.count = MIN(batlog.count, MAX_SAMPLES);
    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(bt60, "bt60", NULL, batlog_set, NULL, NULL);

// --- typing the log ---

static char text[32 + MAX_SAMPLES * 5];
static size_t text_len, text_pos;
static struct zmk_behavior_binding_event type_event;

static void type_work_cb(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(type_work, type_work_cb);

static void type_work_cb(struct k_work *work) {
    size_t n = MIN(TYPE_BATCH, text_len - text_pos);

    bt60_type(&type_event, &text[text_pos], n);
    text_pos += n;
    if (text_pos < text_len) {
        // let this batch drain from the behavior queue before adding the next
        k_work_schedule(&type_work, K_MSEC(n * 2 * BT60_TAP_MS + 100));
    }
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    if (k_work_delayable_is_pending(&type_work)) {
        return ZMK_BEHAVIOR_OPAQUE; // already typing
    }
    int len = snprintf(text, sizeof(text), "batlog every %dmin, %d readings, mV:",
                       batlog.interval_min, batlog.count);
    for (int i = 0; i < batlog.count && len < sizeof(text) - 6; i++) {
        len += snprintf(&text[len], sizeof(text) - len, " %d", batlog.mv[i]);
    }
    text_len = len;
    text_pos = 0;
    type_event = event;
    k_work_schedule(&type_work, K_NO_WAIT);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_battery_log_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_battery_log_driver_api);

// --- logging while unplugged ---

#if IS_ENABLED(CONFIG_BT60_BATTERY_LOG)

static void sample_work_cb(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(sample_work, sample_work_cb);

static void sample_work_cb(struct k_work *work) {
    int percent, millivolts;

    if (zmk_usb_is_powered()) {
        return;
    }
    if (!bt60_read_battery(&percent, &millivolts) && batlog.count < MAX_SAMPLES) {
        batlog.mv[batlog.count++] = millivolts;
        int rc = settings_save_one("bt60/batlog", &batlog,
                                   offsetof(struct battery_log, mv) + batlog.count * sizeof(uint16_t));
        if (rc) {
            LOG_ERR("Failed to save battery log: %d", rc);
        }
    }
    k_work_schedule(&sample_work, K_MINUTES(CONFIG_BT60_BATTERY_LOG_INTERVAL_MIN));
}

static void start_logging(bool fresh) {
    if (fresh) {
        batlog.count = 0;
    }
    batlog.interval_min = CONFIG_BT60_BATTERY_LOG_INTERVAL_MIN;
#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW)
    zmk_rgb_underglow_on(); // a steady load so the discharge curve means something
#endif
    k_work_schedule(&sample_work, K_NO_WAIT);
}

static int usb_listener(const zmk_event_t *eh) {
    const struct zmk_usb_conn_state_changed *ev = as_zmk_usb_conn_state_changed(eh);

    if (ev->conn_state == ZMK_USB_CONN_NONE) {
        start_logging(true);
    } else {
        k_work_cancel_delayable(&sample_work);
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(bt60_battery_log, usb_listener);
ZMK_SUBSCRIPTION(bt60_battery_log, zmk_usb_conn_state_changed);

// Booting on battery (e.g. after a brown-out) continues the existing log instead of clearing it.
static void boot_work_cb(struct k_work *work) {
    if (!zmk_usb_is_powered()) {
        start_logging(false);
    }
}
static K_WORK_DELAYABLE_DEFINE(boot_work, boot_work_cb);

static int battery_log_init(void) {
    k_work_schedule(&boot_work, K_SECONDS(5)); // after settings have loaded
    return 0;
}

SYS_INIT(battery_log_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif // CONFIG_BT60_BATTERY_LOG

#endif // DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
