#define QUEST_ROOM "/room/quest_room"
#define OUT_FILE "/www/quest-list.html"
#define OUT_DIR "/www/quests/"

#define PRE  "<insert file=/nanny/stddef><insert name=body>\n<title>NannyMUD - The Quests.</title><center><h1>NannyMUD - The Quests.</h1></center><ul>\n<insert file=/nanny/stddef><insert name=body>\n\n"
#define POST "</ul><p><i>This listing is automagically composed by the admins quest daemon.</i><insert file=/nanny/enddef>"

#define ADDR "http://www.lysator.liu.se/nanny/"

#define OWN_PAGE ([])

int get_cost(object o) { return (int)o->query_points(); }

/*
string get_quests()
{
  mixed *o;
  mapping q;
  string res,ql;
  int i,qp;
  res=ql="";
  o="room/quest_room"->query_quest_data();

  for(i=0;i<sizeof(o[0]);i++)
  {
    qp  += o[1][i];
    res += "<dt><li><b>"+o[2][i]+ "\n</b><dd>\n"+"<pre>"+o[4][i]+"</pre>\n";
    ql  += o[0][i]+"\n";
  }
  rm("/open/quest_list");
  write_file("open/quest_list",ql);
  return res+"<p>"+"<h2>Total number of quests: "+(i-1)+"</h2><h2>Total qp : "+qp+"</h2>";
}
*/

string get_quests()
{
  mixed *o;
  mapping q;
  string res,ql;
  string qname,s1,n1,n2,name,res2,qpage;
  int i,qp;
  res=ql="";
  o="room/quest_room"->query_quest_data();

  for(i=0;i<sizeof(o[0]);i++)
  {
    qp  += o[1][i];
    o[0][i]=replace(o[0][i]," ","_");
    res2="";
    if(sscanf(o[2][i],"%s(%s by %s and %s)",qname,s1,n1,n2)==4)
      {
	if(OWN_PAGE[o[0][i]]) 
	  {
	    res2+="<a href=\""+OWN_PAGE[o[0][i]]+"\">"+qname+"</a>";
	  }
	else {
	  res2+="<a href=\""+ADDR+"www-texter/quests/";
	  res2+=o[0][i]+".html\">"+qname+"</a>";
	}
	res2+="("+s1+" by ";
	res2+="<a href=\""+ADDR+"ulpc/userinfo.ulpc?who=";
	res2+=capitalize(n1)+"\">"+n1+"</a>";
	if(n2)
	  {
	    res2+=" and ";
	    res2+="<a href=\""+ADDR+"ulpc/userinfo.ulpc?who=";
	    res2+=capitalize(n2)+"\">"+n2+"</a>";
	  }
	res2+=")\n";
      }
    else
      if(sscanf(o[2][i],"%s(%s by %s)",qname,s1,n1)==3)	{
	if(OWN_PAGE[o[0][i]]) 
	  {
	    res2+="<a href=\""+OWN_PAGE[o[0][i]]+"\">"+qname+"</a>";
	  }
	else {
	  res2+="<a href=\""+ADDR+"www-texter/quests/";
	  res2+=o[0][i]+".html\">"+qname+"</a>";
	}
	res2+="("+s1+" by ";
	res2+="<a href=\""+ADDR+"ulpc/userinfo.ulpc?who=";
	res2+=capitalize(n1)+"\">"+n1+"</a>"+")\n";
      }
      else
	if(sscanf(o[2][i],"%s(%s)",qname,s1)==2) {	     	  
	  if(OWN_PAGE[o[0][i]]) 
	    {
	      res2+="<a href=\""+OWN_PAGE[o[0][i]]+"\">"+qname+"</a>";
	    }
	  else {
	    res2+="<a href=\""+ADDR+"www-texter/quests/";
	    res2+=o[0][i]+".html\">"+qname+"</a>";
	  }
	  res2+="("+s1+")\n";
	}
    ql  += o[0][i]+"\n";
    res+="<dt><li>\n"+res2+"\n";
    qpage="<title>"+qname+"</title>\n";
    qpage+=res2+"\n";
    qpage+="<pre>"+o[4][i]+"</pre>\n";
    rm(OUT_DIR+o[0][i]+".html");
    write_file(OUT_DIR+o[0][i]+".html",qpage);
    "/obj/master"->do_chmod(OUT_DIR+o[0][i]+".html");
  }
  rm("/open/quest_list");
  write_file("open/quest_list",ql);

  return res+"<p>"+"<h2>Total number of quests: "+(i-1)+"</h2><h2>Total qp : "+qp+"</h2>";
}

int c;

void reset(int arg)
{
  if(!(c%3))
  {
    rm(OUT_FILE);
    write_file(OUT_FILE,PRE);
    write_file(OUT_FILE,get_quests());
    write_file(OUT_FILE,POST);
    "/obj/master"->do_chmod(OUT_FILE);
  }
}
