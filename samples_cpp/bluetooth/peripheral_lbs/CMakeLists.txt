#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(peripheral_lbs_cpp LANGUAGES CXX)

# NORDIC SDK APP START
target_sources(app PRIVATE
  src/main.cpp
)

# Enable C++ support
set_target_properties(app PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# NORDIC SDK APP END
