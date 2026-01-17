/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include <functional>
#include "../common/result.hpp"

extern "C" {
#include <zephyr/bluetooth/conn.h>
}

namespace nrf::bluetooth {

// Forward declaration
class AdvertisingManager;

/**
 * @brief Connection event types
 */
enum class ConnectionEvent {
    Connected,
    Disconnected,
    Recycled
};

/**
 * @brief Connection event callback type
 */
using ConnectionCallback = std::function<void(ConnectionEvent event, bt_conn* conn)>;

/**
 * @brief Connection Manager class
 * 
 * Manages BLE connection events including connection, disconnection,
 * and connection recycling.
 */
class ConnectionManager {
public:
    ConnectionManager() = default;
    ~ConnectionManager() = default;

    // Delete copy operations
    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

    /**
     * @brief Initialize the connection manager
     * @param advertising_manager Reference to the advertising manager for restart on disconnect
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initialize(AdvertisingManager& advertising_manager);

    /**
     * @brief Set the connection event callback
     * @param callback Function to call on connection events
     */
    void setCallback(ConnectionCallback callback);

    /**
     * @brief Check if currently connected
     * @return true if a connection is active
     */
    [[nodiscard]] bool isConnected() const;

    /**
     * @brief Get the singleton instance
     * @return Reference to the connection manager instance
     */
    static ConnectionManager& instance();

    // Static callbacks for C API (need to be public for BT_CONN_CB_DEFINE)
    static void onConnected(bt_conn* conn, uint8_t err);
    static void onDisconnected(bt_conn* conn, uint8_t reason);
    static void onRecycled();

private:
    ConnectionCallback callback_;
    AdvertisingManager* advertising_manager_{nullptr};
    bool connected_{false};
    bool initialized_{false};
};

} // namespace nrf::bluetooth
