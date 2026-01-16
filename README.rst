nRF Connect SDK: sdk-nrf
########################

.. contents::
   :local:
   :depth: 2

This repository contains the core of nRF Connect SDK, including subsystems,
libraries, samples, and applications.
It is also the SDK's west manifest repository, containing the nRF Connect SDK
manifest (west.yml).

Documentation
*************

Official latest documentation at https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/index.html

For earlier versions, open the latest version and use the drop-down under the title header.

---

```
export PATH=/opt/nordic/ncs/toolchains/322ac893fe/bin:$PATH && export ZEPHYR_TOOLCHAIN_VARIANT=zephyr && export ZEPHYR_SDK_INSTALL_DIR=/opt/nordic/ncs/toolchains/322ac893fe/opt/zephyr-sdk && west build --board nrf54l15dk/nrf54l15/cpuapp --no-sysbuild samples/bluetooth/peripheral_lbs && west flash
```