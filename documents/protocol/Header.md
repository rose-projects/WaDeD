# WPv1 packet header
We want to have fixed-size headers. This makes emission and reception more
straightforward. Future compatibility is assured by prefixing the packet
with the version number of the protocol (on 4 bits), so that modules can
agree on which version to use.
Since our goal is to exchange short messages (tweets), we can reasonably
decide that a message will fit on a single packet, thus eliminating the
issues of dividing and reuniting the messages into packets.

Non-ordered informations we want to have in the header:
- **version number (4 bits)**
	In any given version of the protocol, these bits are constant. It
	helps both identifying the beginning of a WPv1 packet, and assures
	the possibility for retro-compatibility in future versions.
- **total length of the packet (8 bits)**
	This helps the receiver record the message correctly. Since the
	packet will not exceed 200 characters, we need no more than 8 bits.
- **locality (2 bits)**
	This tells us whether we are sending "local" packets (which will
        not be retransmitted). By default, this field is set at 0x00.
        A module willing to emit a local packet will set it at 0x03, then
        the recieving module will save it at 0x02, indicating the packet
        is not to be sent again.
- **packet id (160 bits)**
        Packets need to be identified on the network, in order to ease
        synchonization between modules, hence the need for a distinctive
        id. Here we choose the SHA-1 hash of the sender, the reciever and
        the message, thus identifying uniquely a communication.
        *Todo* : decide if the hash should be time-dependant (this does not
        change the size of the field).
- **source address (16 bits)**
        The mac address of the sender.
- **destination address (16 bits)**
        The mac address of the final reciever. We can select an arbitrary
        value (such as 0xFFFF) to use broadcast diffusion instead.
- **emission time (32 bits)**
        Unix timestamp of the emission.
- **expiration time (20 bits)**
        The packet will expire at emission time + expiration time. When
        the system realizes that the current timestamp is higher than the
        emission date, the message gets erased from the database.
        Note that this allows two interpretation: the system can have an
        internal clock tracking the time constanly and erasing its messages
        periodically (on periodic wakeup for instance), or it can simply
        be communicated the date whenever it meets a mobile device and act
        on that date.
- **checksum of the message (16 bits)**
        Allows for a verification that the content of the packet has been
        succesfully transmitted. If the header checksum and the computed
        checksum do not match, ask for the packet again.

# To be discussed
- **hop limit (8 bits)**
       This number is decremented when the packet is transmitted. This
       allows for better control of the propagation of the packet. If
       implemented, it makes the locality field obsolete, since we can set
       the hop limit to 2 to get a similar effect.
