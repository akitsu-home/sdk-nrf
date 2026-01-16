.. _peripheral_lbs_cpp:

Bluetooth: Peripheral LBS (C++)
###############################

.. contents::
   :local:
   :depth: 2

The Peripheral LBS (C++) sample demonstrates how to use the :ref:`lbs_readme` to implement a BLE peripheral with LED and Button functionality using C++.

This sample is a C++ port of the :ref:`peripheral_lbs` sample.

Requirements
************

The sample supports the following development kits:

.. table-from-sample-yaml::

Overview
********

This sample is functionally equivalent to the C version of the Peripheral LBS sample.
The main differences are:

* Source code is written in C++ (C++17)
* Uses C++ features like:
  
  * Anonymous namespaces for internal linkage
  * ``constexpr`` for compile-time constants
  * ``nullptr`` instead of ``NULL``
  * C++ standard headers (``<cstddef>``, ``<cstring>``, ``<cerrno>``)

* Zephyr and nRF Connect SDK C headers are wrapped in ``extern "C"`` blocks

When connected, the sample exposes the LED Button Service (LBS) GATT service, which allows a connected device to:

* Control LED 3 (USER LED)
* Read the state of Button 1

LED 1 blinks with a period of 2 seconds to indicate that the system is running.
LED 2 is on when connected to a central device.

User interface
**************

LED 1:
   Blinks with a period of 2 seconds, duty cycle 50%, when the main loop is running.

LED 2:
   On when connected.

LED 3:
   Controlled remotely from the connected device.

Button 1:
   State can be read remotely from the connected device.

Building and running
********************

.. |sample path| replace:: :file:`samples_cpp/bluetooth/peripheral_lbs`

.. include:: /includes/build_and_run.txt

Testing
*******

After programming the sample to your development kit, you can test it with the nRF Connect for Mobile app.

1. Power on your development kit.
#. Connect to the device from nRF Connect for Mobile (the device is advertising as "Nordic_LBS").
#. Observe that LED 2 turns on.
#. In the LED Button Service, enable notifications for the Button characteristic.
#. Press Button 1 on the kit and observe the notification in the app.
#. Write a value to the LED characteristic to control LED 3.

Dependencies
************

This sample uses the following |NCS| libraries:

* :ref:`lbs_readme`
* :ref:`dk_buttons_and_leds_readme`

In addition, it uses the following Zephyr libraries:

* :ref:`zephyr:bluetooth_api`:

  * ``include/bluetooth/bluetooth.h``
  * ``include/bluetooth/conn.h``
  * ``include/bluetooth/uuid.h``
  * ``include/bluetooth/gatt.h``
