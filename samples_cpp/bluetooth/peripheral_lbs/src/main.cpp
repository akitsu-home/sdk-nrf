/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <cstddef>
#include <cstring>
#include <cerrno>

extern "C" {
#include <zephyr/types.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <soc.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <bluetooth/services/lbs.h>

#include <zephyr/settings/settings.h>

#include <dk_buttons_and_leds.h>
}

namespace {

// Constants
constexpr const char* kDeviceName = CONFIG_BT_DEVICE_NAME;
constexpr size_t kDeviceNameLen = sizeof(CONFIG_BT_DEVICE_NAME) - 1;

constexpr uint32_t kRunStatusLed = DK_LED1;
constexpr uint32_t kConStatusLed = DK_LED2;
constexpr uint32_t kRunLedBlinkInterval = 200;

constexpr uint32_t kUserLed = DK_LED3;
constexpr uint32_t kUserButton = DK_BTN1_MSK;

// Global state
bool g_app_button_state = false;
k_work g_adv_work;

// Advertising data
const bt_data g_ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, kDeviceName, kDeviceNameLen),
};

const bt_data g_sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_LBS_VAL),
};

// Advertising work handler
void adv_work_handler(k_work* work)
{
    ARG_UNUSED(work);

    int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, g_ad, ARRAY_SIZE(g_ad),
                               g_sd, ARRAY_SIZE(g_sd));
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    printk("Advertising successfully started\n");
}

void advertising_start()
{
    k_work_submit(&g_adv_work);
}

// Connection callbacks
void connected(bt_conn* conn, uint8_t err)
{
    if (err) {
        printk("Connection failed, err 0x%02x %s\n", err, bt_hci_err_to_str(err));
        return;
    }

    printk("Connected\n");
    dk_set_led_on(kConStatusLed);
}

void disconnected(bt_conn* conn, uint8_t reason)
{
    ARG_UNUSED(conn);

    printk("Disconnected, reason 0x%02x %s\n", reason, bt_hci_err_to_str(reason));
    dk_set_led_off(kConStatusLed);
}

void recycled_cb()
{
    printk("Connection object available from previous conn. Disconnect is complete!\n");
    advertising_start();
}

#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
void security_changed(bt_conn* conn, bt_security_t level, bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        printk("Security changed: %s level %u\n", addr, level);
    } else {
        printk("Security failed: %s level %u err %d %s\n", addr, level, err,
               bt_security_err_to_str(err));
    }
}
#endif

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
    .recycled = recycled_cb,
#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
    .security_changed = security_changed,
#endif
};

#if defined(CONFIG_BT_LBS_SECURITY_ENABLED)
void auth_passkey_display(bt_conn* conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Passkey for %s: %06u\n", addr, passkey);
}

void auth_cancel(bt_conn* conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Pairing cancelled: %s\n", addr);
}

void pairing_complete(bt_conn* conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Pairing completed: %s, bonded: %d\n", addr, bonded);
}

void pairing_failed(bt_conn* conn, bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Pairing failed conn: %s, reason %d %s\n", addr, reason,
           bt_security_err_to_str(reason));
}

bt_conn_auth_cb g_conn_auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .cancel = auth_cancel,
};

bt_conn_auth_info_cb g_conn_auth_info_callbacks = {
    .pairing_complete = pairing_complete,
    .pairing_failed = pairing_failed,
};
#else
bt_conn_auth_cb g_conn_auth_callbacks;
bt_conn_auth_info_cb g_conn_auth_info_callbacks;
#endif

// LBS callbacks
void app_led_cb(bool led_state)
{
    dk_set_led(kUserLed, led_state);
}

bool app_button_cb()
{
    return g_app_button_state;
}

bt_lbs_cb g_lbs_callbacks = {
    .led_cb = app_led_cb,
    .button_cb = app_button_cb,
};

// Button handler
void button_changed(uint32_t button_state, uint32_t has_changed)
{
    if (has_changed & kUserButton) {
        uint32_t user_button_state = button_state & kUserButton;

        bt_lbs_send_button_state(user_button_state);
        g_app_button_state = user_button_state ? true : false;
    }
}

int init_button()
{
    int err = dk_buttons_init(button_changed);
    if (err) {
        printk("Cannot init buttons (err: %d)\n", err);
    }

    return err;
}

} // anonymous namespace

int main()
{
    int blink_status = 0;
    int err;

    printk("Starting Bluetooth Peripheral LBS sample (C++)\n");

    err = dk_leds_init();
    if (err) {
        printk("LEDs init failed (err %d)\n", err);
        return 0;
    }

    err = init_button();
    if (err) {
        printk("Button init failed (err %d)\n", err);
        return 0;
    }

    if (IS_ENABLED(CONFIG_BT_LBS_SECURITY_ENABLED)) {
        err = bt_conn_auth_cb_register(&g_conn_auth_callbacks);
        if (err) {
            printk("Failed to register authorization callbacks.\n");
            return 0;
        }

        err = bt_conn_auth_info_cb_register(&g_conn_auth_info_callbacks);
        if (err) {
            printk("Failed to register authorization info callbacks.\n");
            return 0;
        }
    }

    err = bt_enable(nullptr);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    printk("Bluetooth initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    err = bt_lbs_init(&g_lbs_callbacks);
    if (err) {
        printk("Failed to init LBS (err:%d)\n", err);
        return 0;
    }

    k_work_init(&g_adv_work, adv_work_handler);
    advertising_start();

    for (;;) {
        dk_set_led(kRunStatusLed, (++blink_status) % 2);
        k_sleep(K_MSEC(kRunLedBlinkInterval));
    }
}
