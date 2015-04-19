/* Room Converter Made By Rohan 950718
   with inspiration from Tarens early room upfresher.
   | 
   | Main purpose is to use this object to convert standard /room/room
   | to /std/room.c
   |__________
   | Changelog:
   | 950906 Fixed add_exit with 3 and 4 args. Fixed linebreaking better.
   |        Fixed bug in realm. Fixed so you get warning if verbose and
   |        /room/room is indirectly inherited.
   |
*/

#pragma strict_types;

#define TO this_object()
#define TP this_player(1)
#define ENV(str) environment(str)
#define CHECK_CMD "check"
#define Verbose(str) if (show_all) write(str)
#define Write(str) if (!silent) write(str)
#define Debug(str) if(debug) write(str)
#define max_eval 150
#define hard_work 50
#define VERSION "0.51"

int TIME=32; /* Not to small, consider the lag this thing creates...*/
int show_all=0;
int write_2_file=0;
int recursive=0;
int very_recursive=0;
int silent=0;
int force=0;
int mega_force=0;
int debug=0;
/* int indent=0; */
int stop_parsing=0; /* Should we stop recursiv parsing */
int parsing=0; /* Do not allow several parsings simultanius */
int parsed_inherit_room;

object current_room;
object new_room;
string instr=""; /* String to store all of infile in */
int instrp; /* Keep track of where in instr we are parsing */
string outstr=""; /* The string to be written back to file. */
mixed *the_dir_global_save;

/* Some Statistics */
int no_rooms=0;
int no_files=0;
int no_changed=0;
int stime=0;
/*******************/
/* Some stuff that will help parsing */
string outfile_name;
int lastchar; /* This is the number of chars in instr */
int in_string; /* True if we are parsing inside a string */
int in_comment; /* True if we are parsing inside a comment */
string token_type; /* To store the type of token found with get_token */
mapping letters=(["a":1,"b":1,"c":1,"d":1,"e":1,"f":1,"g":1,"h":1,"i":1,
                  "j":1,"k":1,"l":1,"m":1,"n":1,"o":1,"p":1,"q":1,"r":1,
                  "s":1,"t":1,"u":1,"v":1,"w":1,"x":1,"y":1,"z":1,
                  "A":1,"B":1,"C":1,"D":1,"E":1,"F":1,"G":1,"H":1,"I":1,
                  "J":1,"K":1,"L":1,"M":1,"N":1,"O":1,"P":1,"Q":1,"R":1,
                  "S":1,"T":1,"U":1,"V":1,"W":1,"X":1,"Y":1,"Z":1,
                  "_":1]);
mapping blanks=([" ":1,"\n":1,"\t":1]);

int check_room(string file); /* Forward declare check_room */

string short(){ return "Muzgash, the Room Converter";}

void long()
{ 
write("This is Muzgash, the Room Converter.\n"+
      "Use '"+CHECK_CMD+" -h' to get help.\n"+
      "This object was coded by Rohan with inspiration from Tarens early version\n"+
      "Muzgash converts rooms inheiting /room/room to inherit /std/room\n"+
      "Please report any bugs as soon as possible.\n"+
      "--> The creator of Muzgash does NOT garantee that the converted code work <--\n");
}

int id(string str)
{
  if(str) str=lower_case(str);
  if(str=="muzgash" || str=="foo" || str=="checker" || str==CHECK_CMD) return 1;
}

void init()
{
  if(TP->query_level()<21) return destruct(this_object());
  add_action("check_room1",CHECK_CMD);
}

int get(){return 1;}
int drop(){return 1;}

void mv(string from,string to) {
  /* Useing NAIVE mode!!! */
  string tmp;
  tmp=read_file(from);
  write_file(to,tmp);
  rm(from);
}

help(){
  write("Muzgash, the Room Converter says:\n"+
	"'I understand these command:\n"+
	CHECK_CMD+" [-hrRdsvwFDq] <room>\n"+
	" -h this help\n"+
        " -p Problems. Look at this helptext have problems getting it working.\n"+
	" -r recursive in this case it means all files in a dir.\n"+
	" -R recursive in this case it means all files in a dir plus all subdirs.\n"+
	" -d if the Target file is found, erase it\n"+
	" -s Very Silent, no output exept from the ready message.\n"+
	" -v verbose mode. Prints a bunch of things.\n"+
	" -w write to file, the new file will be called <room>_new.c\n"+
	" -F write to file and use the old filename. DON'T USE THIS without backups.\n"+
/*	" -I Indent file.\n"+ No indenting in this version. */
        " -T Turbo mode, makes less pausing but causes more lag.\n"+
        " -D Debug mode (prints a lot of debug-messages)\n"+
	" -q Stop ongoing parsing.\n"+
	"If no <room> is given the environment of me will be used.\n\n"+
        "Capacity for recursive parsing is approx 100 rooms per hour.\n"+ 
        "Muzgash currently has version: "+VERSION+" and is able to handle:\n"+
        "inherit room/room, "+
        "dest_dir =, "+
        "add_exit(<args>), "+
        "short_desc =, "+
        "long_desc =, "+
        "\n"+
        "inorout =, "+
        "items =, "+
        "property =, "+
        "set_indoors(), "+
        "set_outdoors(), "+
        "set_inorout(n), "+
        "\n"+
        "set_realm(), "+
        "realm(), "+
        "the_realm =, "+
        "set_no_castle_flag(arg), "+
        "no_castle_flag =, "+
        "\n"+
        "add_item()"+
        "\n\n"+
        "Known things that this version does not handle:\n"+
        "functions outside reset\n"+
        "blocked_exits =\n"+ /*  ({ 0, "check_south", 0, 0 ... }) */
        "item_commands =\n"+ /*  ({ 0, 0, "open_window", ({ "open", "un-close" }) }) */
        "extra_commands =\n" /*  ({ "cmd_jump", ({ "jump", "suicide" }) ... }) */
        );
}

