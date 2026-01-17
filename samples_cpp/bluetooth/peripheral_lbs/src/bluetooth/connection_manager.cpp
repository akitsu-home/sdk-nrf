/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "connection_manager.hpp"
#include "advertising_manager.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/hci.h>
}

namespace nrf::bluetooth {

// Static instance for singleton
static ConnectionManager* s_instance = nullptr;

ConnectionManager& ConnectionManager::instance() {
    static ConnectionManager instance;
    return instance;
}

common::Result<> ConnectionManager::initialize(AdvertisingManager& advertising_manager) {
    if (initialized_) {
        return common::Result<>::ok();
    }

    s_instance = this;
    advertising_manager_ = &advertising_manager;

    initialized_ = true;
    printk("Connection manager initialized\n");
    return common::Result<>::ok();
}

void ConnectionManager::setCallback(ConnectionCallback callback) {
    callback_ = std::move(callback);
}

bool ConnectionManager::isConnected() const {
    return connected_;
}

void ConnectionManager::onConnected(bt_conn* conn, uint8_t err) {
    if (!s_instance) {
        return;
    }

    if (err) {
        printk("Connection failed, err 0x%02x %s\n", err, bt_hci_err_to_str(err));
        s_instance->connected_ = false;
        return;
    }

    printk("Connected\n");
    s_instance->connected_ = true;

    if (s_instance->callback_) {
        s_instance->callback_(ConnectionEvent::Connected, conn);
    }
}

void ConnectionManager::onDisconnected(bt_conn* conn, uint8_t reason) {
    if (!s_instance) {
        return;
    }

    printk("Disconnected, reason 0x%02x %s\n", reason, bt_hci_err_to_str(reason));
    s_instance->connected_ = false;

    if (s_instance->callback_) {
        s_instance->callback_(ConnectionEvent::Disconnected, conn);
    }
}

void ConnectionManager::onRecycled() {
    if (!s_instance) {
        return;
    }

    printk("Connection object recycled. Restarting advertising.\n");

    if (s_instance->callback_) {
        s_instance->callback_(ConnectionEvent::Recycled, nullptr);
    }

    // Restart advertising when connection is recycled
    if (s_instance->advertising_manager_) {
        s_instance->advertising_manager_->start();
    }
}

} // namespace nrf::bluetooth
