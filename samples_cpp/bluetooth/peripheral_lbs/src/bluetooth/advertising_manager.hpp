/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include <functional>
#include "../common/result.hpp"

extern "C" {
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
}

namespace nrf::bluetooth {

/**
 * @brief Advertising Manager class
 * 
 * Manages BLE advertising operations including start, stop,
 * and advertising data configuration.
 */
class AdvertisingManager {
public:
    AdvertisingManager() = default;
    ~AdvertisingManager() = default;

    // Delete copy operations
    AdvertisingManager(const AdvertisingManager&) = delete;
    AdvertisingManager& operator=(const AdvertisingManager&) = delete;

    /**
     * @brief Initialize the advertising manager
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initialize();

    /**
     * @brief Start advertising
     * 
     * Submits a work item to start advertising asynchronously.
     */
    void start();

    /**
     * @brief Stop advertising
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> stop();

    /**
     * @brief Check if currently advertising
     * @return true if advertising is active
     */
    [[nodiscard]] bool isAdvertising() const;

    /**
     * @brief Get the singleton instance
     * @return Reference to the advertising manager instance
     */
    static AdvertisingManager& instance();

private:
    /**
     * @brief Work handler for starting advertising
     */
    static void advertisingWorkHandler(k_work* work);

    /**
     * @brief Actually start the advertising
     * @return Error code (0 on success)
     */
    int doStartAdvertising();

    k_work adv_work_{};
    bool initialized_{false};
    bool advertising_{false};
};

} // namespace nrf::bluetooth
