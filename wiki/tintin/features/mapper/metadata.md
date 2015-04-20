Mapper Metadata
===============


Overview
--------


**See also:**

- [Mapper](/tintin/features/mapper.md)
- [Helpfile](/tintin/features/mapper/helpfile.md)
- [Automapper](/tintin/features/mapper/automapper.md)
- [Editing](/tintin/features/mapper/editing.md)
- [Display](/tintin/features/mapper/display.md)

The mapper stores it's data in a simple, flat, plaintext database.

The mapper also has the capabilities for storing metadata about each room that can be used in an infinite amount of ways. The command #map info shows you some of these fields:

    Room area:
    Room data:
    Room desc:
    Room name:
    Room note:
    Room symbol:
    Room terrain:

But map set shows them all (I believe):

    roomarea:
    roomcolor:
    roomdata:
    roomdesc:
    roomflags: 0
    roomname:
    roomnote:
    roomsymbol:
    roomterrain:
    roomweight: 1.000

Any of these values can be set with the #map set command, for example:

    #map set roomarea mainland

These options are tailored to the MSDP protocol, which specifies values like TERRAIN that can be easily parsed and stored. I play on an LPMUD, which does not have terrain, so I don't use this value, but I do use the others.

