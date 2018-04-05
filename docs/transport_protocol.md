# Ledger Transport Protocol

## Overview

The Ledger Transport Protocol is the second layer in the Ledger
Protocol Stack, just above USB HID:

```
+---------------+
|     APDU      |
+---------------+
|      LTP      |
+---------------+
|    USB HID    |
+---------------+
```

The LTP is meant to provide a reliable ledger-to-host communication
protocol. It takes care of allocating communication channels, verifying
the data-link and framing APDUs. The LTP uses big-endian.

## Header Format

The LTP consists of 64-byte packets. A packet is either a command or
a reply.

Commands and replies share a common header:

```
 0                   1                   2
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Communication Channel ID    |  Command Tag  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Communication Channel ID**: 16 bits

The LTP allows for basic command multiplexing via 'Channel IDs'. A
channel ID is nothing more than a random 16-bit number. You should
allocate a channel ID through the dedicated ALLOCATE_CHANNEL command.
The default channel ID is 0x0101.

The channel ID is only used for APDU commands.

**Command Tag**: 8 bits

This field is present in both commands and replies (even though its name
is 'Command Tag'). The remaining data in the packet depends on this value:

```
+------------------+---------+
| GET_VERSION      |    0x00 |
+------------------+---------+
| ALLOCATE_CHANNEL |    0x01 |
+------------------+---------+
| PING             |    0x02 |
+------------------+---------+
| APDU             |    0x05 |
+------------------+---------+
```

The remainder of the packet is padded with zeroes to fill the 64
byte buffer.

## GET_VERSION

Query the LTP version.

Reply format:

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            Version                            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Version**: 32 bits

The LTP version, should be 0.

## ALLOCATE_CHANNEL

Allocate a communication channel ID.

Reply format:

```
 0                   1
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Communication Channel ID    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Communication Channel ID**: 16 bits

A channel ID that can be used to multiplex APDU commands.

## PING

Check the data-link. PING commands/replies do not have any data.

## APDU

Send or receive an [APDU](./apdu.md) to/from the device. APDUs need
to be framed because they can exceed 64 bytes. *Every* APDU needs to
be framed, whether it exceeds 64 bytes in length or not. When an APDU
fits in a single LTP packet, it can be send using a single packet.

Format:

```
 0                   1
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Sequence ID          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+       APDU Payload Part       +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Sequence ID**: 16 bits

The packet Sequence ID, starts at 0.

**APDU Payload Part**: remainder of the packet

Part of an [APDU payload](#apdu-payload).

## APDU Payload

An APDU payload is formatted as follows:

```
 0                   1
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          APDU Length          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+              APDU             +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**APDU Length**: 16 bits

The total length of the APDU.

**APDU**: Length equal to *APDU Length* in bytes

The actual APDU, as described [here](./apdu.md).

### APDU Payload Framing

APDU payloads are split up in several smaller parts to be
able to transmit them using the LTP.

After you made your APDU payload you should split it up in parts of
maximum 59<sup id="a1">[1](#f1)</sup> bytes each. Send each part with a [LTP APDU](#apdu) packet.
Make sure to send the parts in the correct order, and to increase the
`Sequence ID` by 1 for each part.

It is important to notice that the `Payload Part` of the first [LTP APDU](#apdu) packet
contains the `APDU Length`, which you must use to check whether or not you
have received all of the APDU payload parts.

<b id="f1">1</b> 59 bytes -> 64 (LTP packet size) - 5 (communication channel ID + command tag + [APDU](#apdu) sequence id) [↩](#a1)
