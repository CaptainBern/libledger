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

Send an APDU to the device.

```
 0                   1
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Sequence ID          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Sequence ID**: 16 bits

The packet Sequence ID. For the first packet this value is set to 0.

The remainder of the packet is APDU data. Note that only the last packet
can be padded with zeroes.
