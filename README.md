[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE.md)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](./CODE_OF_CONDUCT.md)

# A BLE controlled Blinky for nRF52840 DK

> Copyright (c) 2023, Greg PFISTER. MIT License.

## About

This firmware allow to control the leds on the nRF52840 development kit using
a BLE service (`826c9400-8f2f-4dc5-8319-d07b584cf83e`) which exposes 3
characteristics:
- `04df2644-e6b8-4541-8a7f-cecf67f365fe` (writable) to set the time interval 
  from 0 to 65535ms (2 bytes) between each led changes.
- `7656d6e9-46da-425a-8c9e-4fa2becdf619` (writable) to set the leds on/off 
  toggling sequence. The sequence is compose of 8 bytes, on for each of the led 
  (from 0 to 3) which would be turned on/off after waiting for the time 
  interval.
- `24b35ad0-d0f0-4811-bdfb-16d4451a514f` (readable) to inform about the leds
  status.

It only changes the state of one led each time.

The default time interval is 250ms.

The default sequence is: `led 0`, `led 0`, `led 1`, `led 1`, `led 2`, `led 2`,
`led 3`, `led 3`.

## Test BLE

To test, use an app like `LightBlue` to scan for a device named
`Greg's nRF52840 Blinky` which advertises the service 
`fefb5d60-994e-4d19-be43-d7849a807198`.

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
