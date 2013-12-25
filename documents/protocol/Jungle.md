Jungle Protocol
===============

Or what is the language that the WaDed speak together.

This is called jungle protocol because of the anarchy of this protocol. In
order to have no master in this conversation, the way the WaDeD communicate
seems a little desordanate.

It is importante to make the difference between this protocol and the network
protocol, like DASH7. This protocol describes how a string transmitted through
a network protocol is parsed by the WaDeD.

How it works
------------

So, the idea is to compare the messages stored by each of the two WaDeD
comunicating. Doing this means compare the top hashes of the merkel trees, and
then if they are different, compare the sons, and so on until we find wich
messages to send.

But we want to do this conversation of exchange of hashes to be without any
master, and to be carried is the presence of several WaDeD speaking together.
So the idea is: a WaDeD frequently send its top hashes, and whenever a WaDeD
sees a hash that differs from its own, it sends the sons hashes. In broadcast.
This way, we always go down in the tree, we have no master, and we can speak
all together.

The protocol
------------

Here is how a message is parsed:
- *Protocol Version*: 4 bits. We will start with 0, and if the protocol
evolves, we can have so retrocompatibility.
- *Message type*: 4 bits. The represent what type of message is sent; if it is
exchange of hash, of a massage...
- *Message*: the message itself, its content and size are explained below.

The NODE type
-------------

This type of message is used to describe a portion of a merkel tree. It gives a
node in a tree, its hash, and all the hashes of its sons.

- *Node number*: 8 bits. This represents the position of the node on the merkel
trees. The first bit (1 << 7) is set to 0 if the node is on the left tree, and
to 1 if it is on the right one. The last seven bits are used to give the
position in this tree (with a BFS).
- *Hash*: 64 bits. The content of this node: a hash in a merkel tree.
- *Sons list*: 512 bits. The 8 hashes of its 8 sons.

If a WaDeD receives this:
- If its own hash is the same, it will do nothing.
- If some, it will send a NODE type, or a LIST type, corresponding to a son
that differs.

The ROOT type
-------------

This type of message is used to communicate the top hashes.
Since it is the default message, we may consider add some information in it if
we need to.

- *Left hash*: 64 bits. The hash of the left tree.
- *Right hash*: 64 bits. The hash of the right tree.

If a WaDeD receives this:
- If it has the same hashes, it will do nothing.
- If not, it will send a NODE type message.

The LIST type
-------------

This type of message is used to communicate a list of messages that are stored
in the same leaf of a merkel tree.

- *Leaf number*: 10 bits. The leaf number, from 0 to 1023.
- *List size*: 6 bits. The number of messages in the list. We assume there will
be not many messages, since it is vely improbable, and accept that the
programm will not work if not.
- *Hash*: 64 bits. The list hash.
- *Messages hashes*: list_size * 64 bits. The hashes of all the messages in the
list.

If a WaDeD receives this:
- If it has the same list hash, it will do nothing.
- If not, if one of its own messages should be on this list, and is not, it
will send it with a MESSAGE type message.
- If not, it will send its own list in a LIST type message, so that the other
WaDeD can send the missing messages, because of the above rule.

The MESSAGE type
----------------

This type of message is used to communicate a message entirely.

- *Hash*: 64 bits.
- *Emission date*: 32 bits.
- *Expiration date*: 32 bits.
- *Source address*: 16 bits.
- *Destination address*: 16 bits.
- *Message*: bits left.

If a WaDeD receives this:
- If it has the same message (check comparing hashes), it will do nothing.
- If not, it will check the integrity of the message by recalculing the hash,
and if it is correct, it will insert it.