help2(){
  write("Muzgash, the Room Converter says:\n"+
        "The version of the room-converter that should be used has\n"+
        "filename /std/tools/converter.c make sure not to clone the\n"+
        "version under Rohans dir since that one might be under construction\n"+
        "and have buggs.\n"+
        "Before you start parsing rooms make sure that this converter\n"+
        "has ACL-access, the easiest way to fix this is:\n"+
        "edac ~ /std/tools/:ALL\n"+
        "(This will give it access to your home-dir.)\n"+
        "The most normal ways to use this converter is:\n"+
        "check -w <file.c>   (Writes output on <file_new.c>)\n"+
        "check -F <file.c>   (Writes output on same file as infile.)\n"+
        "                    (Original file is stored under <file.old>)\n"+
        "check -RvF /players/rohan/rooms/    (This means:\n"+
        "                    parse all files under /players/rohan/rooms/\n"+
        "                    recursivly using verbose mode and write\n"+
        "                    each file to its original file.)\n"+
        "check -vD <file.c>  (Writes among other things output on screen)\n"+
        "                    (This works even when ACL-rights write is not set)\n");
}

void stop(){
  stop_parsing=1;
  write("Muzgash, the Room Converter tells you: Slowing down...\n");
}

int get_flags(string flags)
{
  int i;
  for(i=0;i<strlen(flags);i++){
    switch(flags[i]) {
    case 'w': write_2_file=1; break;
    case 'v': show_all=1; break;
    case 'D': debug=1; break;
    case 's': silent=1; break;
    case 'd': force=1;break;
    case 'F': mega_force=1; write_2_file=1; break;
/*    case 'I': indent=1;break; */
    case 'r': recursive=1; break;
    case 'R': very_recursive=1; break;
    case 'T': if (TP->query_level()<23) 
        write("Flag T is for level 23++ only to prevent to much lag.\n"); 
      else
        if (TP->query_real_name()=="rohan") TIME=4;
        else TIME=10;
      break;
    case 'h': help(); return 2;
    case 'p': help2(); return 2;
    case 'q': stop(); return 2;
    case 'Q': stop(); return 2;
    }
  }
  return 1;
}

void ready(){
 write("Muzgash tells you: I am ready.\n"+
       "I scanned "+no_files+" files and parsed "+no_rooms+" rooms.\n"+
       "I ");
 if (write_2_file) write("changed\n");
 else write("suggested changes to ");
 write(no_changed+" of the rooms. Parsing time was ");
 write(time()-stime+" seconds.\n");
 parsing=0;
}

int recurs_inner(mixed *the_args) { /* array that always has 2 elements */
 /* This is since call_out only sends 1 arg. */
mixed *the_dir, *new_dir;
string dir_to_expand, file_to_parse;
int i;
int arr_size;
string tmp;
  if (stop_parsing) {
    write("Stopped parsing on user request.\n");
    ready();
    return 1;
  }
  the_dir=the_args[0];
  dir_to_expand=the_args[1];
  if (dir_to_expand!="") {
    Verbose("Expanding dir "+dir_to_expand+"\n");
    new_dir=get_dir(dir_to_expand);
    for(i=0;i<sizeof(new_dir);i++) {
/*      write("adding "+new_dir[i]+" to list\n");*/
      the_dir=the_dir+({dir_to_expand+new_dir[i]});
    }
    if (!new_dir) {
      Verbose("No files found in directory : "+dir_to_expand+"\n");
      return 1;
    }
  }
/*
write("## START WRITING DIR.\n");
  for(i=0;i<sizeof(the_dir);i++) {
      write(the_dir[i]+"\n"); }
write("## STOP WRITING DIR.\n");
*/
  arr_size=sizeof(the_dir);
  if (arr_size==0) {
    /* Whee, we are ready. */
    ready();
    return 1;
  }
  i=strlen(the_dir[0]);
  if ((the_dir[0][i-2..i-1]==".c") && (file_size(the_dir[0])>0)) {
    /* A file (ends with .c) */
    Write("Parse file "+the_dir[0]+"\n");
    no_files++;
    file_to_parse=the_dir[0];
    the_dir=the_dir[1..arr_size-1];
    the_dir_global_save=the_dir; /* So parse can use this var to send back
                                    when the room is parsed. */
    check_room(file_to_parse);
/*    call_out("recurs_inner",2,({the_dir, ""}) );*/
    return 1;
  }
  if (the_dir[0][i-1..i-1]=="/") {
    /* A directory (ends with /) */
    call_out("recurs_inner",2,({the_dir[1..arr_size-1], the_dir[0]}) );
    return 1;
  }
  /* Not a .c file or a dir */
  Verbose("Ignoring file: "+the_dir[0]+"\n");
  no_files++;
  return recurs_inner(({the_dir[1..arr_size-1],""}));  
}

