/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "security_manager.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
}

namespace nrf::bluetooth {

// Static instance for singleton
static SecurityManager* s_instance = nullptr;

SecurityManager& SecurityManager::instance() {
    static SecurityManager instance;
    return instance;
}

common::Result<> SecurityManager::initialize() {
    if (initialized_) {
        return common::Result<>::ok();
    }

    s_instance = this;

#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
    // Setup authentication callbacks
    auth_callbacks_.passkey_display = onPasskeyDisplay;
    auth_callbacks_.cancel = onAuthCancel;

    // Setup authentication info callbacks
    auth_info_callbacks_.pairing_complete = onPairingComplete;
    auth_info_callbacks_.pairing_failed = onPairingFailed;

    // Register authentication callbacks
    int err = bt_conn_auth_cb_register(&auth_callbacks_);
    if (err) {
        printk("Failed to register auth callbacks (err %d)\n", err);
        return common::Result<>::error(err);
    }

    err = bt_conn_auth_info_cb_register(&auth_info_callbacks_);
    if (err) {
        printk("Failed to register auth info callbacks (err %d)\n", err);
        return common::Result<>::error(err);
    }

    printk("Security manager initialized\n");
#else
    printk("Security manager initialized (security disabled)\n");
#endif

    initialized_ = true;
    return common::Result<>::ok();
}

#ifdef CONFIG_BT_LBS_SECURITY_ENABLED

void SecurityManager::onSecurityChanged(bt_conn* conn, bt_security_t level, 
                                         enum bt_security_err err) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        printk("Security changed: %s level %u\n", addr, level);
    } else {
        printk("Security failed: %s level %u err %d %s\n", addr, level, err,
               bt_security_err_to_str(err));
    }
}

void SecurityManager::onPasskeyDisplay(bt_conn* conn, unsigned int passkey) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Passkey for %s: %06u\n", addr, passkey);
}

void SecurityManager::onAuthCancel(bt_conn* conn) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Pairing cancelled: %s\n", addr);
}

void SecurityManager::onPairingComplete(bt_conn* conn, bool bonded) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Pairing completed: %s, bonded: %d\n", addr, bonded);
}

void SecurityManager::onPairingFailed(bt_conn* conn, enum bt_security_err reason) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Pairing failed: %s, reason %d %s\n", addr, reason,
           bt_security_err_to_str(reason));
}

bt_conn_auth_cb* SecurityManager::getAuthCallbacks() {
    return &auth_callbacks_;
}

bt_conn_auth_info_cb* SecurityManager::getAuthInfoCallbacks() {
    return &auth_info_callbacks_;
}

#endif // CONFIG_BT_LBS_SECURITY_ENABLED

} // namespace nrf::bluetooth
