Python
======


Overview
--------
While the TinTin++ scripting language is great (for what it's designed to do), it's not nearly as robust as python.  I spent a significant amount of time building *very* complex TinTin++ scripts before I finally decided it was time to begin migrating as much of my TinTin++ framework over to python.

Getting started with external script integrations is a little tricky, so I've built this guide to walk through the process.

Note: The basic integration commands are covered in the [features/shell-integrations](/tintin/features/shell-integration.md) section. If you aren't familiar with the `#system`, `#script` and `#run` commands, you should review that first.

There are many ways that we can interact with TinTin++ for our external scripts. Each method has its own strengths and weaknesses.

**See also:**

- [Executing Scripts with #SYSTEM](/tintin/advanced/python/execute-system.md)
- [Retrieving Values With #SCRIPT](/tintin/advanced/python/retrieve-script.md)
- [Attaching an Interpreter with #RUN](/tintin/advanced/python/attach-interpreter.md)
- [Shell Integrations](/tintin/features/shell-integration.md)


References
----------

Discussions from TinTin++ forums regarding external scripting:

- [Is it possible to run an external script interactively?](http://tintin.sourceforge.net/board/viewtopic.php?t=1226)
- [Generic MySQL Connector](http://tintin.sourceforge.net/board/viewtopic.php?t=1112)
- [How to do external item database](http://tintin.sourceforge.net/board/viewtopic.php?t=735)
- [Example Python Integration](http://tintin.sourceforge.net/board/viewtopic.php?t=2156)
- [Python Scripting Help](http://tintin.sourceforge.net/board/viewtopic.php?t=1195)
- [Using TinTin++ with an External Script](http://tintin.sourceforge.net/board/viewtopic.php?t=906)
- [LUA Shell](http://tintin.sourceforge.net/scripts/lua.php)

TinTin repositories using python:

- [nstockton/tintin-mume](https://github.com/nstockton/tintin-mume)
- [rascul/tintin-scripts](https://github.com/rascul/tintin-scripts)
- [michael-n-kaplan/ta-scripts](https://github.com/michael-n-kaplan/ta-scripts)
- [sunayforever/pkuxkx](https://github.com/sunwayforever/pkuxkx)
- [twksos/pkuxkx_tintin](https://github.com/twksos/pkuxkx_tintin)

Python telnet proxies:

- [PyBot](http://bazaar.launchpad.net/~pybot-team/pybot/src/files)
- [Python Telnet Proxy](http://www.achaea.com/forum/python-telnet-proxy)
- [Telnet Proxy with Twisted](http://stackoverflow.com/questions/7354864/http-through-telnet-with-python-and-twisted)
- [SimpleProxy](http://www.mccarroll.net/snippets/simpleproxy/index.html)
- [NightCrawler Proxy](https://gist.github.com/night-crawler/6213578)

Mud-Related python projects:

- [tf-mapper](https://github.com/michael-donat/tf-mapper)
- [MudMapper](https://github.com/michael-donat/MudMapper)
- [Mudblood](https://github.com/talanis85/mudblood-py)
- [BastProxy](https://github.com/endavis/bastproxy)
- [Donginer](https://github.com/Senso/Donginger)
- [WHAM Agent](https://github.com/shuchton/wham_agent)
