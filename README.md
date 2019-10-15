# LongFi Device Protocol

[![Build Status](https://travis-ci.com/helium/longfi-device.svg?token=35YrBmyVB8LNrXzjrRop&branch=master)](https://travis-ci.com/helium/longfi-device)

## About

This is project leverages LongFi-Core and connects it to Semtech LoRa radios (SX12xx).

Currently, only uplink is supported.

## Building

1. Generate makefile in `build/`. You only need to do this once.
   ```
   cmake -H. -Bbuild -DBUILD_TESTING=OFF
   ```
   or to cross-compile for for arm
   ```
   cmake -H. -Bbuild -DBUILD_TESTING=OFF -DCMAKE_TOOLCHAIN_FILE=../toolchain-gcc-arm-none-eabi.cmake
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

An example of the simple blocking client code can be seen in the [Arduino wrapper](https://github.com/helium/longfi-arduino/blob/master/src/LongFi.cpp).

The process of integrating this code into a project is as follows:

* *create system bindings*: the library needs access to some system functions as defined in `BoardBindings_t` in `board.h`. These are principally `SPI`, `GPIO`, and `Delay` function pointers. The current definitions have broad input parameters to match the demands of the SX12xx drivers; as we hone in on LongFi, we may be able to prune some of these. To use the library in its current state, all you really need are `spi_in_out` and `gpio_write` to control the `CHIP_SELECT` line. You must mock out all the rest of the functions so as not to dispatch a NULL function.
* *gather RfEvents*: the client must capture RfEvents, but currently we only care about `DIO0`. You can capture this event with simple polling or by making it interrupt generated. A robust integration should maintain a FIFO queue of events and dispatch them sequentially
* *dispatch RfEvents*: the client must dispatch the `longfi_handle_event` function with the events gathered above; a robust integration should dispatch this function at a low priority, allowing the system to respond to other higher priority events
