/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include <cstdint>
#include "../common/result.hpp"

namespace nrf::hardware {

/**
 * @brief LED Controller class
 * 
 * Manages LED initialization and control operations.
 * Provides an abstraction over the DK LED library.
 */
class LedController {
public:
    LedController() = default;
    ~LedController() = default;

    // Delete copy operations
    LedController(const LedController&) = delete;
    LedController& operator=(const LedController&) = delete;

    /**
     * @brief Initialize the LED subsystem
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initialize();

    /**
     * @brief Turn on a specific LED
     * @param led LED identifier
     */
    void turnOn(uint32_t led);

    /**
     * @brief Turn off a specific LED
     * @param led LED identifier
     */
    void turnOff(uint32_t led);

    /**
     * @brief Set LED state
     * @param led LED identifier
     * @param state true for on, false for off
     */
    void set(uint32_t led, bool state);

    /**
     * @brief Toggle LED state
     * @param led LED identifier
     */
    void toggle(uint32_t led);

private:
    bool initialized_{false};
};

} // namespace nrf::hardware
