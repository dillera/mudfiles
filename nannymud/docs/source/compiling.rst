=========
Compiling
=========


-------
Archive
-------
An archived copy of the NannyMUD lpmud driver & starting mudlib. I am currently trying to get it to compile on Ubuntu 14.04 & OSX 10.9. I have also included some other mud drivers for reference.


------
Ubuntu
------
Compiling on Ubuntu 14.04 x64::


    $ sudo apt-get -y install build-essential bison gcc libc6
    $ cd src
    $ make


- First error - hardcoded groups?
- Second error - lang.y: warning: 3 shift/reduce conflicts [-Wconflicts-sr] - Makefile, make_func.y, efun_defs.c (headers not found?)
- The LPMud 3.1.2 source includes a ./configure script not present in NannyMOS.
- Attempting to compile LPMud after running ./configure produces errors similar to those when trying to compile NannyMOS.


----
OS X
----
Compiling on OS X 10.10::

    $ brew install gcc bison
    $ cd src
    $ make


----------
References
----------
Various references.

- .. `Dead Souls Installation FAQ           <http://dead-souls.net/ds-inst-faq.html>`_
- .. `UbuntuForums Trouble Compiling A Mud  <http://ubuntuforums.org/showthread.php?t=524551>`_
- .. `C Optimization Flags                  <https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html>`_
- .. `Error when using memory allocation    <https://stackoverflow.com/questions/11802707/error-when-using-memory-allocation-realloc-with-pointer-of-structure-which-ha>`_

