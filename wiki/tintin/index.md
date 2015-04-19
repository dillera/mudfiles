TinTin++
========
TinTin++ is one of the oldest, most robust mud clients available. It's cross-platform (Windows, OSX, Linux), programmable (impliments it's own scripting engine/language), provides an API for system interaction/integration, runs on the command line (with a curses interface), and is *really* fast (written in Pure C).

Unfortunately, TinTin's documentation is in a sorry state. It essentially exists in three disparate locations:

- The [In-Client Help System][]  (more current than online manual)
- The [Online Manual][] (typically outdated)
- The [TinTin Forums][]

Out of these three, the [TinTin Forums][] are by far contain the most information - but it's extremely difficult to *find the information you're looking for*. Because of this, I've *(very slowly)* been working at compiling an *unofficial guide*, intended to provide supplementary documentation to what already exists. I first published this content as the [TinTin++ Unofficial Documentation][] last year (using [Sphinx][] & [reStructedText][]). I got caught up in life, didn't spend any time mudding, and when I started working on this again I didn't feel like writing [reStructedText][] or working with [Sphinx][], so I converted the documentation to [Github Flavored Markdown][], used [MDWiki][] to generate the site, and [Github Pages][] to serve the static content (rediculously easy to setup and maintain).

The content is organized into the following sections:

- [Features](features/index.md): Covers the built-in features of TinTin++ in a logical flow.
- [Cookbook](cookbook/index.md): Provides *simple* recipes for performing some of the most common tasks that are frequently asked about on the forums.
- [Debugging](debugging/index.md): Discusses methods for debugging tintin scripts.
- [Pitfalls](pitfalls/index.md): Discusses some of the difficulties that are commonly encountered when writing intermediate & advanced tintin scripts.
- [Terminal Colors](termcolor/index.md): A comprehensive guide to understanding terminal colors.
- [Layouts](layouts/index.md): A discussion of various methods to build 'modular' interfaces (not natively supported by tintin).
- [Python Integration](python/index.md): Provides comprehensive information on using python with tintin to perform advanced computation & automation.
- [Understanding TinTin's Source Code](/ttsrc/index.md): Information on modifying TinTin's source.


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
