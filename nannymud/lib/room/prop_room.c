static inherit "/room/room";

/*
   Orignial Code made by Qqqq.
   Taren made some changes to his code...
*/

mapping flags;


static mapping _long = ([ "Players":1,"Rooms":1,"Objects":1, "Livings":1, "Monsters":1 ]);
static mapping compressed = ([ "P":"Players","R":"Rooms","O":"Objects", "L":"Livings", "M":"Monsters"]);

#define PROPD "obj/daemon/propd"
#define LOGNAME "PROPERTY"
#define VALIDTYPE(X) (_long[X]||compressed[X])

reset(arg)
{
  string filename;
  object board;
  
  if (arg) return;
  set_light(1);
  set_short("The Property Room");
  set_long("The commands you can do in this room is:\n"+
	   "helpp gives some help.\n"+
	   "addp <propertyname> [Objects/Rooms/Players/Livings/Monsters] <propertylinks>\n"+
	   "removep <propertyname>\n"+
	   "lookupp [-FVwWL] [<propertyname>/Objects/Rooms/Players/Livings/Monsters/ALL]\n"+
	   "linkp <propertyname> <propertyname>\n"+
	   "unlinkp <propertyname> <properrtyname>\n"+
	   "changep <propertyname> <type>\n"+
	   "If you are 23++ here is some other commands:\n"+
	   "setflagp <propertyname> <flag>\n"+
	   "chownp <propertyname> <owner>\n"+
	   "If you need any help, look in /doc/properties/prop.doc or prop.tut.\n");
  add_exit("room/inner2","north");
  set_indoors();
  board=clone_object("obj/bboard");
  board->set_name("/bboard/prop_board");
  board->set_max_notes(100);
  move_object(board,this_object());
}


init()
{
  ::init();
  if (call_other(this_player(), "query_level", 0) < 20) {
    write("Hey! You! Puny mortal! " +
	  "What are you trying to do in here? Get out!\n");
    call_other(this_player(),"move_player", "X#room/adv_guild");
    return;
  }
  if (this_player()->query_level() >= 20)
  {
    
    add_action("load","loadp");
    add_action("help","helpp");
    
    add_action("newp","newp");
    add_action("addp","addp");
    add_action("removep","removep");
    add_action("_lookupp","lookupp");
    add_action("linkp","linkp");
    add_action("unlinkp","unlinkp");
    add_action("changep","changep");
    if (this_player()->query_level() >= 23)
    {
      add_action("setflagp","setflagp");
      add_action("chownp","chownp");
    }
  }
}

     

load()
{
  mapping database;
  mixed *in;
  int i;
  database= ([ ]);
  if(find_object(PROPD))
    destruct(find_object(PROPD));
  database=PROPD->query_database();
  in=m_indices(database);
  for(i=0;i<sizeof(in);i++)
    write(in[i]+"\n");
  write("\n");
  return 1;
}

