Terminal Layouts
================


Overview
--------
TinTin++ is a 'single window' console application and and a very simple API for *splitting* windows (through the `#split` API). These splits are horizontal only (you cannot split a window vertically in TinTin) and are *very* basic. It's highly desirable to build more advanced interfaces (the primary reason lots of users end up with GUI applications).

**See also:**

- [Building a tmux API](/tintin/advanced/terminal-layouts/tmux-api.md)
- [Building a tmuxp API](/tintin/advanced/terminal-layouts/tmuxp-api.md)
- [Mapper Display](/tintin/features/mapper/display.md)
- [Chat Protocol](/tintin/advanced/chat.md)
- [Netcat Pipes](/tintin/advanced/netcat.md)

Note: This section is currently incomplete.


Use Cases
---------
The most common use case scenario is to display the map in a separate window (rather than a `#SPLIT`). This is *especially* useful when using a wide terminal, since `#SPLIT` will only create a horizontal divider and occupy an extremely large amount of unnecessary space. Another common use case is to pipe specific types of messages into separate windows by the content type - i.e. having a 'primary' window running tintin, piping all combat data into a separate window, and all communications into a separate window.

This has lots of additional applications:

- Debugging (piping messages from scripts into a separate debugging pane)
- Building menu systems
- Multiplaying Displays (playing multiple characters simultaneously)
    - Displaying a list of sessions 
    - Viewing data from multiple sessions simultaneously


Terminal Multiplexers
---------------------
[Terminal Multiplexers][Terminal Multiplexer] are different than [Terminal Emulators][Terminal Emulator]. A [Terminal Emulator][] is the the software (typically a gui, but not necessarily) that manages the console session (not to be confused with the [shell][] either). Some common terminal emulators are [gnome-terminal][] (linux), [iTerm2][] (OSX), and [MinTTY][] (Windows). 

A [Terminal Multiplexer][] typically runs in the [Terminal Emulator][] and allows for multiple sessions to execute simltaneously in a single interface (terminal emulator 'window'). Some graphical terminal emulators ([iTerm2][] & [terminator][]) have the built-in ability to run multiple sessions in a single graphical interface. This is *similar* to a terminal multiplexer, but not quite the same (or as powerful).

The two most commonly used terminal multiplexers are [GNU Screen][screen] and [tmux][]. [Screen][] is by far the oldest and [tmux][] is by far the most powerful. For our purposes of building layouts with tintin++, we'll be discussing [tmux][], as it's the *better* solution.


Introduction to Tmux
--------------------
[Tmux][] is a modern, scriptable, [terminal emulator][] - the keyword being *scriptable*. This is an important disctinction between it and [screen][], because screen is not - and the API provided by tmux is necessary for our purposes.


Tmux Session Managers
---------------------
Tmux session managers are *awesome*. They wrap tmux and provide an interface for saving (or freezing) sessions, reloading sessions, advanced configuration features, and in some cases ([tmuxp][]) bindings for programatically interacting with tmux sessions using languages, rather than a shell-based command line interface.

There are several session managers available for tmux. Some of the most common are:

- [tmuxinator][]
- [teamocil][]
- [tmuxp][]

Each of these tools leverage tmux's internal, *protected* layout interface to save & restore layouts, with additional commands to configure the dimensions and settings of windows & panes in a tmux session. To manually export the session's layout string from tmux, you can use the command:

    `tmux display-message -p "#{window_layout}"`

This will spit out a long string of data, that describes, in terms that tmux understands, how to build your interface. 

Out of these options, I use [tmuxp][], because it provides an API for interacting with tmux sessions via python.

Note: The `window-layout` string exports *exact* sizes, tailoring the layout to a specific window size/resolution. If you're running your tintin environment on displays on varying sizes, you'll probably want to build layouts for each platform. In this case, you'll most likely want to setup an additional wrapper for starting the environment where the layout can be specified (see [startin][] for an example of this).




Building a TinTin/Tmuxp API
---------------------------
Note: TODO.


<!-- References -->

[Terminal Multiplexer]:         http://en.wikipedia.org/wiki/Terminal_multiplexer
[Terminal Emulator]:            http://en.wikipedia.org/wiki/Terminal_emulator
[Shell]:                        http://en.wikipedia.org/wiki/Shell_(computing)
[gnome-terminal]:               https://help.gnome.org/users/gnome-terminal/stable/
[iTerm2]:                       http://iterm2.com/
[MinTTY]:                       https://code.google.com/p/mintty/
[Terminator]:                   http://gnometerminator.blogspot.com/p/introduction.html
[List of Terminal Emulators]:   http://en.wikipedia.org/wiki/List_of_terminal_emulators
[screen]:                       https://www.gnu.org/software/screen/
[tmux]:                         http://tmux.sourceforge.net/
[tmuxp]:                        https://github.com/tony/tmuxp
[teamocil]:                     http://www.teamocil.com/
[tmuxinator]:                   https://github.com/tmuxinator/tmuxinator