int recurs(string dire, mixed *this_level){
  mixed dir;
  string tmp;
  int i,time;
  if (dire) dir=get_dir(dire);
  if (!dir) {
    tmp=dire;
    dire="/"+(string) TP->query_path()+"/";
    if (tmp) dire=dire+tmp;
    dir=get_dir(dire);
  }
  if (!dir) {
    if(silent) write("No files found in directory : "+dire+"\n");
    return 1;
  }
  Debug("Files found ("+dire+"):\n");
  for(i=0;i<sizeof(dir);i++) {
    if((file_size(dire+dir[i])>0)&&(sscanf(dir[i],"%s.c",tmp)==1)) {
      this_level=this_level+({dire+dir[i]});
    }
    else 
      if (very_recursive) /* recurs(dire+dir[i]+"/"); */
        this_level=this_level+({dire+dir[i]+"/"});
  }
  recurs_inner(({this_level, ""}));
  return 1;
}

string remove_leading_blanks(string the_string) {
int i;
string this_char;
  i=0;
  this_char=the_string[i..i];
  while (blanks[this_char]) { i++; this_char=the_string[i..i]; }
  return the_string[i..strlen(the_string)];
}

string remove_ending_blanks(string the_string) {
int i;
string this_char;
  i=strlen(the_string)-1;
  this_char=the_string[i..i];
  while (blanks[this_char]) { i--; this_char=the_string[i..i]; }
  return the_string[0..i];
}

string get_token() {
string retstr;
string blankchars;
string tmpstr, tmpstr2;
string nextchar, thischar;
int startp;

  blankchars="";
  retstr="";
  startp=instrp;
  if (!instr[instrp]) { token_type="end_token"; return "";  }
  thischar=instr[instrp..instrp];
  while (blanks[thischar]) { instrp++;  thischar=instr[instrp..instrp]; }
  if (instrp>startp) {
    /* there was some blanks, lets return them as a token. */
    blankchars=instr[startp..(instrp-1)];
    token_type="blanks";
    return blankchars;
  }

  startp=instrp;
  while (letters[thischar]) { instrp++; thischar=instr[instrp..instrp]; }
  if (instrp>startp) {
    /* We have found a token */
    retstr=instr[startp..(instrp-1)];
    token_type="ident";
    return retstr;
  }
  /* This must be a special char since it is not a letter and not a blank */
  /* lets check for a few special tokens that are more than one chars long: */
  /* These tokens are  ({  ([  (startcomment)   also look for "             */
  token_type="special_char";
  nextchar=instr[instrp+1..instrp+1]; thischar=instr[instrp..instrp]; instrp++;
  switch (thischar) {
    case ",": { token_type="comma"; return ","; }
    case ";": { token_type="semicolon"; return ";"; }
    case ")": { token_type="end_parentes"; return ")"; }
    case "(": /* Look for start array and start mapping */
      if (nextchar=="{") { instrp++; token_type="start_array"; return "({"; } 
      if (nextchar=="[") { instrp++; token_type="start_mapping"; return "([";} 
      token_type="start_parentes";
      return "(";
      break;
    case "}": /* Look for end array */
      if (nextchar==")") { instrp++; token_type="end_array"; return "})"; } 
      return "}";
      break;
    case "]": /* Look for end mapping */
      if (nextchar==")") { instrp++; token_type="end_mapping"; return "])"; } 
      return "]";
      break;
    case "-": /* Look for end mapping */
      if (nextchar==">") { instrp++; token_type="call_other"; return "->"; } 
      return "-";
      break;
    case "/": /* Look for start comment */
      if (nextchar=="*") { 
        /* Here goes a start comment lets parse to the end */
        instrp++;
        retstr="/*";
        if (in_string) { token_type="start_comment"; return retstr; }
        in_comment=instrp; 
	thischar=instr[instrp..instrp];
        nextchar=instr[instrp+1..instrp+1];
	while (!((thischar=="*") && (nextchar=="/"))) {
          instrp++;
	  thischar=instr[instrp..instrp];
	  nextchar=instr[instrp+1..instrp+1];
        }
        instrp+=2;
        retstr+=instr[in_comment..instrp];
        in_comment=0;
        token_type="comment";
        return retstr;
        break;
      }
      token_type="slash";
      return "/";
      break;
    case "\\":
      if (nextchar=="\\") {  /* Look for quoted \  */
        instrp++;
        retstr="\\\\";
        token_type="quoted_backslask";
        return retstr;
      }
      if (nextchar=="\"") {  /* Look for quoted "  */
        instrp++;
        retstr="\\\"";
        token_type="quoted_fnutt";
        return retstr;
      }
      return "\\"; /* Hmm, what is this? Unknown quote perhaps. */
      break;
    case "*": /* Look for end comment */
      if (nextchar=="/") { 
        instrp++;
        token_type="end_comment";
        return "*/";
      } 
      return "*";
      break;
    case "\"": /* This is the string-" */
      if (in_string) { /* We are inside a string already, so this must be
                      the end of a string or substring. */
        token_type="end_string";
        in_string=0; /* Ok, lets set this so get_string works */
        return "\"";
      }
      token_type="start_string";
      in_string=1; /* Ok, lets set this so get_string works */
      return "\"";
      break;
    default: /* Ok, this must be a one char token. */
      retstr=thischar;
      return retstr;
      break;
  } /* end switch */
}

string get_string()
{
int startp, end_of_string;
string retstr, last_token;
  retstr="";
  startp=instrp;
  end_of_string=0;
  last_token=get_token();
  while ((last_token!="") && !end_of_string) {
    if (in_string) {
      retstr+=last_token;
    }
    else {
      switch (token_type) {
      case "comma":
      case "semicolon":
      case "end_parentes":
      case "end_array":
      case "end_mapping":
      case "call_other":
	{ 
	  end_of_string=1; 
	  break;
	}
      default: retstr+=last_token;
      }
    }  
    if (!end_of_string) {
      startp=instrp;
      last_token=get_token();
    }
    else {
      instrp=startp;
    }
  } /* while */
  token_type="string";
  return retstr;
} /* get_string */

