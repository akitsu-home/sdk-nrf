#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(peripheral_lbs_cpp LANGUAGES CXX)

# NORDIC SDK APP START

# Main application
target_sources(app PRIVATE
  src/main.cpp
  src/application.cpp
)

# Hardware module
target_sources(app PRIVATE
  src/hardware/led_controller.cpp
  src/hardware/button_controller.cpp
)

# Bluetooth module
target_sources(app PRIVATE
  src/bluetooth/advertising_manager.cpp
  src/bluetooth/connection_manager.cpp
  src/bluetooth/security_manager.cpp
)

# Services module
target_sources(app PRIVATE
  src/services/lbs_service_handler.cpp
)

# Include directories
target_include_directories(app PRIVATE
  src
)

# Enable C++ support
set_target_properties(app PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# NORDIC SDK APP END
