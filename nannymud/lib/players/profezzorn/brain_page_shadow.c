#include "brain.h"

object o,shell;
int t;
string text;

int start_shadow(object obj,object sh)
{
  if(this_interactive()!=obj || this_player()!=obj) return 0;
  shell=sh;
  t=time();
  text="";
  return !!shadow(o=obj,1);
}

string query_text() { return text; }

void catch_tell(string s)
{
  text+=s;
  if(!shell || t!=time())
  {
    tell_object(o,text);
    text="";
    destruct(this_object());
  }
}