string get_array()
{
string retstr, tmpstr;
int startp;
int level;
  startp=instrp;
  tmpstr=get_token();
  retstr="";
  if (token_type=="blanks") tmpstr=get_token();
  if (tmpstr!="({") {
    /* Oh, shit this is not an array and it should be... */
    write("Start array was expected but "+tmpstr+" was encountered.\n");
    write("Parsing is very likely to fail now.\n");
    instrp=startp;
    return "({})"; /* Try returning an empty array */
  }
  level=1;
  retstr+=tmpstr;
  while ((level>0) && (tmpstr!="")) {
    tmpstr=get_token();
    retstr+=tmpstr;
    switch (token_type) {
      case "start_array": level++; break;
      case "end_array": level--; break;
      default: break;
    } /* switch */
  }
  token_type="array";
  return retstr;
}

string get_mapping()
{
string retstr, tmpstr;
int startp;
int level;
  startp=instrp;
  tmpstr=get_token();
  retstr="";
  if (token_type=="blanks") tmpstr=get_token();
  if (tmpstr!="([") {
    /* Oh, shit this is not a mapping and it should be... */
    write("Start mapping was expected but "+tmpstr+" was encountered.\n");
    write("Parsing is very likely to fail now.\n");
    instrp=startp;
    return "([])"; /* Try returning an empty mapping */
  }
  level=1;
  retstr+=tmpstr;
  while ((level>0) && (tmpstr!="")) {
    tmpstr=get_token();
    retstr+=tmpstr;
    switch (token_type) {
      case "start_mapping": level++; break;
      case "end_mapping": level--; break;
      default: break;
    } /* switch */
  }
  token_type="mapping";
  return retstr;
}

string get_mixed() /* Returns a string contaning a string/mapping/array 
                      token_type will also be set correctly. */
{
string retstr, tmpstr;
int startp;
int level;
  startp=instrp;
  tmpstr=get_token();
  if (token_type=="blanks") { startp=instrp; tmpstr=get_token(); }
  switch (token_type) {
    case "start_mapping": instrp=startp; return get_mapping(); break;
    case "start_array": instrp=startp; return get_array(); break;
    case "start_string": instrp=startp; in_string=0; return get_string(); 
                         break;
    default: return tmpstr; break;
  } /* switch */
}

string strip_array(string str)
{
int first, last;
string char;
  first=0;
  last=strlen(str)-1;
  char=str[first..first+1];
  while (char!="({") {
    first++;
    char=str[first..first+1];
  }
  first=first+2;
  char=str[last-1..last];
  while (char!="})") {
    last--;
    char=str[last-1..last];
  }
  last=last-2;
  char=str[first..last];
  return char;
}

int create_add_item_cmd(string verb, string item, string func_name, string leading_blanks)
{
int i;
  func_name=remove_leading_blanks(func_name);
  func_name=remove_ending_blanks(func_name);
  func_name=func_name[1..strlen(func_name)-2];
  item=remove_leading_blanks(item);
  if (item[0..0]=="(") {
    /* This must be an array, lets take the first string in the array */
    item=strip_array(item);
    i=0;
    while (item[i..i]!=",") i++;
    item=item[0..i-1];
  }
  outstr+=leading_blanks+"add_item_cmd("+verb+","+item+
          ",\"@"+func_name+"()\");";
  return 1;
}

int create_add_item(mixed description, string items, string leading_blanks)
{
int lastchar;
  description=remove_ending_blanks(description);
  lastchar=strlen(description);
  if (description[lastchar-1..lastchar-1]!="\n")
    description=description[0..lastchar-2]+".\\n\"";
  outstr+="add_item("+items+",\n"+
          leading_blanks+"  "+description+");";
  return 1;
}

int create_add_exit4(mixed directions, string filename, string arg3, string arg4)
{
  directions=remove_leading_blanks(directions);
  filename=remove_leading_blanks(filename);
  if ((filename[0..0]=="\"") && (filename[1..1]!="/")) {
    /* We need to add a / in the filename */
    outstr+="add_exit("+directions+",\"/"+filename[1..strlen(filename)-1]+","+arg3+","+arg4+");";
    return 1;
  }
  /* Ok filename starts with / so we don't have to add it */
  outstr+="add_exit("+directions+","+filename+","+arg3+","+arg4+");";
  return 1;
}

int create_add_exit3(mixed directions, string filename, string arg3)
{
  directions=remove_leading_blanks(directions);
  filename=remove_leading_blanks(filename);
  if ((filename[0..0]=="\"") && (filename[1..1]!="/")) {
    /* We need to add a / in the filename */
    outstr+="add_exit("+directions+",\"/"+filename[1..strlen(filename)-1]+","+arg3+");";
    return 1;
  }
  /* Ok filename starts with / so we don't have to add it */
  outstr+="add_exit("+directions+","+filename+","+arg3+");";
  return 1;
}

int create_add_exit(mixed directions, string filename)
{
  directions=remove_leading_blanks(directions);
  filename=remove_leading_blanks(filename);
  if ((filename[0..0]=="\"") && (filename[1..1]!="/")) {
    /* We need to add a / in the filename */
    outstr+="add_exit("+directions+",\"/"+filename[1..strlen(filename)-1]+");";
    return 1;
  }
  /* Ok filename starts with / so we don't have to add it */
  outstr+="add_exit("+directions+","+filename+");";
  return 1;
}

