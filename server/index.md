NannyMUD Server
===============

Overview
--------
NannyMUD was written several decades ago and hasn't received *real* updates in many years. I was able to obtain a copy of the driver & mudlib, and I'm trying to get them to compile on a *modern* system with a *modern* compiler.


LPC
---

- [NannyMUD LPC Guide](http://mud.stack.nl/manuals/lysator/manual.html) (1998)

Note: Cached copies of these resources are available [here](https://github.com/nfarrar/resources/lpc).


Resources
---------
The included archives were obtained from the following locations:

- [NannyMOS 3.1.1][nannymos-link]           ([download][nannymos-dl])
- [NannyLIB][nannylib-link]                 ([download][nannylib-dl])
- [LDMud 3.3.719][ldmud33719-link]          ([download][ldmud33719-dl])
- [LPMud 3.1.2][lpmud312-link]              ([download][lpmud312-dl])
- [LPC 4.05.14][lpc40514-link]              ([download][lpc40514-dl])
- [Dead Souls II v22.2b14][ds2v222b14-link] ([download][ds2v222b14-dl])
- [Discworld][discoworld-link]              ([download][discoworld-dl])
- [Nightmare 3.3.1][nightmare331-link]      ([download][nightmare331-dl])

Note: Cached copies of these resources are available [here](https://github.com/nfarrar/resources/server).


Compiling
---------

### Ubuntu
Compiling on Ubuntu 14.04 x64::


    $ sudo apt-get -y install build-essential bison gcc libc6
    $ cd src
    $ make


- First error - hardcoded groups?
- Second error - lang.y: warning: 3 shift/reduce conflicts [-Wconflicts-sr] - Makefile, make_func.y, efun_defs.c (headers not found?)
- The LPMud 3.1.2 source includes a ./configure script not present in NannyMOS.
- Attempting to compile LPMud after running ./configure produces errors similar to those when trying to compile NannyMOS.


### OSX
Compiling on OS X 10.10::

    $ brew install gcc bison
    $ cd src
    $ make


References
----------
Various references.

- [Dead Souls Installation FAQ](http://dead-souls.net/ds-inst-faq.html)
- [UbuntuForums Trouble Compiling A Mud](http://ubuntuforums.org/showthread.php?t=524551)
- [C Optimization Flags](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- [Error when using memory allocation](https://stackoverflow.com/questions/11802707/error-when-using-memory-allocation-realloc-with-pointer-of-structure-which-ha)


<!-- Resources -->

[nannymos-link]:        http://www.filewatcher.com/_/?q=nannymos
[nannymos-dl]:          ftp://ftp.kanga.nu/MUD/Servers/LPC/nannymud/nannymos.tar.gz
[nannylib-link]:        http://www.filewatcher.com/_/?q=nannylib
[nannylib-dl]:          ftp://ftp.gr.debian.org/pub/net/mud/lpmud/nannymud/nannylib.tgz
[ldmud33719-link]:      http://www.mudbytes.net/file-2675
[ldmud33719-dl]:        http://www.mudbytes.net/download:228:2675/ldmud-3.3.719.tar.gz
[lpmud312-link]:        http://www.mudbytes.net/index.php?a=files&cid=230
[lpmud312-dl]:          http://www.mudbytes.net/download:230:3/lpmud-3.1.2.tar.gz
[lpc40514-link]:        http://www.mudbytes.net/file-748
[lpc40514-dl]:          http://www.mudbytes.net/download:234:748/lpc4.05.14.tar.gz
[ds2v222b14-link]:      http://www.mudbytes.net/file-843
[ds2v222b14-dl]:        http://www.mudbytes.net/download:71:843/dsII.zip
[discoworld-link]:      http://www.mudbytes.net/file-1039
[discoworld-dl]:        http://www.mudbytes.net/download:71:1039/dw_fluffos_v2.zip
[nightmare331-link]:    http://www.mudbytes.net/file-1044
[nightmare331-dl]:      http://www.mudbytes.net/download:71:1044/nightmare3_fluffos_v2.zip
