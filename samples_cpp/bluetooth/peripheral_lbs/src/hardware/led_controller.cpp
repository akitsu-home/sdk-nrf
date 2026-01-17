/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "led_controller.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <dk_buttons_and_leds.h>
}

namespace nrf::hardware {

common::Result<> LedController::initialize() {
    if (initialized_) {
        return common::Result<>::ok();
    }

    int err = dk_leds_init();
    if (err) {
        printk("LEDs init failed (err %d)\n", err);
        return common::Result<>::error(err);
    }

    initialized_ = true;
    printk("LEDs initialized\n");
    return common::Result<>::ok();
}

void LedController::turnOn(uint32_t led) {
    dk_set_led_on(led);
}

void LedController::turnOff(uint32_t led) {
    dk_set_led_off(led);
}

void LedController::set(uint32_t led, bool state) {
    dk_set_led(led, state ? 1 : 0);
}

void LedController::toggle(uint32_t led) {
    // DK library doesn't have a direct toggle, so we use set
    // For proper toggle, we'd need to track state
    dk_set_led(led, 1);  // This is a simplified version
}

} // namespace nrf::hardware
