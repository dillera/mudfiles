Python
======


Overview
--------
While the TinTin++ scripting language is great (for what it's designed to do), it's not nearly as robust as python.  I spent a significant amount of time building *very* complex TinTin++ scripts before I finally decided it was time to begin migrating as much of my TinTin++ framework over to python.

Getting started with external script integrations is a little tricky, so I've built this guide to walk through the process.

Note: The basic integration commands are covered in the [features/shell-integrations](/tintin/features/shell-integration.md) section. If you aren't familiar with the `#system`, `#script` and `#run` commands, you should review that first.

There are many ways that we can interact with TinTin++ for our external scripts. Each method has its own strengths and weaknesses.


Executing Scripts with #SYSTEM
-----------------------------
If we want tintin++ to run an external script that *does not* need to return data to TinTin, we can run it using the `#system` API. This is very limited, but may be desirable in some circumstances, for example - if we want an external script ensure that specific files & directories are created - and display some output directly to us:

    #nop create the necessary files and directories on a fresh setup;
    #system scripts/setup-env.py

Unlike the `#SCRIPT` and `#RUN` APIs, output from the script is displayed in the terminal, not redirected into variable(s).

It's important to note that the script does not "persist in memory" when run with `#SYSTEM` - it immediately executes, performs whatever procesing it does, and then terminates. Any computed data is no longer available once the script terminates.

A more practical example would be building an interface for inserting an item into a database. We *probably* don't need a return value, and we'd leverage some back-end mechanism for persistently storing the data after the script terminates:

    #nop An example method of interacting with a an external itemdb script.
    #system {scripts/itemdb.py --action='add' --name='A sword' --value=1000 --weight=10 --npc_id=10}

It would be tedious to type the above command *each* time we want to insert an item into our database, so we'd probably couple this with a TinTin `#ALIAS`:

    #alias {additem} {
      #system {scripts/itemdb.py --action='add' --name='%1' --value=%2 --weight=%3 --npc_name=%4}
    }

And now from our interface, we could call the alias manually:

    additem {A sword} {1000} {10} {Some NPC}

Note: For usability purposes, the second example allows us to specify the NPC name rather than ID (we'll never be able to remember the ID's from our database of NPCs), so the script would also need to perform a lookup to get the correct ID for us.

On the backend, our python script still needs to parse the arguments, then insert the new data into a database, but this is trivial in python and insanely less difficult than a *pure tintin* approach. This approach allows us to do lots of things that are otherwise insanely complex/impossible:

- Build relationships between NPCs and items
- Build relationships between NPCs and map locations
- Store *large* amounts of data
- Setup *simple* actions that call our python API to automatically add items or NPCs to the database


Retrieving Values With #SCRIPT
-----------------------------
If we want tintin++ to execute some code and compute some values to be saved in TinTin++ variables, then `#SCRIPT` *might* be the best solution. Again, we can pass arguments into our script via the call to the script, and this time a value will be returned and stored in the specified variable. A good use case for this might be to retrieve information about an NPC from an npc database:

    #nop get information about the NPC named 'Some NPC';
    #script {npclist} {scripts/npcdb.py --action='find' --condition='WHERE name='Some NPC'}

Or we could get a list of NPCs that match a specific criteria, such as a difficulty rating:

    #nop get a list of npcs with a difficulty greater than 5
    #script {npclist} {scripts/npcdb.py --action='find' --condition='WHERE difficulty>=5'}

And like the previous example, we probably want to setup some aliases for common queries:

    #alias {getNPCSByName} {
        #script {npclist} {scripts/npcdb.py --action='find' --condition='WHERE name=%1'}
    }

    #alias {getNPCSByDifficulty} {
        #script {npclist} {scripts/npcdb.py --action='find' --condition='WHERE difficulty>=%1'}
    }

While this is already *insanely* useful (the ability to programatically save and lookup information about NPCs from a database) - this can be *far* more useful. The current implimentation is going to return a list of npcs and store the result in a variable in tintin. To programatically use this data, we can pass the variable list to another script:

    #nop navigates to the mapper location for each NPC in the npclist and stops when the first NPC is found.
    #alias {gotoFirstNPC} {
        #system {scripts/movetonpc.py %1}
    }

There are several important things to note about the alias above:

1. We used `#SYSTEM` rather than `#SCRIPT` because we aren't looking for a return value.
2. The script assumes we have some way to retrieve mapper locations associated with NPCs in our database.
3. We're assuming we can inject commands into a session, which we don't yet have a method to do.

It's also important to note that like the previous example - the script executes, than terminates. It doesn't *continue* to run - it would simply (if we actually built it this way) just dump the movement commands to the terminal, then terminate.  Therefore it can't really monitor the session and perform error checking.

With this type of setup data is being passed statelessly through TinTin. This has some *serious* drawbacks that become immediately present:

Each time we want to execute a python command/query we need to spin up a new instance of the script. If we're just using it for a glorified note-taking system, that doesn't seem to bad - I mean, how fast can we really type? But if we consider trying to scale up so that we can perform advanced logic for multiple characters, simltaneously, in a completely automated fashion - this can become a problem.

Each query or call to a script forces us to convert data between TinTin and our scripting engine (python). This has a lot of 'programming overhead' (and subsequently room for error):

1. We take a `TinTin++` variable list and run it through an argument conversion processor (probably a TinTin alias)
2. This generates (hopefully) a list command line arguments to pass to the python script.
3. The python script parses the argument list and (hopefully) generates the correct internal data representations.
4. The python script performs some computations/lookups/whatever.
5. The python script passes the internal data structure back to a TinTin data type conversion routine (probably specially formatted print statements)
6. The end result is *hopefully* a variable list in TinTin containing the data we wanted.

The steps above are for a single API call (using this approach) - and there's no way to pass data directly between python scripts (which would be *far* more effecient and eliminate all the conversion steps that introduce lots of room for error).

There's gotta be a *better* way ...


Attaching to Interpreters with #RUN
-----------------------------------
The `#RUN` command is a different beast than `#SYSTEM` and `#SCRIPT`.





References
----------

TinTin Discussions regarding external scripting:
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

Mud-Related Python Projects:
- [tf-mapper](https://github.com/michael-donat/tf-mapper)
- [MudMapper](https://github.com/michael-donat/MudMapper)
- [Mudblood](https://github.com/talanis85/mudblood-py)
- [BastProxy](https://github.com/endavis/bastproxy)
- [Donginer](https://github.com/Senso/Donginger)
- [WHAM Agent](https://github.com/shuchton/wham_agent)