help(string str)
{
  switch(str)
  {
  case "addp":
    write("addp <propertyname> [Objects/Rooms/Players/Monsters/Livings] <propertylinks>\n"+
	  "This command is used to add new properties, simply give the propertyname, the\n"+ 
	  "type and to what other properties the property should have hardlinks.\n"+
	  "Note that the links can also be manipulated with other commands.\n");
    return 1;
  case "removep":
    write("removep <propertyname>\n"+
	  "With this command can you remove a property. you can only remove properties\n"+
	  "that you have created and is owner to.\n"+
	  "Note that arches++ can change creator of a property.\n");
    return 1;
  case "lookupp":
    write(
	  "lookupp [-FVWw] [<propertyname>/Objects/Rooms/Players/Monsters/Livings/ALL]\n\n"+
	  
	  "This is the command you use view the data in the database.\n"+
	  "If you use a <propertyname> then the -F and -V is worthless.\n\n"+
	  
	  "For the other commands then -F gives:\n"+
	  "<propertyname> (<type>;<statusflag>)\n\n"+
	  
	  "With -V then you get:\n"+
	  "<type>  <propertyname>  <statusflag>    <Description>\n\n"+
	  
	  "With -W <string>\n"+
	  "Then you will get all properties that has the <string> in their description.\n\n"+
	  "With -L <string>\n"+
	  "You will see all the properties defined.\n\n"+
	  "With -w <string>\n"+
	  "Then you will get all properties that has the <string> in their names.\n\n"+
	  
	  "If you use the alternativ Objects then you only will see properties defined\n"+
	  "on objects. Monsters, Livings, Players and Rooms are trivial.\n"+
	  "All will give you all properties.\n"+
	  "The properties is always presented in letter order.\n");
    return 1;
  case "linkp":
    write("linkp <propertyname1> <propertyname2>\n\n"+	    
	  "Makes a hardlink on propertyname1 to propertyname2.\n"+
	  "Anyone can add a link to any properties.\n");
    return 1;
  case "unlinkp":
    write("unlinkp <propertyname1> <properrtyname2>\n"+
	  "Remove a hardlink on propertyname1 that goes to propertyname2.\n"+
	  "Anyone can also do this to all properties.\n");
    return 1;
  case "changep":
    write("changep <propertyname> <type>\n"+
	  "Lets you change the type for a property. Valid types are: Players, Objects,\n"+
	  "Livings, Monsters and Rooms. You can only do that on your own properties.\n"+
	  "Arches++ can do it to anything as usual.\n");
    return 1;
  case "setflagp":
    write("setflagp <propertyname> <flag>\n"+
	  "Lets you change the statusflag on any property.\n"+
	  "ONLY 23++\n");
    return 1;
  case "chownp":
    write("chownp <propertyname> <owner>\n"+
	  "Lets you change the owner of a property.\n"+
	  "ONLY 23++\n");
    return 1;
  case "flag":
  case "owner":
  case "status":
    write("The flags are:\n"+
	  "* Supported by the Mudlib.\n"+
	  "+ Will be supported by the Mudlib.\n"+
	  "- Will NOT be supported by the Mudlib.\n"+
	  "When the owner is Mudlib it is safe to assume that it will have\n"+
	  "the flag * or + set.\n");
    return 1;
  default:
    write("The commands you can use is:\n"+
	  "addp, removep, lookupp, linkp, unlinkp, changep\n"+
	  "and for the 23++ there is: setflagp, chownp\n"+
	  "For information about the flags and owner do helpp flag\n"+
	  "The help is also availble in the file :\n"+
	  "/doc/properties/prop.doc\n"+
	  "There is also some other documents about properties in that\n"+
	  "directory.\n");
    return 1;
  }
}

static string namn,type,filename;
static string *links;

static nomask
addp(str) 
{
  string rest;
  string *array;
  int i,size;
  links = ({ });
  notify_fail("more /doc/build/prop.doc\n");
  if (!str) 
    return 0;
  if (sscanf(str,"%s %s",namn,rest) != 2)
    return 0;
  array = explode(rest," ");
  type = ({ });
  
  /*Take out all validtypes*/
  while(sizeof(array) && VALIDTYPE(array[0])) { 
    if(compressed[array[0]])
      type += ({ compressed[array[0]] });
    else
      type += ({ array[0] });
    array = array[1..1000000];
  }

  if (!sizeof(type)) 
    return write("You must give some type(s).\n");
  if (PROPD->query_prop(namn)) 
    return write("There already is such a property.\n");
  
  if (stringp(rest) && (links = array) && (size = sizeof(links)))
    for(i=0;i<size;i++)
      if (!PROPD->query_prop(links[i]))
	return printf("%s does not exist and you can therefor not link to it.\n",links[i]);
  
  write("Okey add the explanation for it.\n");
  write("Add text with i, and finish it with a '.' on a new line.\n");
  write("Press 'x' to save the new property.\n");
  write("End with doing w and then q, if you want to cancel just do Q\n");
  filename = "/open/qqqq_soap/tmpfile"+(time());
  ed(filename,"exit_ed");
  return 1;
}

nomask
exit_ed() 
{
  string desc;
  int size,i;
  if (file_size(filename) < 1)
    return write("Aborted.\n");
  if (PROPD->query_prop(namn))
    return write("Someone added this property while you were writing the description.\n");
  if (!(desc = read_file(filename)))
    return write("Can't read the file, aborted.\n");
  rm(filename);
  
  size = sizeof(links);
  
  for(i=0;i<size;i++)
    if (!PROPD->query_prop(links[i]))	/*Someone removed it while in ed*/
      links -= ({ links[i] });
    else
      PROPD->addlink(links[i],namn);

  PROPD->addp(namn,({ this_player()->query_real_name(), type, desc, links, ({ }), "", time()}));
  
  log_file(LOGNAME,"ADD:" + this_player()->query_real_name() + ", " + namn+"\n");
  write("It has been added.\n");
  return 1;
}

