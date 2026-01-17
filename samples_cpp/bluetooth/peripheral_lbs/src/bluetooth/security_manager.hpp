/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include "../common/result.hpp"

extern "C" {
#include <zephyr/bluetooth/conn.h>
}

namespace nrf::bluetooth {

/**
 * @brief Security Manager class
 * 
 * Manages BLE security operations including pairing, bonding,
 * and authentication callbacks.
 * 
 * This class is only active when CONFIG_BT_LBS_SECURITY_ENABLED is defined.
 */
class SecurityManager {
public:
    SecurityManager() = default;
    ~SecurityManager() = default;

    // Delete copy operations
    SecurityManager(const SecurityManager&) = delete;
    SecurityManager& operator=(const SecurityManager&) = delete;

    /**
     * @brief Initialize the security manager and register callbacks
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initialize();

    /**
     * @brief Check if security is enabled
     * @return true if security features are enabled
     */
    [[nodiscard]] static constexpr bool isEnabled() {
#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
        return true;
#else
        return false;
#endif
    }

    /**
     * @brief Get the singleton instance
     * @return Reference to the security manager instance
     */
    static SecurityManager& instance();

#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
    // Static callbacks for C API
    static void onSecurityChanged(bt_conn* conn, bt_security_t level, 
                                   enum bt_security_err err);
    static void onPasskeyDisplay(bt_conn* conn, unsigned int passkey);
    static void onAuthCancel(bt_conn* conn);
    static void onPairingComplete(bt_conn* conn, bool bonded);
    static void onPairingFailed(bt_conn* conn, enum bt_security_err reason);

    /**
     * @brief Get authentication callbacks structure
     * @return Pointer to the auth callbacks structure
     */
    bt_conn_auth_cb* getAuthCallbacks();

    /**
     * @brief Get authentication info callbacks structure
     * @return Pointer to the auth info callbacks structure
     */
    bt_conn_auth_info_cb* getAuthInfoCallbacks();
#endif

private:
    bool initialized_{false};

#ifdef CONFIG_BT_LBS_SECURITY_ENABLED
    bt_conn_auth_cb auth_callbacks_{};
    bt_conn_auth_info_cb auth_info_callbacks_{};
#endif
};

} // namespace nrf::bluetooth
