[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE.md)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](./CODE_OF_CONDUCT.md)

# A BLE controlled Blinky for nRF52840 DK

> Copyright (c) 2023, Greg PFISTER. MIT License.

## About

This firmware allow to control the led 0 on the nRF52840 development kit using
a BLE service (`826c9400-8f2f-4dc5-8319-d07b584cf83e`) which exposes 2
characteristics:
- `04df2644-e6b8-4541-8a7f-cecf67f365fe` (readable) the actual selected 
  sequence for the blinking light.
- `3ed3db80-4fdb-461b-ad850-cf0764566488` (writable w/out response) the new 
  selected sequence for the blinking light.
- `24b35ad0-d0f0-4811-bdfb-16d4451a514f` (readable) to inform about the led
  status.

(All readable characteristics support notifications.)

8 sequences are defined for the blinking led:

1. 16 random (between 100 and 3000ms)
2. 100ms on, 100ms off
3. 500ms on, 500ms off
4. 1s on, 1s off
5. 2s on, 2s off
6. 3s on, 3s off 
7. 4s on, 4s off
8. S.O.S: 3 short blinks (100ms), 3 long blink (500ms), 3 short blinks (100ms) 
   again and long pause 2s

On the development kit, the led 1 is used to display the BLE connection status 
(on: connected, off: disconnected).

## Supported boards

Board used for development and testing:
- Nordics nRF52840 USB Dongle (board: `nrf52840dongle_nrf52840`) via a specific
  [overlay](./app/nrf52840dongle_nrf52840.overlay)
- Nordics nRF52840 DK (board: `nRF52840dk_nrf52840`) via a specific
  [overlay](./app/nrf52840dk_nrf52840.overlay)

## Getting started

### West workspace

The simplest method is to run the following command:

```sh
west init -m git@github.com:gpfister/gp-nrf-ble-blinky.git --mr master gp-nrf-ble-blinky-workspaceest
```

However this wouldn't work if using `git+ssh` with ssh custom paramters.

Alternatively you may create a workspace folderg, clone the repository, add the 
`west` confi and update `west`:

First, get the sources:

```sh
mkdir <WORKSPACE_FOLDER>
cd <WORKSPACE_FOLDER>
git clone git@github.com:gpfister/gp-nrf-ble-blinky.git -c core.sshCommand="ssh -i ~/.ssh/gpfister.github -o IdentitiesOnly=yes"
cd gp-nrf-ble-blinky
```

Then, you can initialise the repository:
```sh
cd gp-nrf-ble-blinky
west init -l
cd ..
west update -o=--depth=1 -n
```

### Build and flash via command line

To build:

```sh
export BOARD=<SUPPORTED_BOARD>
west build -b $BOARD app
```

To flash:

```sh
west flash
```

### Build and flash via `VS Code`

1. Open the workspace in `VS Code`.
2. In the `nRF Connect for VS Code` plugin, run the 
   `west update -o=--depth=1 -n`.
3. In the `nRF Connect for VS Code` plugin, select the app located in 
   `gp-nrf-ble-blinky/app` folder.
4. Create a building configuration picking any of the supported board (see 
   above).

## Unit testing

To run unit tests:

```sh          
CMAKE_PREFIX_PATH=/opt/nordic/ncs/toolchains/20d68df7e5/opt/ west twister -T gp-nrf-ble-blinky/app -v --inline-logs --integration
CMAKE_PREFIX_PATH=/opt/nordic/ncs/toolchains/20d68df7e5/opt/ west twister -T gp-nrf-ble-blinky/tests --integration
```

## Test BLE

Ideally, use the `nRF Connect for Desktop Bluetooth Low Energy`. Scan for a 
device named `Greg's nRF Blinky`.

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
