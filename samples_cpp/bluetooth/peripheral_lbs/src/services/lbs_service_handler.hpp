/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include <functional>
#include "../common/result.hpp"

extern "C" {
#include <bluetooth/services/lbs.h>
}

namespace nrf::hardware {
class LedController;
class ButtonController;
} // namespace nrf::hardware

namespace nrf::services {

/**
 * @brief LED callback type - called when remote device controls LED
 * @param state LED state (true = on, false = off)
 */
using LedCallback = std::function<void(bool state)>;

/**
 * @brief Button callback type - called when button state is requested
 * @return Current button state
 */
using ButtonStateCallback = std::function<bool()>;

/**
 * @brief LBS (LED Button Service) Service Handler class
 * 
 * Manages the Nordic LED Button Service GATT service,
 * handling LED control and button state notifications.
 */
class LbsServiceHandler {
public:
    LbsServiceHandler() = default;
    ~LbsServiceHandler() = default;

    // Delete copy operations
    LbsServiceHandler(const LbsServiceHandler&) = delete;
    LbsServiceHandler& operator=(const LbsServiceHandler&) = delete;

    /**
     * @brief Initialize the LBS service
     * @param led_controller Reference to LED controller for LED control
     * @param button_controller Reference to button controller for state
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initialize(
        hardware::LedController& led_controller,
        hardware::ButtonController& button_controller);

    /**
     * @brief Set custom LED callback
     * @param callback Function to call when LED state changes
     */
    void setLedCallback(LedCallback callback);

    /**
     * @brief Set custom button state callback
     * @param callback Function to call when button state is requested
     */
    void setButtonStateCallback(ButtonStateCallback callback);

    /**
     * @brief Send button state notification to connected device
     * @param state Button state (true = pressed)
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> sendButtonState(bool state);

    /**
     * @brief Get the singleton instance
     * @return Reference to the LBS service handler instance
     */
    static LbsServiceHandler& instance();

private:
    // Static callbacks for C API
    static void ledCallbackHandler(bool led_state);
    static bool buttonCallbackHandler();

    hardware::LedController* led_controller_{nullptr};
    hardware::ButtonController* button_controller_{nullptr};
    LedCallback led_callback_;
    ButtonStateCallback button_state_callback_;
    bt_lbs_cb lbs_callbacks_{};
    bool initialized_{false};
};

} // namespace nrf::services
