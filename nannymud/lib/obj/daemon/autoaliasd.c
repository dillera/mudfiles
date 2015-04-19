int id(string s)
{
  return s=="demon" || s=="daemon" || s=="autoaliasd" ||
    s=="alias demon" || s=="alias daemon";
}

void reset(int arg)
{
  if(arg) return;
  move_object(this_object(),"/obj/daemon/daemonroom");
}

short() { return "the autoliasdaemon"; }

long()
{
  write("The autoaliasdaemon is a small fellow with nothing better to do\n");
  write("than mutilating peoples auto-load-strings.\n");
}


string auto_load_alias(string p)
{
  sscanf(p,"/%s",p);
  if(p[-1]=='c' && p[-2]=='.') p=p[0..strlen(p)-3];

  switch(p)
  {
  case "players/brom/obj/ear-ring":
    return "/misc/brom_earring";

  case "players/milamber/KN/cloak":
    return "/players/knight/obj/cloak";

  case "open/wedring":
    return "/obj/wedring";

  case "local/brainloader":
    return "/std/tools/brainloader";

  case "players/pell/describe":
    return "/players/akasha/obj/describer";

  default: 
    return p;
  }
}

query_prevent_shadow() { return 1; }
