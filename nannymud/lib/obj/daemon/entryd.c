string st;

enter()
{
  string tmp;
  if(!st)
    st=replace(read_file("etc/LOGO2"), "$$$$$$$$$$$$$$$$",
                sprintf("%- 16s", version()));

  tmp=replace(st, "###",                
	      sprintf("%3d",sizeof(users())));
  write(tmp); 
  return 1;
}

void reset(int arg) {
  if(_isclone(this_object())) 
    return destruct(this_object());
  move_object(this_object(),"/obj/daemon/daemonroom");
}

string short() { return "The entry-mess daemon"; }

int id(string s) { return s=="entry" || s=="entry daemon" || s==short(); }

void long() 
{
  write("This is a very pointless litle daemon.\n");
}

int query_prevent_shadow() { return 1; }
