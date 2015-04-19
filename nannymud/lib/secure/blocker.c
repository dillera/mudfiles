
object shad;

reset(arg) {
  string skrap;

  if (arg) return;
  if (sscanf(file_name(this_object()),"%s#%s",skrap,skrap) == 2) {
    shad = clone_object("local/q");
    shad->q_activate();
}
}

_exit() {
  if (objectp(shad))
    destruct(shad);
}

init() {
  if (this_player()->query_level() < 20) return;
  write("You now got the commands: Add, Sub, AddN, SubN, Blocked, Set, Load and Save.\n");
  add_action("add","Add");
  add_action("sub","Sub");
  add_action("addn","AddN");
  add_action("subn","SubN");
  add_action("show","Blocked");
  add_action("set","Set");
  add_action("ladda","Load");
  add_action("save","Save");
}

add(str) {
shad->q_add_approve(str);
write("Done.\n");
return 1;
}

subn(str) {
shad->q_rem_not_approve(str);
write("Done.\n");
return 1;
}

addn(str) {
shad->q_add_not_approve(str);
write("Done.\n");
return 1;
}

sub(str) {
shad->q_rem_approve(str);
write("Done.\n");
return 1;
}

show(str) {
string array;
int i,max;
if (str) {
  write("Blocked.\n--------\n");
  array = shad->q_query_not_approve();
}
else
  {
  write("Allowed.\n--------\n");
  array = shad->q_query_approve();
}
max = sizeof(array);
for(i=0;i<max;i++) 
  write(array[i]+"\n");
write("Done.\n");
return 1;
}

set(str) {
  int x,y,q,w;
  if (!str) {
    write("Trace: "+shad->q_query_array()[0]+", Block: "+shad->q_query_array()[1]+", Bag: "+shad->q_query_array()[2]+
	  ", Block Unknown: "+
	  shad->q_query_array()[3]+
	  ", Level: "+
	  shad->q_query_level_block()+"\n");
    return 1;
  }
  if (sscanf(str,"%d %d %d %d",x,y,q,w) != 4) 
    if (sscanf(str,"%d",x)) 
      {
	shad->q_set_level_block(x);
	return write("Block level set to: "+x+".\n"),1;
      }
    else
      return write("Wrong format: <1/0> <2/1/0> <2/1/0> <1/0>\n"+"For trace, block, baging, block unknown or just <level> to set block level.\n"),1;
  shad->q_set_array( ({x , y , q, w}));
  write("Done.\n");
  return 1;
}

save() {
shad->q_save();
write("Done.\n");
return 1;
}

ladda() {
shad->q_load();
write("Done.\n");
return 1;
}

get() { return 1; }

drop() { return 1; }
       
query_auto_load() {
  return "/secure/blocker:load"; }

init_arg() {
  shad->q_load();
}