static nomask
chownp(str) 
{
  string type,namn;
  if (this_player()->query_level() < 23) 
    return write("You are not allowed to use this command.\n");
  if (sscanf(str,"%s %s",namn,type) != 2)
    return write("RTFM!!!\n");
  if (!PROPD->query_prop(namn))
    return write("That property do not exist.\n");
  PROPD->chownp(namn,type);
  write("Done.\n");
  log_file(LOGNAME,"CHOWN:" + this_player()->query_real_name() + ", " + namn
	   +", "+type+".\n");
  return 1;
}

static nomask
changep(str) 
{
  string type,namn,rest;
  string *array;
  if (sscanf(str,"%s %s",namn,rest) != 2)
    return write("RTFM!!!\n");
  array = explode(rest," ");
  type = ({ });
  while(sizeof(array) && VALIDTYPE(array[0])) { /*Take out all validtypes*/
    type += ({ array[0] });
    array = array[1..1000000];
  }
  if (!sizeof(type) || sizeof(array))
    return write("This is not a valid type, use Objects, Rooms, Livings, Monsters or Players.\n");
  if (!PROPD->query_prop(namn))
    return write("That property do not exist.\n");
  if (PROPD->query_owner(namn) != this_player()->query_real_name() && 
      this_player()->query_level() < 23) 
    return write("Not allowed to change type on this property.\n");
  
  PROPD->changep(namn,type);
  write("Done.\n");
  log_file(LOGNAME,"CHANGE:" + this_player()->query_real_name() + ", " + namn
	   +", "+type+".\n");
 return 1;
}

static nomask
removep(str) 
{
  mixed *array,*tmp;
  int i,size;
  if (this_player()->query_level() < 23 && 
      this_player()->query_real_name() != PROPD->query_owner(str))
    return write("You are not allowed to remove that property.\n");
  
  switch(PROPD->removep(str))
  {
  case -1:
    return write("There is no such property.\n");
  case 1:break;
  default:
    return write("Unkown error.\n");
  }
  write("It has been removed.\n");
  log_file(LOGNAME,"REMOVE:" + this_player()->query_real_name() + ", " + str+"\n");
  return 1;
}

static nomask
linkp(str) 
{
  string propfran,proptill;
  if (!str || sscanf(str,"%s %s",propfran,proptill) != 2)
    return write("linkp <from> <to>\n");
  switch(PROPD->linkp(propfran,proptill))
  {
  case -1:
    return write("One or both of the properties does not exist.\n");
  case -2: 
    return write("There is already a link between these properties.\n");
    case -3:
    return write("A property can't be linked to itself.\n");
  case 1:break;
  default:
    return write("Unkown error.\n");
    }
  write("Link established.\n");
  log_file(LOGNAME,"ADDLINK:" + this_player()->query_real_name() + ", " + propfran+ ", "+proptill+"\n");
  return 1;
}

static nomask
unlinkp(str) 
{
  string propfran,proptill;
  int res;
  if (!str || sscanf(str,"%s %s",propfran,proptill) != 2)
    return write("Unlinkp: <from prop> <toprop>.\n");
  switch(PROPD->unlinkp(propfran,proptill))
  {
  case -1: return write("One or both of the properties does not exist.\n");
  case -2: return write("There is no link between these properties.\n");
  case 1:  write("Ok!\n");break;
  default: write("Unkown error.\n");break;
  }
  log_file(LOGNAME,"REMLINK:" + this_player()->query_real_name() + ", " + propfran+ ", "+proptill+"\n");
  return 1;
}

static nomask
setflagp(str) 
{
  string namn,flag;
  if (this_player()->query_level() < 23)
    return write("Hey, that is illegal.\n");
  if (!str || sscanf(str,"%s %s",namn,flag) < 2)
    return write("Valid flags are:\n'*' means supported by the MUDLIB\n"+
		 "'+' Soon to be supported by the MUDLIB.\n");  
  if (!PROPD->query_prop(namn))
    return write("Must be an existing property.\n");
  PROPD->set_flag(namn,flag);
  log_file(LOGNAME,"FLAG:" + this_player()->query_real_name() + ", " +
	   namn + ", " + flag + "\n");
  write("Flag set.\n");
  return 1;
}

alfa(str1,str2)
{
  return str1==str2; 
}

static take_out_arg(tmp)
{
  for(;strlen(tmp) > 0;tmp = tmp[1..100000]) flags[tmp[0..0]] = 1;
}

mapping database = ([ ]);

_lookupp(str)
{
  database=PROPD->query_database();
  lookupp(str),
  database=([]);
  return 1;
}

