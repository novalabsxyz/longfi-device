# LongFi Device Protocol

## Code Status

[![Build Status](https://travis-ci.com/helium/longfi-device.svg?token=35YrBmyVB8LNrXzjrRop&branch=master)](https://travis-ci.com/helium/longfi-device)

## About

This is a prototype for the user facing API of Helium LongFi Protocol. In its current form, it is hardly a protocol at all but some of the pieces are there to allow for in-field testing. It currently supports the following:
* sending packets at full power at SF10
* routing packets using OUI and device ID
* fragmenting packets such that TOA does not violate FCC
* channel hopping such that multiple fragments may be transmitted without waits

Notably, this library does **not**:
* configure TCXOs or handle antenna control
* receive packets
* encrypt packets
* encode packets
* throttle spreading factor
* throttle transmit power

## Usage

An example of the simple blocking client code can be seen in the [Arduino wrapper](https://github.com/helium/longfi-arduino/blob/master/src/LongFi.cpp).

The process of integrating this code into a project is as follows:

* *create system bindings*: the library needs access to some system functions as defined in `BoardBindings_t` in `board.h`. These are principally `SPI`, `GPIO`, and `Delay` function pointers. The current definitions have broad input parameters to match the demands of the SX12xx drivers; as we hone in on LongFi, we may be able to prune some of these. To use the library in its current state, all you really need are `spi_in_out` and `gpio_write` to control the `CHIP_SELECT` line. You must mock out all the rest of the functions so as not to dispatch a NULL function.
* *gather RfEvents*: the client must capture RfEvents, but currently we only care about `DIO0`. You can capture this event with simple polling or by making it interrupt generated. A robust integration should maintain a FIFO queue of events and dispatch them sequentially
* *dispatch RfEvents*: the client must dispatch the `longfi_handle_event` function with the events gathered above; a robust integration should dispatch this function at a low priority, allowing the system to respond to other higher priority events

## Testing

This library is designed to work on devices that communicate with 8 channel receivers, such as the SX1301/SX1302. We have [a stripped down "hotspot" image available here](https://drive.google.com/file/d/1JfqozFRjeRuGcBKE5RAwseC0yw9oUW9Q/view?usp=sharing), which runs on production hardware as well as on the [RAK "Blackspot"](https://www.adafruit.com/product/4327).

To copy the bootable sdcard.img onto the microSD card:

`$ sudo dd if=output/images/sdcard.img of=/dev/<microSD> bs=1M`

Insert into Raspberry Pi boot. Once you find the device on the network, you can login using SSH.

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
```
