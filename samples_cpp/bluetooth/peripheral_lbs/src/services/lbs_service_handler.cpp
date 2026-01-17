/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "lbs_service_handler.hpp"
#include "../hardware/led_controller.hpp"
#include "../hardware/button_controller.hpp"
#include "../config/constants.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <bluetooth/services/lbs.h>
}

namespace nrf::services {

// Static instance for singleton
static LbsServiceHandler* s_instance = nullptr;

LbsServiceHandler& LbsServiceHandler::instance() {
    static LbsServiceHandler instance;
    return instance;
}

common::Result<> LbsServiceHandler::initialize(
    hardware::LedController& led_controller,
    hardware::ButtonController& button_controller) {
    
    if (initialized_) {
        return common::Result<>::ok();
    }

    s_instance = this;
    led_controller_ = &led_controller;
    button_controller_ = &button_controller;

    // Setup LBS callbacks
    lbs_callbacks_.led_cb = ledCallbackHandler;
    lbs_callbacks_.button_cb = buttonCallbackHandler;

    int err = bt_lbs_init(&lbs_callbacks_);
    if (err) {
        printk("Failed to init LBS service (err %d)\n", err);
        return common::Result<>::error(err);
    }

    initialized_ = true;
    printk("LBS service initialized\n");
    return common::Result<>::ok();
}

void LbsServiceHandler::setLedCallback(LedCallback callback) {
    led_callback_ = std::move(callback);
}

void LbsServiceHandler::setButtonStateCallback(ButtonStateCallback callback) {
    button_state_callback_ = std::move(callback);
}

common::Result<> LbsServiceHandler::sendButtonState(bool state) {
    int err = bt_lbs_send_button_state(state ? 1 : 0);
    if (err) {
        // Note: This can fail if not connected, which is normal
        return common::Result<>::error(err);
    }
    return common::Result<>::ok();
}

void LbsServiceHandler::ledCallbackHandler(bool led_state) {
    if (!s_instance) {
        return;
    }

    // Call custom callback if set
    if (s_instance->led_callback_) {
        s_instance->led_callback_(led_state);
    }

    // Default behavior: control the user LED
    if (s_instance->led_controller_) {
        s_instance->led_controller_->set(config::LedConfig::kUser, led_state);
    }
}

bool LbsServiceHandler::buttonCallbackHandler() {
    if (!s_instance) {
        return false;
    }

    // Call custom callback if set
    if (s_instance->button_state_callback_) {
        return s_instance->button_state_callback_();
    }

    // Default behavior: return button state from controller
    if (s_instance->button_controller_) {
        return s_instance->button_controller_->isPressed(config::ButtonConfig::kUserMask);
    }

    return false;
}

} // namespace nrf::services