int create_add_property(mixed the_property)
{
  outstr+="add_property("+the_property+");";
  return 1;
}

create_states_from_array(mixed the_string,
                         string leading_blanks,
                         string creation_type)
{
string tmpinstr;
int tmpinstrp;
string filename;
string directions;
string tmpstr;
int maxlength;
int first_loop;
int nr_args; /* The number of args each create call should have */
/*  write("%"+the_string+"\n"); Good for debugging this function */
  nr_args=2; /* The default */
  if (creation_type=="add_property") nr_args=1;
  first_loop=1;
  tmpinstr=instr; /* Very ugly, but makes me able to reuse code */
  tmpinstrp=instrp;
  instr=the_string;
  instrp=0;
  maxlength=strlen(the_string);
  while (maxlength>0) {
    filename="";
    while ((filename=="") && (instrp<maxlength)) {
      tmpstr=instr[instrp..instrp];
      switch (tmpstr) {
        case "\"":
        case "(":
          filename=get_mixed();
	  break;
        default: instrp++; break;
      } /* switch */
    }
    if (!filename || (filename=="")) { 
      /* There is no more exits in the array */
      instr=tmpinstr;
      instrp=tmpinstrp;
      return 1;
    }
    /* Filename found */
    if (nr_args>1) {
      directions="";
      while ((directions=="") && (instrp<=maxlength)) {
	tmpstr=instr[instrp..instrp];
	switch (tmpstr) {
          case "\"":
	  case "(":
	    directions=get_mixed();
	    break;
	  case "}":
	    /* There are no more exits to add. */
	    /* This should never happen here */
	    instr=tmpinstr;
	    instrp=tmpinstrp;
	    return 1;
	    break;
	  default: instrp++; break;
	} /* switch */
      }    
      if (!directions || (directions=="")) { 
	/* There is no more exits in the array */
	instr=tmpinstr;
	instrp=tmpinstrp;
	return 1;
      }
    }
    if (!first_loop) outstr+=leading_blanks;
    if (creation_type=="add_exit") {
      create_add_exit(directions, filename);
      outstr+="\n";
    }
    if (creation_type=="add_item") {
      create_add_item(directions, filename, leading_blanks);
      outstr+="\n";
    }
    if (creation_type=="add_property") {
      create_add_property(filename);
      outstr+="\n";
    }
    first_loop=0;
  }
  instr=tmpinstr; /* Lets take back the very ugly hack... */
  instrp=tmpinstrp;
  return 1;
}  /* create_states_from_array */

create_add_exits(mixed the_array, mixed leading_blanks)
{
  the_array=strip_array(the_array);
  if (!stringp(leading_blanks)) leading_blanks="";
  create_states_from_array(the_array, leading_blanks, "add_exit");
  return 1;
}

create_add_items(mixed the_array, mixed leading_blanks)
{
  the_array=strip_array(the_array);
  if (!stringp(leading_blanks)) leading_blanks="";
  create_states_from_array(the_array, leading_blanks, "add_item");
  return 1;
}

create_add_propertys(mixed the_propertys, string leading_blanks)
{
  the_propertys=strip_array(the_propertys);
  if (!stringp(leading_blanks)) leading_blanks="";
  create_states_from_array(the_propertys, leading_blanks, "add_property");
  return 1;  
}

int in_reset;
int level;
int inorout;
parse_init()
{
  inorout=0;
  level=0;
  in_reset=0;
  outstr="";
  parse(1);
  return 1;
}

