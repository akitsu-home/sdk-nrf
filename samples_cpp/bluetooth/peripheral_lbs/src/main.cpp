/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "application.hpp"
#include "bluetooth/connection_manager.hpp"
#include "bluetooth/security_manager.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/conn.h>
}

// Connection callbacks must be defined at file scope for BT_CONN_CB_DEFINE macro
namespace {

void on_connected(bt_conn* conn, uint8_t err) {
    nrf::bluetooth::ConnectionManager::onConnected(conn, err);
}

void on_disconnected(bt_conn* conn, uint8_t reason) {
    nrf::bluetooth::ConnectionManager::onDisconnected(conn, reason);
}

void on_recycled() {
    nrf::bluetooth::ConnectionManager::onRecycled();
}

#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
void on_security_changed(bt_conn* conn, bt_security_t level, enum bt_security_err err) {
    nrf::bluetooth::SecurityManager::onSecurityChanged(conn, level, err);
}
#endif

} // anonymous namespace

// Register connection callbacks with Zephyr
BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = on_connected,
    .disconnected = on_disconnected,
    .recycled = on_recycled,
#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
    .security_changed = on_security_changed,
#endif
};

/**
 * @brief Application entry point
 */
int main() {
    nrf::Application app;

    auto result = app.initialize();
    if (!result) {
        printk("Application initialization failed (err %d)\n", result.errorCode());
        return 0;
    }

    app.run();
    return 0;
}
