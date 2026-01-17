/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "application.hpp"
#include "config/constants.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/settings/settings.h>
}

namespace nrf {

common::Result<> Application::initialize() {
    printk("Starting Bluetooth Peripheral LBS sample (C++ modular)\n");

    // Initialize hardware
    auto result = initializeHardware();
    if (!result) {
        printk("Hardware initialization failed\n");
        return result;
    }

    // Initialize Bluetooth
    result = initializeBluetooth();
    if (!result) {
        printk("Bluetooth initialization failed\n");
        return result;
    }

    // Initialize services
    result = initializeServices();
    if (!result) {
        printk("Services initialization failed\n");
        return result;
    }

    // Setup event handlers
    setupEventHandlers();

    // Start advertising
    advertising_manager_.start();

    printk("Application initialized successfully\n");
    return common::Result<>::ok();
}

void Application::run() {
    int blink_status = 0;

    while (true) {
        led_controller_.set(config::LedConfig::kRunStatus, (++blink_status) % 2);
        k_sleep(K_MSEC(config::TimingConfig::kRunLedBlinkIntervalMs));
    }
}

common::Result<> Application::initializeHardware() {
    // Initialize LEDs
    auto result = led_controller_.initialize();
    if (!result) {
        return result;
    }

    // Initialize buttons
    result = button_controller_.initialize();
    if (!result) {
        return result;
    }

    return common::Result<>::ok();
}

common::Result<> Application::initializeBluetooth() {
    // Initialize security manager first (registers callbacks before bt_enable)
    if constexpr (bluetooth::SecurityManager::isEnabled()) {
        auto result = security_manager_.initialize();
        if (!result) {
            return result;
        }
    }

    // Enable Bluetooth
    int err = bt_enable(nullptr);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return common::Result<>::error(err);
    }
    printk("Bluetooth initialized\n");

    // Load settings if enabled
    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    // Initialize advertising manager
    auto result = advertising_manager_.initialize();
    if (!result) {
        return result;
    }

    // Initialize connection manager
    result = connection_manager_.initialize(advertising_manager_);
    if (!result) {
        return result;
    }

    return common::Result<>::ok();
}

common::Result<> Application::initializeServices() {
    return lbs_service_.initialize(led_controller_, button_controller_);
}

void Application::setupEventHandlers() {
    // Setup button callback
    button_controller_.setCallback(
        [this](uint32_t button_state, uint32_t has_changed) {
            onButtonChanged(button_state, has_changed);
        });

    // Setup connection callback
    connection_manager_.setCallback(
        [this](bluetooth::ConnectionEvent event, bt_conn* conn) {
            onConnectionEvent(event, conn);
        });
}

void Application::onButtonChanged(uint32_t button_state, uint32_t has_changed) {
    if (has_changed & config::ButtonConfig::kUserMask) {
        bool pressed = (button_state & config::ButtonConfig::kUserMask) != 0;
        lbs_service_.sendButtonState(pressed);
    }
}

void Application::onConnectionEvent(bluetooth::ConnectionEvent event, bt_conn* conn) {
    switch (event) {
        case bluetooth::ConnectionEvent::Connected:
            led_controller_.turnOn(config::LedConfig::kConnectionStatus);
            break;

        case bluetooth::ConnectionEvent::Disconnected:
            led_controller_.turnOff(config::LedConfig::kConnectionStatus);
            break;

        case bluetooth::ConnectionEvent::Recycled:
            // Advertising restart is handled by connection manager
            break;
    }
}

} // namespace nrf