parse(int reset_token)
{
string token;
string tmpstr, tmpstr2, tmpstr3, tmpstr4, blanks;
int i;
int eval_loops;
  Debug("***** ENTER PARSE\n");
  eval_loops=0;
  if (reset_token) token="XXX"; /* Just so token != "" */
/* while (token!="") {
    token=get_token();
    outstr=outstr+token;
    write("# "+token_type+" "+token+"\n");
  }
  Verbose("## END OF FILE.\n");
  write(outstr);
  Verbose("## END OF DUMPING outstr.\n");
    write("#Token= "+token+" type: "+token_type+" length: "+strlen(token)+"\n");
  return 1;
*/
/* Comment out the above to run normally, it is just for testing. */

  while (!in_reset && (token!="")) {
    eval_loops++;
    if (eval_loops>max_eval) { call_out("parse",2,0); return 0; }
    token=get_token();
    Debug(token_type+" "+token+"\n");
    switch(token) {
      case "\"": if (token_type!="start_string") {
                   outstr+=token; break;
                 }
                 else {
                   outstr+=token;
                   while (token_type!="end_string") {
                     token=get_token();
                     Debug("In string: "+token_type+" "+token+"\n");
                     outstr+=token;
                   }
                   break;
                 }
      case "inherit": tmpstr="";
	              i=instrp;
                      tmpstr=get_token();
                      if (token_type=="blanks") {
                        token+=tmpstr;
			i=instrp;
                        tmpstr=get_token();
                      }
                      if (token_type=="start_string") {
                        instrp=i;
                        in_string=0; /* We backtrack out of the string */
                        tmpstr=get_string();
                      }
                      if (tmpstr=="\"room/room\"" || tmpstr=="\"room/room.c\"" ||
                        tmpstr=="\"/room/room\"" || tmpstr=="\"/room/room.c\"" ||
                        tmpstr=="\"<room>\"" || tmpstr=="\"<room.c>\"") {
                        /* I hope this is all the ways people write in the
                           inherit string */
                        /* Lets replace /room/room.c with /std/room */
                        outstr+=token+"\"/std/room\"";
			parsed_inherit_room=1;
                        break;
                      }
                      /* This was inherit of something else, lets keep it. */
                      outstr+=token+tmpstr;
                      break;
      case "reset": 
	in_reset=1; 
	outstr+=token; 
	while (token_type!="end_parentes") { /* find name of arg to reset */
	  tmpstr=token;
          tmpstr2=token_type;
	  token=get_token();
	  outstr+=token;
	}
	while (token!="{") {
	  token=get_token();
	  outstr+=token;
	}
	if (tmpstr2=="ident")
	  outstr+="\n  ::reset("+tmpstr+");";
	else
	  outstr+="\n  ::reset();";
	break;
      default: outstr+=token; break;
    } /* switch */
  }
  /* PARSE RESET */
  /* Lets parse the reset function only */
  while (in_reset && (token!="")) {
    eval_loops++;
    if (eval_loops>max_eval) { call_out("parse",2,0); return 0; }
    token=get_token();
Debug(token_type+" "+token+"\n");
/* write("###"+token+" "+level+" "+in_reset+"\n"); ### debug only */
    switch(token) {
      case "\"": if (token_type!="start_string") {
                   outstr+=token; break;
                 }
                 else {
                   outstr+=token;
                   while (token_type!="end_string") {
                     token=get_token();
                     Debug("In string: "+token_type+" "+token+"\n");
                     outstr+=token;
                   }
                   break;
                 }
      case "{": level++; outstr+=token; break;
      case "}": level--; if (level==0) in_reset=0; outstr+=token; break;
      case "inorout": tmpstr="";
                      while (token!=";") {
                        tmpstr+=token;
                        if (token_type!="blanks") tmpstr2=token;
                        token=get_token();
                      }
                      if (tmpstr2=="1") { 
                        outstr+="add_property(\"indoors\");";
                        inorout=1;
                        break;
                      } else
                      if (tmpstr2=="2") { 
                        outstr+="add_property(\"outdoors\");";
                        inorout=2;
                        break;
                      } else {
                        /* Failed to understand inorout, lets keep it. */
                        outstr+="inorout"+tmpstr+";";
                        break;
                      }
      case "dest_dir": blanks="";
                       /* This takes a lot of eval-nodes */
                       eval_loops=eval_loops+hard_work;
                       i=instrp-9;
                       while ((i>0) && 
                         ((instr[i..i]==" ") || (instr[i..i]=="\t"))) i--;
                       if (i<(instrp-9)) blanks=instr[i+1..instrp-9];
                       tmpstr=get_token(); 
                       tmpstr2=tmpstr;
                       while (tmpstr!="=") {
                         tmpstr2+=tmpstr;
                         tmpstr=get_token();
                       }
                       tmpstr=get_mixed();
                       if (token_type!="array") {
                         write("Failed to parse dest_dir. Aborting.\n");
                         return 0;
                       }
                       create_add_exits(tmpstr, blanks);
                       /* note the s in exits, this adds all add_exits */
                       tmpstr="";
                       while (tmpstr!=";") tmpstr=get_token();
                       break;
      case "items":    blanks="";
                       /* This takes a lot of eval-nodes */
                       eval_loops=eval_loops+hard_work;
                       i=instrp-6;
                       while ((i>0) && 
                         ((instr[i..i]==" ") || (instr[i..i]=="\t"))) i--;
                       if (i<(instrp-6)) blanks=instr[i+1..instrp-6];
                       tmpstr=get_token(); 
                       tmpstr2=tmpstr;
                       while (tmpstr!="=") {
                         tmpstr2+=tmpstr;
                         tmpstr=get_token();
                       }
                       tmpstr=get_mixed();
                       if (token_type!="array") {
                         write("Failed to parse dest_dir. Aborting.\n");
                         return 0;
                       }
                       create_add_items(tmpstr, blanks);
                       tmpstr="";
                       while (tmpstr!=";") tmpstr=get_token();
                       break;
      case "add_exit": tmpstr=get_token(); 
                       tmpstr2=tmpstr;
                       while (tmpstr!="(") {
                         tmpstr2+=tmpstr;
                         tmpstr=get_token();
                       }
                       tmpstr=get_mixed();
                       tmpstr2=get_token();
                       while (tmpstr2!=",") {
                         tmpstr+=tmpstr2; tmpstr2=get_token(); 
		       }
                       tmpstr2=get_mixed();
                       tmpstr3="";
                       while ((tmpstr3!=",") && (tmpstr3!=")")) tmpstr3=get_token();
                       if (tmpstr3==",") {
                         tmpstr3=get_mixed();
			 tmpstr4="";
			 while ((tmpstr4!=",") && (tmpstr4!=")"))
			   tmpstr4=get_token();
			 if (tmpstr4==",") {
			   tmpstr4=get_mixed();
			   create_add_exit4(tmpstr2, tmpstr, tmpstr3, tmpstr4);
			 }
			 else
			   create_add_exit3(tmpstr2, tmpstr, tmpstr3);
                       }
                       else
			 create_add_exit(tmpstr2, tmpstr);
                       tmpstr="";
                       while (tmpstr!=";") tmpstr=get_token();
                       break;
      case "short_desc": tmpstr=get_token();
                         tmpstr2=tmpstr;
                         while (tmpstr!="=") {
                           tmpstr2+=tmpstr;
                           tmpstr=get_token();
                         }
                         tmpstr="";
                         tmpstr2=tmpstr;
                         i=instrp;
                         while (tmpstr!=";") {
                           tmpstr2+=tmpstr;
                           i=instrp;
                           tmpstr=get_token();
                           if (token_type=="start_string") {
                             instrp=i;
                             in_string=0;
                             tmpstr=get_string();
                           }
                           if (tmpstr=="") {
			     write("Failed to parse short_desc. Aborting.\n");
			     return 0;
			   }
                         }
                         outstr+="set_short("+remove_leading_blanks(tmpstr2)+
                                 ");";
                         break;
      case "long_desc": tmpstr=get_token(); 
                         tmpstr2=tmpstr;
                         while (tmpstr!="=") {
                           tmpstr2+=tmpstr;
                           tmpstr=get_token();
                         }
                         tmpstr="";
                         tmpstr2=tmpstr;
                         i=instrp;
                         while (tmpstr!=";") {
                           tmpstr2+=tmpstr;
                           i=instrp;
                           tmpstr=get_token();
                           if (token_type=="start_string") {
                             instrp=i;
                             in_string=0;
                             tmpstr=get_string();
                           }
                           if (tmpstr=="") {
			     write("Failed to parse long_desc. Aborting.\n");
			     return 0;
			   }
                         }
                         outstr+="set_long("+remove_leading_blanks(tmpstr2)+
                                 ");";
                         break;
      case "set_indoors":tmpstr="";
                         while (tmpstr!=";") tmpstr=get_token();
                         outstr+="add_property(\"indoors\");";
                         inorout=1;
                         break;
      case "set_outdoors":tmpstr="";
                         while (tmpstr!=";") tmpstr=get_token();
                         outstr+="add_property(\"outdoors\");";
                         inorout=2;
                         break;
      case "set_inorout": tmpstr=get_token(); 
	                 while (tmpstr!="(") {
                           tmpstr=get_token();
                         }
                         tmpstr=get_token();
	                 while (token_type=="blanks") {
                           tmpstr=get_token();
                         }
                         /* Ok, tmpstr should be 1 or 2 now */
                         if (tmpstr=="1") outstr+="add_property(\"indoors\");";
                         if (tmpstr=="2") outstr+="add_property(\"outdoors\");";
                         tmpstr="";
                         while (tmpstr!=";") tmpstr=get_token();
                         break;
      case "set_realm":
      case "realm":      tmpstr=get_token();
                         while (tmpstr!="(") tmpstr=get_token();
                         tmpstr2=get_mixed();
                         while (tmpstr!=";") tmpstr=get_token();
                         outstr+="add_property(\"realm\","+tmpstr2+");";
                         break;
      case "the_realm":  tmpstr=get_token(); 
                         tmpstr2=tmpstr;
                         while (tmpstr!="=") {
                           tmpstr2+=tmpstr;
                           tmpstr=get_token();
                         }
                         tmpstr=get_mixed();
                         if (token_type!="string") {
                           write("Failed to parse realm. Aborting.\n");
                           return 0;
                         }
                         outstr+="add_property(\"realm\","+tmpstr+")";
                         break;
	
      case "property":   blanks="";
	                 /* This might takes a bunch of eval-nodes */
	                 eval_loops=eval_loops+hard_work/3;
	                 i=instrp-9;
                         while ((i>0) && 
                           ((instr[i..i]==" ") || (instr[i..i]=="\t"))) i--;
                         if (i<(instrp-9)) blanks=instr[i+1..instrp-9];
                         tmpstr=get_token(); 
                         tmpstr2=tmpstr;
                         while (tmpstr!="=") {
                           tmpstr2+=tmpstr;
                           tmpstr=get_token();
                         }
                         tmpstr=get_mixed();
                         if (token_type=="string") {
                           create_add_property(tmpstr, blanks);
                         } 
                         else
                         if (token_type=="array") {
                           create_add_propertys(tmpstr, blanks);
                         }
                         /* note the s in propertys, this adds all 
                            add_property, lets not use real english here
                            since that only confuses my bad spelling */
                         tmpstr="";
                         while (tmpstr!=";") tmpstr=get_token();
                         break;
      case "add_item": blanks="";
	               i=instrp-9;
	               while ((i>0) && 
                         ((instr[i..i]==" ") || (instr[i..i]=="\t"))) i--;
                       if (i<(instrp-9)) blanks=instr[i+1..instrp-9];
                       tmpstr=get_token(); 
                       tmpstr2=tmpstr;
                       while (tmpstr!="(") {
                         tmpstr2+=tmpstr;
                         tmpstr=get_token();
                       }
                       tmpstr=get_mixed();
                       tmpstr2=get_token();
                       while (tmpstr2!=",") {
                         tmpstr+=tmpstr2; tmpstr2=get_token(); 
		       }
                       tmpstr2=get_mixed();
                       tmpstr3="";
                       while ((tmpstr3!=",") && (tmpstr3!=")")) tmpstr3=get_token();
                       if (tmpstr3==",") {
                         tmpstr3=get_mixed();
			 tmpstr4="";
			 while ((tmpstr4!=",") && (tmpstr4!=")"))
			   tmpstr4=get_token();
			 if (tmpstr4==",") {
			   tmpstr4=get_mixed();
			   create_add_item(tmpstr2, tmpstr, blanks);
                           outstr+="\n";
                           create_add_item_cmd(tmpstr4, tmpstr, tmpstr3, blanks);
			 }
			 else {
			   create_add_item(tmpstr2, tmpstr, blanks);
                           Verbose("Warning, add_item with 3 args encountered. Ignoring last arg.\n");
                           outstr+="/* Warning: "+tmpstr3+" was ignored. */\n";
		         }
                       }
                       else
			 create_add_item(tmpstr2, tmpstr, blanks);
                       tmpstr="";
                       while (tmpstr!=";") tmpstr=get_token();
                       break;
      /* Remove some code that is not supported in the new room and is obsolete */
      case "set_no_castle_flag":
      case "no_castle_flag": tmpstr="";
                             while (tmpstr!=";") tmpstr=get_token();
                             break;
      default: outstr+=token; break;
    } /* switch */
  }
  outstr=outstr+instr[instrp..lastchar];
  /* Ok, contents of outstr is now the fixed version of instr */
  Verbose("## END OF FILE reached.\n");
  if (instr!=outstr) no_changed++;
  if (!parsed_inherit_room) 
    Verbose("Warning, the room just parsed does not directly inherit /room/room.\N"+
            "  So manual change in some file this room inherits is required.\n");
  if (!inorout) Write("WARNING neither indoors och outdoors was defined.\n");
  if (write_2_file) {
    write_file(outfile_name,outstr);
  }
  if (debug) { /* Write results to screen, but write can not take real
                  long strings so write 4000 chars at a time */
    i=strlen(outstr);
    while (i>4000) {
      write(outstr[0..4000]);
      outstr=outstr[4001..i];
      i=strlen(outstr);
    }
    write(outstr);
  }
  Verbose("## END OF DUMPING outstr.\n");
  /* Now lets continue with the next room to parse */
  if (recursive || very_recursive)
    call_out("recurs_inner",TIME,({the_dir_global_save, ""}));
  else Write("Finished parsing.\n");
  return 1;
}

