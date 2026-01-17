/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "advertising_manager.hpp"
#include "../config/constants.hpp"

extern "C" {
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <bluetooth/services/lbs.h>
}

namespace nrf::bluetooth {

// Static instance for singleton
static AdvertisingManager* s_instance = nullptr;

// Advertising data
static const bt_data s_ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, config::kDeviceName, config::kDeviceNameLen),
};

static const bt_data s_sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_LBS_VAL),
};

AdvertisingManager& AdvertisingManager::instance() {
    static AdvertisingManager instance;
    return instance;
}

common::Result<> AdvertisingManager::initialize() {
    if (initialized_) {
        return common::Result<>::ok();
    }

    s_instance = this;
    k_work_init(&adv_work_, advertisingWorkHandler);

    initialized_ = true;
    printk("Advertising manager initialized\n");
    return common::Result<>::ok();
}

void AdvertisingManager::start() {
    k_work_submit(&adv_work_);
}

common::Result<> AdvertisingManager::stop() {
    int err = bt_le_adv_stop();
    if (err) {
        printk("Advertising failed to stop (err %d)\n", err);
        return common::Result<>::error(err);
    }

    advertising_ = false;
    printk("Advertising stopped\n");
    return common::Result<>::ok();
}

bool AdvertisingManager::isAdvertising() const {
    return advertising_;
}

void AdvertisingManager::advertisingWorkHandler(k_work* work) {
    if (s_instance) {
        s_instance->doStartAdvertising();
    }
}

int AdvertisingManager::doStartAdvertising() {
    int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, 
                               s_ad, ARRAY_SIZE(s_ad),
                               s_sd, ARRAY_SIZE(s_sd));
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        advertising_ = false;
        return err;
    }

    advertising_ = true;
    printk("Advertising successfully started\n");
    return 0;
}

} // namespace nrf::bluetooth
