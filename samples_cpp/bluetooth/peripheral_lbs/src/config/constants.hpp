/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include <cstdint>

extern "C" {
#include <dk_buttons_and_leds.h>
}

namespace nrf::config {

/**
 * @brief Device configuration constants
 */
constexpr const char* kDeviceName = CONFIG_BT_DEVICE_NAME;
constexpr std::size_t kDeviceNameLen = sizeof(CONFIG_BT_DEVICE_NAME) - 1;

/**
 * @brief LED configuration
 */
struct LedConfig {
    static constexpr uint32_t kRunStatus = DK_LED1;
    static constexpr uint32_t kConnectionStatus = DK_LED2;
    static constexpr uint32_t kUser = DK_LED3;
};

/**
 * @brief Button configuration
 */
struct ButtonConfig {
    static constexpr uint32_t kUserMask = DK_BTN1_MSK;
};

/**
 * @brief Timing configuration
 */
struct TimingConfig {
    static constexpr uint32_t kRunLedBlinkIntervalMs = 1000;
};

} // namespace nrf::config