int col_size,col2;

int scale_fun(string str) 
{
  string a;
  a=str+"\n\n\n\n";
  if(col_size<strlen(str))
  {    
    col_size=strlen(str);
    col2=67-col_size;
  }
  return a[0]*0x200000+a[1]*0x4000+a[2]*0x80+a[3];
}

sort(arr)
{  
  return m_values(mkmapping(map_array(arr,"scale_fun",this_object(),1),
			  arr));  
}

static nomask lookupp(str) 
{
  string mcom,tmp;
  int i,size;
  mixed array;
  col_size=5;
  for(flags = ([]), mcom = str; mcom&&(sscanf(mcom, "-%s %s", tmp, mcom) == 2
				       || (sscanf(mcom, "-%s", tmp) == 1 && !(mcom = 0)));
      take_out_arg(tmp));
  str = mcom;
  if (!str) 
    return write("Must give an argument.\n");
  if (str == "ALL")
  {
    size = sizeof(array = m_indices(database));
    if (!size) 
      return write("There isn't any properties saved.\n");
    array = sort(array);
    write("All properties:\n");
    for(i=0;i<(size-1);i++) 
      write(print_out(array[i]));
    write(print_out(array[i],1)+"\n");
    return 1;
  }
  if (VALIDTYPE(str))
  {
    
    size = sizeof(array = m_indices(database));
    if (!size) 
      return write("There isn't any properties saved.\n");
    array = sort(array);
    for(i=0;i<size;i++)
      if (member_array(str,database[array[i]][1]) != -1)
	write(print_out(array[i]));
    write("that is all properties of type "+str+"\n");
    return 1;
  }
  if (flags["w"])
  {
    array = regexp(m_indices(database), str);
    size = sizeof(array);
    if (!size) 
      return write("There isn't any properties with that in it is name.\n");
    array = sort(array);
    for(i=0;i<(size-1);i++) 
      write(print_out(array[i]));
    write(print_out(array[i],1)+"\n");
    return 1;
  }
  if (flags["W"])
  {
    size = sizeof(array = m_indices(database));
    str = lower_case(str);
    if (!size) 
      return write("There isn't any properties saved.\n");
    array = sort(array);
    for(i=0;i<(size-1);i++)
      if (sizeof(regexp(explode(lower_case(database[array[i]][2]),"\n"),str)))
	write(print_out(array[i]));
    write(print_out(array[i],1)+"\n");
    write("that is all properties that matches "+str+".\n");
    return 1;
  }
  if(flags["L"])
  {
    map_array(m_indices(database),"lookupp",this_object());
    return 1;
  }
  if (database[str])
  {
    write("Property: "+str+"\n");
    write("Type    : "+
	  sprintf("%-9@s\n",sort(database[str][1])));
    write("Creator : "+capitalize(database[str][0])+"\n");
    write("Status  : "+database[str][5]+"\n");
    size = sizeof(array = PROPD->give_me_links(str));
    if (size) {
      write("Properties that are set if this one is used:\n");
      for(i=0;i<(size-1);i++) 
	write(print_out(array[i]));
      write(print_out(array[i],1)+"\n");
    }
    size = sizeof(array = database[str][4]);
    if (size) {
      write("Properties that set this property when used:\n");
      for(i=0;i<(size-1);i++) 
	write(print_out(array[i]));
      write(print_out(array[i],1)+"\n");
    }    
    if (database[str][2] && database[str][2] != "") 
      write("The description is:\n"+database[str][2]);
    return 1;
  }
  write("No such property.\n");
  return 1;
}

print_out(namn,flag)
{
  string tmp;
  tmp = "";
  if (!pointerp(database[namn][1]))
    /*This was just to convert from old format*/
    database[namn][1] = ({ database[namn][1] });
  while (strlen(tmp) < sizeof(database[namn][1]))
    tmp += database[namn][1][strlen(tmp)][0..0];
  if (flags["V"]) 
    return sprintf("%-2s %-6s %"+col_size+"|s %-"+col2+"=s\n",
		   database[namn][5],tmp,namn
		   ,replace(database[namn][2],"\n"," "));
  if (flags["F"])
    if (flag)
      return sprintf("%s (%s;%s) ",namn,tmp,database[namn][5]);
    else
      return sprintf("%s (%s;%s), ",namn,tmp,database[namn][5]);
  if (flag) 
    return namn;
  else
    return sprintf("%s, ",namn);
}
