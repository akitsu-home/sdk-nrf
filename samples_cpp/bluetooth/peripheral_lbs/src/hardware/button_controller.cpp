/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "button_controller.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <dk_buttons_and_leds.h>
}

namespace nrf::hardware {

// Static instance for singleton
static ButtonController* s_instance = nullptr;

ButtonController& ButtonController::instance() {
    static ButtonController instance;
    return instance;
}

common::Result<> ButtonController::initialize() {
    if (initialized_) {
        return common::Result<>::ok();
    }

    s_instance = this;

    int err = dk_buttons_init(buttonChangedHandler);
    if (err) {
        printk("Buttons init failed (err %d)\n", err);
        return common::Result<>::error(err);
    }

    initialized_ = true;
    printk("Buttons initialized\n");
    return common::Result<>::ok();
}

void ButtonController::setCallback(ButtonCallback callback) {
    callback_ = std::move(callback);
}

bool ButtonController::isPressed(uint32_t button_mask) const {
    return (current_state_ & button_mask) != 0;
}

void ButtonController::buttonChangedHandler(uint32_t button_state, uint32_t has_changed) {
    if (s_instance) {
        s_instance->current_state_ = button_state;
        if (s_instance->callback_) {
            s_instance->callback_(button_state, has_changed);
        }
    }
}

} // namespace nrf::hardware
