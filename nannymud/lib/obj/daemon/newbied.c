
int check_newbie(string name, string *arr) {
  if(name == "guest") {
    if(member_array("ban_guest",arr) == -1)
      return 1;
    else
      return 0;
  }
  if(sizeof(arr) == 1 && arr[0] == "ban_guest")
    return 0; /* Was 1. Brom 950417 */
  if(member_array(name,arr) == -1)
    return 0;
  else
    return 1;
} /* check_newbie */

int allowed_newbie(string host, string name) {
  string *file;
  string ip1,ip2,ip3,ip4;
  string ipt1,ipt2,ipt3,ipt4;
  string tmp;
  string *newb;
  int i;

  file = explode(read_file("/etc/newbie"),"\n");

  sscanf(host,"%s.%s.%s.%s",ip1,ip2,ip3,ip4);

  for(i = 0; i < sizeof(file); i++) {
    sscanf(file[i],"%s.%s.%s.%s:%s",ipt1,ipt2,ipt3,ipt4,tmp);
    newb = explode(tmp,"#");
    if(ipt1 == "*")
      return check_newbie(name, newb);
    if(ip1 != ipt1)
      continue;
    if(ipt2 == "*")
      return check_newbie(name, newb);
    if(ip2 != ipt2)
      continue;
    if(ipt3 == "*")
      return check_newbie(name, newb);
    if(ip3 != ipt3)
      continue;
    if(ipt4 == "*")
      return check_newbie(name, newb);
    if(ip4 != ipt4)
      continue;
    return check_newbie(name, newb);
  }
  return 1;
} /* allowed_newbie */
int query_prevent_shadow() { return 1; }
