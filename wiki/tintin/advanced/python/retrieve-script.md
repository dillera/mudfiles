Retrieving Values With #SCRIPT
==============================

Overview
--------

**See also:**

- [Python Overview](/tintin/advanced/python.md)
- [Executing Scripts with #SYSTEM](/tintin/advanced/python/execute-system.md)
- [Attaching an Interpreter with #RUN](/tintin/advanced/python/attach-interpreter.md)
- [Shell Integrations](/tintin/features/shell-integration.md)


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

