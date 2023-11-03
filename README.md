[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE.md)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](./CODE_OF_CONDUCT.md)

# A BLE controlled Blinky for nRF52840 DK

> Copyright (c) 2023, Greg PFISTER. MIT License.

## About

This firmware allow to control the led 0 on the nRF52840 development kit using
a BLE service (`826c9400-8f2f-4dc5-8319-d07b584cf83e`) which exposes 2
characteristics:
- `04df2644-e6b8-4541-8a7f-cecf67f365fe` (readable/writable) the sequence of 
  changes, in the form of 8 consecutive time interval (from 0 to 65535ms)
- `24b35ad0-d0f0-4811-bdfb-16d4451a514f` (readable) to inform about the led
  status.

The default sequence is: 100ms, 100ms, 100ms, 100ms, 100ms, 500ms, 500ms, 500ms,
(3 rapid short blink, 1 spaced long blink).

On the development kit, the led 1 is used to display the BLE connection status 
(on: connected, off: disconnected).

## Supported boards

Board used for development and testing:
- Nordics nRF52840 USB Dongle
- Nordics nRF52840 DK

## Test BLE

Ideally, use the `nRF Connect for Desktop Bluetooth Low Energy`. Scan for a 
device named `Greg's nRF52840 Blinky`.

Alternatively, use an app like `LightBlue`.

## References

If you are interested into bringing BLE to your existing nRF Connect SDK,
please have a look at the following material:
- [Developing Bluetooth Low Energy products using nRF Connect SDK](https://youtu.be/hY_tDext6zA?si=ptoFH2iMeS5JuhbJ)

## Requirements

- A nRF52840 development kit
- nRF Command Line tool Tools (test on v2.5.0)
- nFR Connect Desktop
- Visual Studio Code + nrf Connect for VS Code

## Contributions

Contributions are welcome, however please read our 
[code of conduct](./CODE_OF_CONDUCT.md).

## License

You can get a copy of the license [here](./LICENSE.md).
