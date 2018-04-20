# LibLedger

## Overview

LibLedger is a threadsafe C library that allows software to interface with
[Ledger](https://www.ledgerwallet.com/) hardware wallets.

Before getting started, we strongly recommend you
read the [official Ledger documentation](https://ledger.readthedocs.io/en/latest/index.html) first.

## Supported devices

Currently supported devices:

- Ledger Blue
- Ledger Nano S

There are currently no plans to add support for older devices (such as the Ledger Nano or Ledger HW.1).

## Building

To build LibLedger you'll need the following dependencies:

- LibUSB (used by HID API)
- HID API (libhidapi-libusb)
- mbedTLS (for BOLOS support)

Then run the following commands:
```
$ meson builddir
$ ninja -C builddir
```
