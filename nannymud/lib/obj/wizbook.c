/*

  I think this one was made by Anvil 1991.
  Updated by Celeborn in Jan 1994.
  Updated by Anna in March 1994.
*/

id(str) { return str == "book" || str=="wizbook"
                 || str=="handbook" || str=="guide" || str=="guide book"; }

short() { return "The Wizard Guide Book"; }

get() {return 1;}
drop() {return 1;}

long(str)
{
  write("A big, fat book to aspiring wizards.\n"+
        "It has the name "+this_player()->query_name()+" written in big capital letters on the cover.\n"+
        "On the back side, it tells you to type \"read page n\" to see page n.\n");

}

query_auto_load() {return "obj/wizbook:";}

reset(arg)
{
}

init()
{
  add_action("open", "open");
  if(this_player()->query_level()<20) return;
  add_action("read", "read");
}

read(str)
{
  int page;

  if(!str) return notify_fail("Read what?\n"),0;
  if (sscanf(str, "page %d", page) == 1) {
    if (file_size("/obj/wizbook/wizbook-" + page)<1) {
      write("No such page, sorry. Read page 1 for index.\n");
    return 1;
    }
    this_player()->simplemore(read_file("/obj/wizbook/wizbook-" + page));
    this_player()->simplemore("\n");
    this_player()->flush();
    return 1;
  }
  return 0;
}

open(str)
{
 if (id(str)) {

 if(this_player()->query_level()<20) 
  {
    write("It seems impossible to open!\n"); return 1;
    return 1;
  }
    write("The book is open. Just read the page you want.\n");
    return 1;
  } else
    return 0;
}





