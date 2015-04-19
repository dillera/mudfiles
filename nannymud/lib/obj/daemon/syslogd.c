int need_timestamp;

#define TIMESTAMP_TIME 300

void do_log(string l)
{
  string f;
  if(previous_object() == this_object() ||
     !previous_object())
    return;
  f=file_name(previous_object());
  sscanf(f,"%s#",f);
  if(sscanf(f,"obj/%*s")!=1) return;
/*  if(f!="obj/player" &&
     f!="obj/wiztool")
    return;
*/
  if(l[-1]!='\n') l+="\n";
  if(need_timestamp < time())
  {
    l=ctime(time())+"\n"+l;
    need_timestamp=time()+TIMESTAMP_TIME;
  }
  write_file("/secure/log/syslog",l);
}

query_prevent_shadow() { return 1; }
