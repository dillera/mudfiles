Mapper Help
===========

Overview
--------
A mirror of the 'in-client' mapper documentation (not available online, and viewing all this documentation in the client is tedious). Extracted from [help.c](http://git.io/vvhjx) on 2015/14/19.


See also:

- [Mapper](index.md)
- [Helpfile](helpfile.md)
- [Automapper](automapper.md)
- [Metadata](metadata.md)
- [Editing](editing.md)
- [Display](display.md)


"In-Client" Mapper Help
-----------------------


The map command is the backbone of the auto mapping feature. In order to use mapping for muds using 'ne' 'se' 'sw' 'nw' directions, you need to create a #pathdir for each direction.

    #map at <location> <command>
Execute the command at the location.

    #map color <field> value
Sets the map color for the given field.

    #map create <size>
Creates a new map and room 1, required to start mapping. Default size is 15000 rooms.

    #map destroy
Deletes the map.

    #map delete <direction>
Deletes the room in the given direction.

    #map dig <direction|vnum> [new|<vnum>]
Creates an exit in the given direction. If no valid direction is given or no existing room is found a new room is created. Useful for portal links and other alternative forms of transportation. If the 'new' argument is provided all existing rooms are ignored. If a room vnum is given as the second argument an exit will be created to the given room vnum. If the direction argument is a valid unused vnum that room is created.

    #map exit <exit> <COMMAND|DIRECTION|FLAG|GET|NAME|SAVE|SET|VNUM> <arg>
Set the exit data. Useful with a closed door where you can set the exit command: '#map exit e command {open east;e}'

    #map exitflag hide
When set, '#map' will not display the map beyond this exit. When mapping overlapping areas or areas that aren't build consistently you need this flag as well to stop auto-linking, unless you use void rooms.

    #map explore <direction>
Follows the direction until a dead end or an intersection is found. The route is stored in #path and can subsequently be used with #walk

    #map find <name> <exits> <desc> <area> <note> <terrain> <flag>
Searches for the given room name. If found the shortest path from your current location to the destination is calculated. The route is stored in #path and can subsequently be used with the various #path commands. If <exits> is provided all exits must be matched, if <roomdesc>, <roomarea> or <roomnote> or <roomterrain> or <roomflag> is provided these are matched as well against the room to be found. These options are also available to the goto, run, delete, at and link commands.

    #map flag asciigraphics
Draws a smaller but more detailed map that displays the ne se sw nw exits and room symbols.

    #map flag asciivnums
Display room vnums with asciigraphics enabled.

    #map flag nofollow
When you enter movement commands the map will no longer automatically follow along. Useful for MSDP and GMCP automapping scripts.

    #map flag static
Will make the map static so new rooms are no longer created when walking into an unmapped direction. Useful when you're done mapping and regularly bump into walls accidentally creating a new room. #map dig etc will still work.

    #map flag vtgraphics
Enables vt line drawing on some terminals

    #map flag vtmap
Will enable the vtmap which is shown in the top split screen if you have one. You can create a 16 rows high top screen by using '#split 16 1'.

    #map goto <room vnum> [dig]
Takes you to the given room vnum, with the dig argument a new room will be created if non exists.

    #map goto <name> <exits> <desc> <area> <note> <terrain>
Takes you to the given room name, if you provide exits those must match.

    #map get <option> <variable> [vnum]
Store a map value into a variable, if no vnum is given the current room is used.

    #map get roomexits <variable>
Store all room exits into variable.

    #map info
Gives information about the map and room you are in.

    #map insert <direction> [roomflag]
Insert a room in the given direction. Most useful for inserting void rooms.

    #map jump <x> <y> <z>
Jump to the given coordinate, which is relative to your current room.

    #map leave
Makes you leave the map. Useful when entering a maze.

    #map legenda <symbols>
The legend exists of 17 decimal numbers which represent character symbols used for drawing a map. Binary order is used with the n e s w representing bit 1 to 4. The first number stands for a no exit room. Number 2 to 16 stand for n e ne s ns es nes w nw ew new sw nsw esw nesw. The 17th number stands for the room the player is current in. The legenda is set by default, but can be adjusted to take advantage of fonts with line drawing characters. To check your font for drawing characters use: #loop 32 255 cnt {#echo {%-3s  %a} $cnt $cnt}

    #map link <direction> <room name> [both]
Links two rooms. If the both argument and a valid direction is given the link is two way.

    #map list <name> <exits> <desc> <area> <note> <terrain>
Lists all matching rooms and their distance. Use {variable} {<variable>} to save the output to a variable. {roomname} {<name>}, {roomarea} {<area>}, etc, are valid too.

    #map map {<x>x<y>} {filename} {a}
Shows a map of surrounding rooms. The {horizontal x vertical} argument i.e 80x25 is optional, and so is the filename argument to log the map output to file. The {a} argument causes data to be appended so it can easily be tailed with the tail -f <file> shell command.

    #map move <direction>
This does the same as an actual movement command, updating your location on the map and creating new rooms. Useful when you are following someone and want the map to follow. You will need to create actions using '#map move', for this to work.

    #map name <room name>
Allows changing the room name, by default rooms have the same name as their internal index number. #map read <filename>: Will load the given map file.

    #map resize <size>
Resize the map, setting the maximum number of rooms.

    #map return
Returns you to your last known room after leaving the map or loading a map.

    #map roomflag avoid
When set, '#map find' will avoid a route leading through that room. Useful when you want to avoid death traps.

    #map roomflag hide
When set, '#map' will not display the map beyond this room. When mapping overlapping areas or areas that aren't build consistently you need this flag as well to stop auto-linking, unless you use void rooms.

    #map roomflag leave
When entering a room with this flag, you will automatically leave the map. Useful when set at the entrance of an unmappable maze.

    #map roomflag void
When set the room becomes a spacing room that can be used to connect otherwise overlapping areas. A void room should only have two exits. When entering a void room you are moved to the connecting room until you enter a non void room.

    #map roomflag static
When set the room will no longer be autolinked when walking around. Useful for mapping mazes. #map run <room name> [delay]: Calculates the shortest path to the destination and walks you there. The delay is optional and requires using braces. Besides the room name a list of exits can be provided for more precise matching.

    #map set <option> <value> [vnum]
Set a map value for your current room, or given room if a room vnum is provided. #map travel <direction> <delay>: Follows the direction until a dead end or an intersection is found. Use braces around the direction if you use the delay, which will add the given delay between movements.

    #map undo
Will undo your last move. If this created a room or a link they will be deleted, otherwise you'll simply move back a room. Useful if you walked into a non existant direction.

    #map uninsert <direction>
Exact opposite of the insert command.

    #map unlink <direction> [both]
Will remove the exit, this isn't two way so you can have the map properly display no exit rooms and mazes. If you use the both argument the exit is removed two-ways.

    #map vnum <low> [high]
Change the room vnum to the given number, if a range is provided the first available room in that range is selected.

    #map write <filename>
Will save the map.
