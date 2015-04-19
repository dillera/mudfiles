Curses
======

Examples
--------

|                                              Source                                             |                          Description                           |
| ----------------------------------------------------------------------------------------------- | -------------------------------------------------------------- |
| [github: bpython/bpython](https://github.com/bpython/bpython)                                   | A fancy curses interface to the Python interactive interpreter |
| [github: cykerway/ncmpy](https://github.com/cykerway/ncmpy)                                     | A [Python + Curses]-based MPD client                           |
| [github: firecat53/tabview](https://github.com/firecat53/tabview)                               | Python curses command line CSV viewer                          |
| [github: doug-letough/vindinium-curses_ui](https://github.com/doug-letough/vindinium-curses_ui) | Python/Curses user interface for the Vindinium A.I contest     |


Urwid
-----

- [Uriwid Home](http://urwid.org/)
- [Urwid Source](https://github.com/wardi/urwid)
- [Urwid Tutorial - Minimal Application](http://urwid.org/tutorial/)
- [Nic0's Sphere: Urwid for Python, a ncurses library](http://www.nicosphere.net/urwid-for-python-a-ncurses-library-2541/)
- [users-cs.au.dk/ - Urwid Tutorial](http://users-cs.au.dk/jakobt/urwid-0.9.8.3r2/tmpl_tutorial.html)
- [LWN.net - Urwid, a Console UI library for Python](http://lwn.net/Articles/172998/)
- [Build a Website with NCurses](http://www.pythondiary.com/tutorials/build-website-ncurses.html)

### Example Applications

- [Urwid Application List](https://github.com/wardi/urwid/wiki/Application-list)
- [Uriwid Example Applications](http://urwid.org/examples/index.html)
- [Console Applications with Urwid](https://www.youtube.com/watch?v=4UwhXC1OP2A)

Applications:

- [bpython-urwid](https://github.com/janewang/BPython/blob/master/bpython/bpython/urwid.py)
- [ngiri](https://github.com/sushi-irc/nigiri)
- [videotop](https://github.com/intnull/videotop) - A console browser for online videos for websites like YouTube.
- [alot](https://github.com/pazz/alot) - Commandline MUA using notmuch and urwid.
- [turses](https://github.com/dialelo/turses) - A Twitter client for the console (UNMAINTAINED).
- [speedometer](https://github.com/wardi/speedometer) - Chart network TX/RX and file download rates on the console.
- [xo](https://github.com/scopatz/xo) - Exofrills text editor.|
- [hachoir-urwid](http://goo.gl/OgostQ) - Binary file explorer based on Hachoir library to parse the files.
- [urwidcmd](https://code.google.com/p/urwidcmd/) - This project is a try of cloning Midnight(Norton) Commander or Dos Navigator programs.
- [pudb](http://git.tiker.net/pudb.git/tree) - PuDB is a full-screen, console-based visual debugger for Python.
- [flix](https://github.com/NYCPython/Flix/blob/master/Flix.py)


### Widgets & Snippets
Various widgets & snippets.

- Menus
    - [pymenu-urwid](https://gist.github.com/XayOn/6656708)
- Unsorted
    - [github: pazz/urwidtrees](https://github.com/pazz/urwidtrees)
    - [github: rbistolfi/urwid-combobox](https://github.com/rbistolfi/urwid-combobox)
    - [github: suminb/urwid-stackedwidget](https://github.com/suminb/urwid-stackedwidget)
    - [github: Jdaco/urwidgets](https://github.com/Jdaco/urwidgets)
    - [github: jellonek/urwidext](https://github.com/jellonek/urwidext)
    - [github: devsandk/urwidakh](https://github.com/devsandk/urwidakh)
    - [github: jrd/urwidm](https://github.com/jrd/urwidm)
    - [github: extempore/urwidext](https://github.com/extempore/urwidext)
    - [github: tonycpsu/urwid-datatable](https://github.com/tonycpsu/urwid-datatable)
    - [github: robla/urwid-demo](https://github.com/robla/urwid-demo)


## Broken Terminal
When I break out of an urwid application with ctrl-c my terminal becomes hosed.

- This does not happen if the application exits gracefully (for example with a `quit_on_q` handler.)
- Running `tput-reset` does not fix it.
