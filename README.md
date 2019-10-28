# LongFi Device Protocol

[![Build Status](https://travis-ci.com/helium/longfi-device.svg?token=35YrBmyVB8LNrXzjrRop&branch=master)](https://travis-ci.com/helium/longfi-device)

## About

This project leverages [LongFi-Core](https://github.com/helium/longfi-core) as a protocol layer and connects it to Semtech LoRa radios (SX12xx).

Currently, frequency and transmit power are controlled within this library, but those responsibilities are intended to migrate to the protocol layer.

Currently, only uplink is supported.

## Building

1. Generate makefile in `build/`. You only need to do this once.
   ```
   cmake -H. -Bbuild
   ```
   or to cross-compile for armv6s-m (such as STM32L0xx)
   ```
   cmake -H. -Bbuild  -DCMAKE_TOOLCHAIN_FILE=../toolchain-gcc-arm-none-eabi.cmake -DCMAKE_FLAGS="-march=armv6s-m"
   ```
1. Compile
   ```
   make -C build
   ```
1. Documentation
   ```
   make -C build docs
   open build/docs/html/index.html
   ```


## Usage

Several libraries demonstrate usage of this library:
* [longfi-st-hal](https://github.com/helium/longfi-st-hal): a simple bare metal superloop for STM32L0XX
* [longfi-device-rs](https://github.com/helium/longfi-device-rs/): a bare metal Rust binding, including example and Board Support Crate references in README
* [longfi-arduino](https://github.com/helium/longfi-arduino): an Arduino binding, simplifying usage for several boards with Arduino board support

The process of integrating this code into a project is as follows:

* *create system bindings*: the library needs access to some system functions as defined in `BoardBindings_t` in `board.h`:
```
 typedef struct
 {
     // must provide for drivers to work
     uint8_t (*spi_in_out)(LF_Spi_t * obj, uint8_t outData);
     void (*spi_nss)(bool sel);
     void (*reset)(bool enable);
     void (*delay_ms)(uint32_t);
     uint32_t (*get_random_bits)(uint8_t);
     bool (*busy_pin_status)(); // only necessary for SX126x
                                // optional board control
     // if external PA, this callback allows you to reduce power
     // of SX12xx and to do any PA switching
     uint8_t (*reduce_power)(uint8_t);
     uint8_t (*set_board_tcxo)(
         bool enable); // to control power supply TCXO (wake/sleep)
     void (*set_antenna_pins)(
         AntPinsMode_t mode,
         uint8_t       power); // to control antenna pins for TX/RX/Sleep

 } BoardBindings_t;
```
* *gather RfEvents*: the client must capture RfEvents, but currently we only care about `DIO0` for SX127x and `DIO1` for SX126x. You can capture this event with simple polling or by making it interrupt generated. A robust integration should maintain a FIFO queue of events and dispatch them sequentially
* *dispatch RfEvents*: the client must dispatch the `longfi_handle_event` function with the events gathered above; a robust integration should dispatch this function at a low priority, allowing the system to respond to other higher priority events
* *send message*: the client can send messages, but must take care to only dispatch a single send at time; `ClientEvent_TxDone` signals that the library is ready to send again

## Testing

You can setup a hotspot for testing by following [this tutorial](https://developer.helium.com/hotspot/developer-setup).

You need to start the LongFi protocol layer: `/usr/bin/concentrate longfi &`

And then you can tail the messages, including raw radio packets with: `tail -f /var/data/log/messages`

```
// raw LoRa packet
Sep 12 17:48:19 gateway user.debug concentrate[176]: received RxPacketLoRa { freq: 916800000, if_chain: 6, crc_check: Pass, timestamp: 929.661612s, radio: R0, bandwidth: BW125kHz, spreading: SF9, coderate: 4/5, rssi: -118.0, snr: 7.5, snr_min: 4.5, snr_max: 9.25, crc: 48637, payload: [0, 2, 0, 0, 0, 40, 40, 254, 239, 247, 146, 22, 133, 85, 181, 183, 12, 126, 112, 0, 11, 0, 0, 9, 74] }
// parsed LongFi packet
Sep 12 17:48:19 gateway user.debug concentrate[6713]: [LongFi][app] Packet received: LongFiPkt { oui: 0x2, device_id 0x2828, packet_id: 0x0, mac: 0xeffe, quality: O, crc_fails: 0, payload: [247, 146, 22, 133, 85, 181, 183, 12, 126, 112, 0, 11, 0, 0, 9, 74] }
// delivered to theorical client
Sep 12 17:48:19 gateway user.debug concentrate[6713]: [LongFi][app] Sending to client
```

Alternatively, you can put up a client to the longfi layer: ` /usr/bin/concentrate longfi-test`
```
Received packet! Length = 16
248 18 22 133 85 181 183 12 126 112 0 11 0 0 9 82 
