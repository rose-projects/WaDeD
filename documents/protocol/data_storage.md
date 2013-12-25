# Data storage

## Introduction

From the use cases we see emerging the following needs for our product:

- send a format-free message, to one or more people. This message will be 
relayed by any stone that comes across a stone that is already holding 
the message.

- send a formated message, with for instance a date and a place (meeting),
information about a stone (battery state), the last interaction between a
stone and another specific stone, or an acknowledgement of such a message.

- install new firmware and reboot on it.

- manage memory, erase expired messages, update infos.

Everything that transits through the network can be divided into two
categories: local messages, sent to the device by a fixed stone, which
need no further retransmission, and data that need to be transmitted on the
network. Two devices that meet need not worry about the local data the
other is holding. They do need to know what "propagating data" the
other has.

Inside those categories we can again make a division: some data will need
to be updated (information about the battery of a chosen stone), while
other will not (meetings, generic messages). This is not only an
opportunity to gain memory space: it would actually be nocive to keep and
relay outdated information.

## Transmission

Using a transparent mode-like transmission would be chaotic and is not
suitable for network applications. We need to structure our data (which RF
modules already do anyways). We see that we have varying types of data:
firmware code, structured messages, unstructured messages, etc.

In front of that need, we can use two approaches. The first one is to 
have several kinds of packets, representing different kinds of data. This
is inappropriate, mainly for two reasons:
- There will always be a need to differenciate kinds of packets.
This will be done with some sort of identifier, which we could as
well embed into a generic type of packet.
- This greatly lacks flexibility. Further evolution of the protocol
will need to make important update on the lower layers of
communication. This is something we want to avoid.

The other approach is to use a generic packet type with the necessary 
information, and specify the nature of the communication in the header.
The XBee 802.15.4 module API mode specification gives us a good example of
that kind of approach. They are structured as follow:

        ------------------------------------------------
        |   start   | total  | API-specific |          |
        | delimiter | length |  structure   | checksum |
        ------------------------------------------------

with the API-specific structure being itself divided in two:

        -----------------------------
        | command ID | command data |
        –––––––––––––––––––––––––––––

This model has good properties in regard to our needs: structure and
flexibility.

## Storing packets

So knowing that we want to exchange packets and having a rough idea of what
they will look like, we want to know what data to store. The question of
how to store it for efficient treatment and retrieval will come later.
The IP protocol segregates data into small chunks, IP packets, then sends
them on the network. Upon receival, they are re-assembled into the original
data. This suits large data transmission, but if we can skip this phase,
we benefit from a simpler protocol and (a bit) less CPU use. So we will
adopt this convention: one transferred packet = one packet in memory. When
a stone needs to send a memory packet, simply add what is needed for over
the air transmission (delimiters, checksum,…) and send it to the radio.

Packets all are a form of message, whether they come from another user that
is sending a message to this stone or is using using it as a relay, from a
device plugged to the stone or from the stone itself. Therefore they should
carry at least who the intended receiver of the packet is, and who the
emitter is. Packets should not be able to live forever, so there should be
a time notion in their registering, such as an expiration timestamp, or
an emission timestamp and a relative value for the expiration date of the
packet. The emission date is always a good thing to have for a message, so
we will rather adopt this option. A hop limit (limit to the number of
retransmissions of the same message) can be a nice thing to have, as well
as a checksum of the content, but this one may be left unstored.

In order to compare whether two stones carry the same message or not, they
should be uniquely identified, so a field should be dedicated to the
message ID. This can be achieved through using a hash function H, and using
H(source address || destination address || content of the message) as the
ID (where || is the concatenation). This is somewhat similar to what Git
uses to generate commit hashes.

Finally we want to identify the kind of communication we are storing. As
mentioned above, this can be achieved by storing the equivalent of a XBee
Command ID, ie. a field indicating the nature of the content.

This being stored, we can store the message in itself, according to a
format following the convention chosen for the Command ID.

## Storing stone data

In addition to storing packets, a stone must keep certain data that it
might have to provide or to use. This includes updates of the firmware,
battery levels, specific messages to provide to stones it encounters,
parameters, and a list of the last times it saw other stones. Other stones
should be able to access these through radio communication, but only when
they have the right to do so - we may not want everybody to be able to
know who was here last.

## Transfer packets structure

In consideration to what we have previously said, a packet should be
structured as follow:

        -------------------------------------------
        |   start   | packet |         | payload  |
        | delimiter | header | payload | checksum |
        -------------------------------------------

With the packet header:

        ---------------------------------------------------------
        | protocol | packet | address | time  |  hop  | payload |
        | version  |   ID   |  block  | block | limit | length  |
        ---------------------------------------------------------

The address block:

        -------------------------
        | source  | destination |
        | address |   address   |
        -------------------------

The time block:

        -------------------------
        | emission | expiration |
        |   date   |    time    |
        -------------------------
        
And the payload:
        
        ------------------------------
        |    type    | type-specific |
        | identifier |    content    |
        ------------------------------

Field descriptions:

- **start delimiter (16 bits?)** 
fixed string that identifies the start of a packet.
Size to define. 16 bits should be enough to avoid false positives.

- **protocol version (4 bits)**
fixed string identifying the protocol version. Offers the possibility of
retro-compatibility in future versions.

- **packet ID (64 bits)**
identify uniquely the packet on the network. Size considers we use a truncated
version of the CRC hash provided by the STM32L MCU.

- **source address (16 bits)**
address uniquely identifying the sender on the network.

- **destination address (16 bits)**
address uniquely identifying the receiver on the network.

- **emission date (32 bits)**
the Unix timestamp of the emission of the message.

- **expiration time (20 bits)**
the packet expires when the current Unix timestamp is greater than
emission date + expiration time.

- **hop limit (8 bits)**
decreases every time the packet is transferred. If it reaches 0, the packet
is destroyed.

- **payload length (8 bits)**
total length of the payload block.

- **type identifier (8 bits)**
type of communication. The protocol should specify the existing types, the
associated type identifier values, and the structure of the content.

- **type-specific content (max 1160 bits)**
content of the message. Should be structured as specified according to the
type identifier.

- **payload checksum (16 bits)**
checksum. Is used to ensure the packet was well transmitted.

## To do
        - Transfer packets structure
