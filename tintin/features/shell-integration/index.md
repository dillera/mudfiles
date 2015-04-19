Shell Integration
=================


Overview
--------
While the TinTin++ scripting language is great (for what it's designed to do), it's still just a *"toy"* language and not nearly as robust as languages like python or ruby.  I spent a significant amount of time building *very* complex TinTin++ scripts before I finally decided that the TinTin language wasn't going to be able to handle everything I wanted to do.

Getting started with external script integrations is a little tricky, so I've built this guide to walk through the process.

TinTin provides three distinct commands for interacting with external resources & scripts:

- [Run](http://tintin.sourceforge.net/manual/run.php)
- [Script](http://tintin.sourceforge.net/manual/script.php)
- [System](http://tintin.sourceforge.net/manual/system.php)

The 'System' Command
--------------------
Of the three commands, `#system` is the most straight forward. This *does not* return a value to TinTin - it simply executes a command in the parent shell and the output is dumped straight to console. Some command examples that I use:

    #alias {clear} {#system clear}
    #alias {ls}    {#system ls}
    #alias {ll}    {#system ls -lah}
    #alias {pwd}   {#system pwd}


The 'Script' Command
--------------------
Next is the `#script` command. This is a little more complicated (than `#system`)- and acts differently based on the way in which it's called. The standard syntax is:

    #script {variable} {shell commands}

This just calls a script and stores any output generated by the script in the specified tintin variable, here's a couple examples:

    #script {result} {python -c 'print "Hello TinTin++"'}
    #VARIABLE {result}={{1}{Hello TinTin++}}

    #nop given the test.py script below ...
    #script {result} {test.py}
    #VARIABLE {result}={{1}{hello 01 from test.py}{2}{hello 02 from test.py}}

    #!/usr/bin/env python
    # test.py
    print('hello 01 from test.py')
    print('hello 02 from test.py')

Note: The variable is treated as a list, and each line output is stored in an index of that variable.


The 'Run' Command
-----------------
The `#run` command is more extensive than both `#script` and `#run` - it creates a new session and attaches the script/command to that session. This is done specifically with the intent of running interactive commands (i.e.  attaching an interpreter to a session). When the command ends the session also ends. The standard syntax for calling a script using `#run` is:

    #run {session name} {shell commands}

Given the script:

    #!/usr/bin/env python
    print('hello from test.py')

We can run it with the command `#run test.py`, which yields the output:

    hello from test.py
    #SESSION 'test'
    #SESSION 'gts' ACTIVATED

As expected, a new session named 'test' is started, the script dumps it's output, and the session dies. Not very useful - but we can make it do more interesting things. For example - we can startup a python interpreter and attach it to a session with:

    #run python python

We can now write python in our python session: `print('echo with python')` - and tintin script as well: `#showme hello!`.

Note: In a regular tintin session, and preceding whitespace before the `#` character is stripped out. In a `#run python python` session this does not happen and results in an intepretter error.

We can leave the python session running in the background and send commands to it (just like normal) which contain python code that will be executed:

    #gts
    #python print("hello from #gts session")

Note: While we don't see the output in #gts, we can switch back to the python session (`#python`) and scroll through the history buffer to see that the command was actually executed.



References
----------

- [Generic MySQL Connector](http://tintin.sourceforge.net/board/viewtopic.php?t=1112)
- [Example Python Integration](http://tintin.sourceforge.net/board/viewtopic.php?t=2156)
- [Python Scripting Help](http://tintin.sourceforge.net/board/viewtopic.php?t=1195)
- [Using TinTin++ with an External Script](http://tintin.sourceforge.net/board/viewtopic.php?t=906)