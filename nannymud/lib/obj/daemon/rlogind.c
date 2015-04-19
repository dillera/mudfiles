/* Testing rlogin 921017 Anvil */

query_rlogin(user,ip) {
  string rl;
  string ips,name,a,b;
  string ip1,ip2,ip3,ip4;
  string ipt1,ipt2,ipt3,ipt4;
  string tmp;
  int i,j;

  rl = read_file("/etc/rlogins");

  rl = explode(rl,"\n");
  for(j = 0; j < sizeof(rl); j++) {
    if(sscanf(rl[j],user+":%s:%s",ips,name) == 2) {
      sscanf(ip,"%s.%s.%s.%s",ip1,ip2,ip3,ip4);
      tmp = explode(ips,",");
      for(i = 0; i < sizeof(tmp); i++) {
	sscanf(tmp[i],"%s.%s.%s.%s",ipt1,ipt2,ipt3,ipt4);
	if(ipt1 ==  "*")
	  return name;
	if(ipt1 != ip1)
	  continue;
	if(ipt2 ==  "*")
	  return name;
	if(ipt2 != ip2)
	  continue;
	if(ipt3 ==  "*")
	  return name;
	if(ipt3 != ip3)
	  continue;
	if(ipt4 ==  "*")
	  return name;
	if(ipt4 != ip4)
	  continue;
	return name;
      }
    }
  }
  return 0;
}
