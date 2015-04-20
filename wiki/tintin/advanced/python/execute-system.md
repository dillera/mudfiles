Executing Scripts with #SYSTEM
==============================


Overview
--------
The `#SYSTEM` command is extremely straight-foward. It 


**See also:**

- [Python Overview](/tintin/advanced/python.md)
- [Retrieving Values With #SCRIPT](/tintin/advanced/python/retrieve-script.md)
- [Attaching an Interpreter with #RUN](/tintin/advanced/python/attach-interpreter.md)
- [Shell Integrations](/tintin/features/shell-integration.md)


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


