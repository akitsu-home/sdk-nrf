/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include <cstdint>
#include <functional>
#include "../common/result.hpp"

namespace nrf::hardware {

/**
 * @brief Button state change callback type
 * @param button_state Current button state bitmask
 * @param has_changed Bitmask of buttons that changed
 */
using ButtonCallback = std::function<void(uint32_t button_state, uint32_t has_changed)>;

/**
 * @brief Button Controller class
 * 
 * Manages button initialization and event handling.
 * Provides an abstraction over the DK buttons library.
 */
class ButtonController {
public:
    ButtonController() = default;
    ~ButtonController() = default;

    // Delete copy operations
    ButtonController(const ButtonController&) = delete;
    ButtonController& operator=(const ButtonController&) = delete;

    /**
     * @brief Initialize the button subsystem
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initialize();

    /**
     * @brief Set the button callback handler
     * @param callback Function to call when button state changes
     */
    void setCallback(ButtonCallback callback);

    /**
     * @brief Get current button state
     * @param button_mask Button to check
     * @return true if button is pressed
     */
    [[nodiscard]] bool isPressed(uint32_t button_mask) const;

    /**
     * @brief Get the singleton instance
     * @return Reference to the button controller instance
     */
    static ButtonController& instance();

private:
    /**
     * @brief Static callback for C API
     */
    static void buttonChangedHandler(uint32_t button_state, uint32_t has_changed);

    ButtonCallback callback_;
    bool initialized_{false};
    uint32_t current_state_{0};
};

} // namespace nrf::hardware
