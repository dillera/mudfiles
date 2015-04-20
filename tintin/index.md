TinTin++
========


Overview
--------
TinTin++ is one of the oldest, most robust mud clients available. It's cross-platform (Windows, OSX, Linux), programmable (impliments it's own scripting engine/language), provides an API for system interaction/integration, runs on the command line (with a curses interface), and is *really* fast (written in Pure C).

Unfortunately, TinTin's documentation is in a sorry state. It essentially exists in three disparate locations:

- The [In-Client Help System][]  (more current than online manual)
- The [Online Manual][] (typically outdated)
- The [TinTin Forums][]

Out of these three, the [TinTin Forums][] are by far contain the most information - but it's extremely difficult to *find the information you're looking for*. Because of this, I've *(very slowly)* been working at compiling an *unofficial guide*, intended to provide supplementary documentation to what already exists. I first published this content as the [TinTin++ Unofficial Documentation][] last year (using [Sphinx][] & [reStructedText][]). I got caught up in life, didn't spend any time mudding, and when I started working on this again I didn't feel like writing [reStructedText][] or working with [Sphinx][], so I converted the documentation to [Github Flavored Markdown][], used [MDWiki][] to generate the site, and [Github Pages][] to serve the static content (rediculously easy to setup and maintain).

The content is organized into the following sections:


Contents
--------

- [Features](/tintin/features/index.md)
    - [Sessions](/tintin/features/sessions.md)
    - [Datatypes](/tintin/features/datatypes.md)
    - [Command Files](/tintin/features/command-files.md)
    - [Loops](/tintin/features/loops.md)
    - [Colors](/tintin/features/colors.md)
    - [Tab Completion](/tintin/features/tab-completion.md)
    - [Mapper](/tintin/features/mapper.md)
        - [Helpfile](/tintin/features/mapper/helpfile.md)
        - [Automapper](/tintin/features/mapper/automapper.md)
        - [Metadata](/tintin/features/mapper/metadata.md)
        - [Editing](/tintin/features/mapper/editing.md)
        - [Display](/tintin/features/mapper/display.md)
- [Cookbook](/tintin/cookbook/index.md)
    - [Class Guards](/tintin/cookbook/class-guards.md)
    - [Clear Console](/tintin/cookbook/clear-console.md)
    - [Executing Code in All Sessions](/tintin/cookbook/executing-code-in-all-sessions.md)
    - [Parsing Tables by Reference](/tintin/cookbook/parsing-tables-by-reference.md)
    - [Passing by Reference](/tintin/cookbook/passing-by-reference.md)
    - [Read Verbose with Flag](/tintin/cookbook/read-verbose-with-flag.md)
    - [Reload Command File](/tintin/cookbook/reload-command-file.md)
    - [Getting a String's Length](/tintin/cookbook/string-get-length.md)
    - [Repeating Characters in a String](/tintin/cookbook/string-repeat-character.md)
    - [Splitting a String by Index](/tintin/cookbook/string-split-by-num-chars.md)
    - [Variable Table Indexes](/tintin/cookbook/variable-table-indexes.md)
    - [Variable Text Existence](/tintin/cookbook/variable-text-existence.md)
- [Debugging](/tintin/debugging/index.md)
- [Pitfalls](/tintin/pitfalls/index.md)
- [Advanced](/tintin/advanced/index.md)
    - [Terminal Colors](/tintin/advanced/terminal-colors.md)
    - [Terminal Layouts](/tintin/advanced/terminal-layouts.md)
        - [Building a tmux API](/tintin/advanced/terminal-layouts/tmux-api.md)
        - [Building a tmuxp API](/tintin/advanced/terminal-layouts/tmuxp-api.md)
    - [Chat Protocol](/tintin/advanced/chat.md)
    - [Netcat Pipes](/tintin/advanced/netcat.md)
    - [Python Integration](/tintin/advanced/python.md)
        - [Executing Scripts with #SYSTEM](/tintin/advanced/python/execute-system.md)
        - [Retrieving Values With #SCRIPT](/tintin/advanced/python/retrieve-script.md)
        - [Attaching an Interpreter with #RUN](/tintin/advanced/python/attach-interpreter.md)
    - [Understanding TinTin's Source Code](/tintin/advanced/tintin-source-code.md)


Additional Resources
--------------------

- [TinTin++](http://tintin.sourceforge.net/)
- [TinTinPlus User's Group](http://tintinplusplus.github.io/)
- [TinTin Source (Github)](https://github.com/tintinplusplus/tintin)



<!-- Citations -->

[In-Client Help System]:                http://git.io/vvjkl
[Online Manual]:                        http://tintin.sourceforge.net/manual/
[TinTin Forums]:                        http://tintin.sourceforge.net/board/
[TinTin++ Unofficial Documentation]:    http://tintinplusplus-unoffical-documentation.readthedocs.org/en/latest/
[reStructedText]:                       http://docutils.sourceforge.net/rst.html
[Sphinx]:                               http://sphinx-doc.org/
[Github Flavored Markdown]:             https://help.github.com/articles/github-flavored-markdown/
[MDWiki]:                               https://github.com/Dynalon/mdwiki/
[Github Pages]:                         https://pages.github.com/
