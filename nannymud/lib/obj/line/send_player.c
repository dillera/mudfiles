object o, div_guildmark;
string linename, general_linename, to_linename;
int co=71;
int online=1;
string REAL_NAME, CAP_NAME;

reset(var) 
{ 
  if(var) return;
  enable_commands();
  return; 
}

selfdestruct() { call_out("selfdestruct2",2); }
selfdestruct2() { if(this_object()) destruct(this_object()); }

init_statue(who,lname,*div)
{
object s;
int linename_length;
  linename=lname;
  linename_length=strlen(linename);
  o=who;
  s=present("Noedid",o);
  if(s) co=s->flagval("columns") -linename_length -1;
  REAL_NAME=who->query_real_name();
  CAP_NAME=capitalize(REAL_NAME);
  if (div && mappingp(div)) {
    general_linename=div["general"];
    to_linename=div["to"];
    div_guildmark=div["guildmark"];
  }
}

catch_tell(s) {
  if(!o) { selfdestruct(); return; }
  if (!online) return 1;
  if (general_linename && (s[0..0]=="#"))
    s=sprintf(general_linename+"%-=*s",co,s[1..strlen(s)-1]);  /* Real fast linebreaking */
  else
    s=sprintf(linename+"%-=*s",co,s);  /* Real fast linebreaking */
  tell_object(o,s);
}

catch_tell_line_to(s,pass_if_offline) {
  if(!o) { selfdestruct(); return; }
  if (!online && !pass_if_offline) return 1;
  if (to_linename)
    s=sprintf(to_linename+"%-=*s",co,s);  /* Real fast linebreaking */
  else
    s=sprintf(linename+"%-=*s",co,s);  /* Real fast linebreaking */
  tell_object(o,s);
}

query_npc()
{
  if(o) return interactive(o);
  selfdestruct();
}
query_living() { return query_npc(); }

go_online() { online=1; }

go_offline() { online=0; }

query_online() { return online; }

id(name) {
  if (o) return ((name==REAL_NAME) || (name==CAP_NAME));
  selfdestruct();
}

query_who() { return o; }

query_guildmark() { return div_guildmark; }

#define DUM(X) X(x) { if(o) return o->X(x); selfdestruct(); }
DUM(short)
DUM(query_name)
DUM(query_real_name)
DUM(query_title)
DUM(query_possessive)
DUM(query_pronoun)
DUM(query_objective)
DUM(query_long)
DUM(query_money)
DUM(query_level)









