/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include "common/result.hpp"
#include "hardware/led_controller.hpp"
#include "hardware/button_controller.hpp"
#include "bluetooth/advertising_manager.hpp"
#include "bluetooth/connection_manager.hpp"
#include "bluetooth/security_manager.hpp"
#include "services/lbs_service_handler.hpp"

namespace nrf {

/**
 * @brief Main application class for Bluetooth LBS sample
 * 
 * This class orchestrates all components of the BLE peripheral application,
 * including hardware controllers, Bluetooth managers, and GATT services.
 */
class Application {
public:
    Application() = default;
    ~Application() = default;

    // Delete copy operations
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * @brief Initialize all application components
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initialize();

    /**
     * @brief Run the main application loop
     * 
     * This function blocks and runs the main LED blink loop.
     * It does not return under normal operation.
     */
    void run();

private:
    /**
     * @brief Initialize hardware components (LEDs, buttons)
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initializeHardware();

    /**
     * @brief Initialize Bluetooth stack and managers
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initializeBluetooth();

    /**
     * @brief Initialize GATT services
     * @return Result indicating success or failure
     */
    [[nodiscard]] common::Result<> initializeServices();

    /**
     * @brief Setup event handlers and callbacks
     */
    void setupEventHandlers();

    /**
     * @brief Handle button state changes
     * @param button_state Current button state
     * @param has_changed Bitmask of changed buttons
     */
    void onButtonChanged(uint32_t button_state, uint32_t has_changed);

    /**
     * @brief Handle connection events
     * @param event Connection event type
     * @param conn Connection object (may be null)
     */
    void onConnectionEvent(bluetooth::ConnectionEvent event, bt_conn* conn);

    // Component instances
    hardware::LedController led_controller_;
    hardware::ButtonController button_controller_;
    bluetooth::AdvertisingManager advertising_manager_;
    bluetooth::ConnectionManager connection_manager_;
    bluetooth::SecurityManager security_manager_;
    services::LbsServiceHandler lbs_service_;
};

} // namespace nrf