/* Check that the room is loading and inherits room/room.c and a few more 
   things. Then call parse(out_file_name); */
int check_room(string file)
{
  int res,i,imax;
  string old_str,tmp;
  string new_file; /* Filename of output file */
  string *inheritl;
  object room;
  if(!stringp(file)) {
    write("Fatal error in function check_room.\n"+
          "Argument was not a string: "+file+".\n");
    return 1;
  }
  old_str=file;
  if(!sscanf(file,"%s.c",tmp)==1) file=file+".c";
  res=file_size(file);
  if(res<0) {
    file="/"+file;
    res=file_size(file);
  }
  if(res<0) {
    file="/"+TP->query_path()+"/"+old_str;
    res=file_size(file); 
  }
  if (res<0) {
    write("File not found: "+old_str+"\n");
    return 1;
  }
  catch(room=load_object(file));
  if (room) inheritl=inherit_list(room);
  else {
    Write("Couldn't load "+file+"\n");
    if (recursive || very_recursive)
      call_out("recurs_inner",2,({the_dir_global_save, ""}));
    return 1;
  }
/*  tmp=write_inherit(inheritl,file);  ### Lets skip this. */
  if(!inheritl) {
    Write("The file : "+file+" didn't inherit /room/room.\n");
    if (recursive || very_recursive)
      call_out("recurs_inner",2,({the_dir_global_save, ""}));
    return 1;
  }
  /* Might be good to check that room/room is inherited */  
  imax=sizeof(inheritl);
  i=0;
  while ((i<imax) && (inheritl[i]!="room/room.c")) i++;
  if (i==imax) {
    Write("The file : "+file+" didn't inherit /room/room.\n");
    if (recursive || very_recursive)
      call_out("recurs_inner",2,({the_dir_global_save, ""}));
    return 1;
  }
  Debug("Checking file: "+file+"\n");
  if(write_2_file) {
    sscanf(file,"%s.c",old_str);   
    new_file=old_str+"_new.c";
    res=file_size(new_file);
    if(res>-1)
      if(force) rm(new_file);
      else	
	if (mega_force) {
          Write("Using Mega Force mode!\n");
        }
	else {
	  Write("Target file ("+new_file+") found! Aborting.\n");
	  if (recursive || very_recursive)
	    call_out("recurs_inner",2,({the_dir_global_save, ""}));
	  return 1;
	}    
    Verbose("Target file :"+new_file+"\n");
    if (mega_force) { 
      instr=read_file(file);
      mv(file,old_str+".old"); /* Might be good to take backup of the
                                  file to be changed.*/
      new_file=file; /* Write to original file, sigh */
    }
  }
  if (!mega_force) instr=read_file(file);
  instrp=0;
  in_string=0;
  in_comment=0;
  lastchar=strlen(instr);
  outfile_name=new_file;
  no_rooms++;
  parse_init(); /* Parse is working on global variable instr */
  return 1;
}

int check_room1(string file)
{
  string s1,s2;
  /* Some Statistics */
  no_rooms=0;
  no_files=0;
  no_changed=0;
  stime=0;

  show_all=0;
  write_2_file=0;
  recursive=0;
  very_recursive=0;
  silent=0;
  force=0;
  mega_force=0;
  debug=0;
/*  indent=0;*/
  stop_parsing=0;
  parsed_inherit_room=0;
  current_room=ENV(TO);
  stime=time();
  if(file&&((sscanf(file,"-%s %s",s1,s2)==2)|| (sscanf(file,"-%s",s1)==1)))
    {
      file=s2;
      if(get_flags(s1)==2) return 1;
    }
  if (parsing) {
    write("Muzgash is already working hard. Use flag -q to make him stop.\n"+
          "Aborting command.\n");
    return 1;
  }
  parsing=1;
  if(recursive||very_recursive){
    return recurs(file,({}));
  }
  if (file) { 
    check_room(file);
    parsing=0;
    return 1; 
  }
  else { 
    check_room(file_name(ENV(TP)));
    parsing=0;
    return 1;
  }
}




