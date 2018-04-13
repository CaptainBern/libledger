# ADPU

## Overview

http://www.embedx.com/pdfs/ISO_STD_7816/info_isoiec7816-4%7Bed2.0%7Den.pdf

Ledger devices use [pseudo-ADPUs](https://en.wikipedia.org/wiki/Smart_card_application_protocol_data_unit) to communicate.
This protocol is similar to the one described in [ISO-7816](http://cardwerk.com/iso-7816-part-4/)
but it is not completely standard-compliant.

## Structure

Command ADPU:

| Field Name | Length | Description      |
|------------|--------|------------------|
| CLA        |      1 | Class byte       |
| INS        |      1 | Instruction byte |
| P1         |      1 | First parameter  |
| P2         |      1 | Second parameter |
| Length     |      1 | Data length      |
| Data       | Length | ADPU Data        |
