/* Written by Profezzorn */

#include </std/rfc/good_monster.h>
inherit "/std/rfc/simple_monster";

/** Variables **/
mapping responses;
mapping parses;


void catch_tell(string input)
{
  if(responses &&
     previous_object()!=this_object() &&
     this_player()!=this_object())
  {
    object tp;
    string name,a,b,c,word;
    int e;
    string *tmp;
    int matches;
    string m,data;


    if(tp=this_player()) 
      name=this_player()->query_real_name();

    enable_commands();

    if(sscanf(input,"%s %s %s\n",a,b,c))
    {
      if(lower_case(a)==name)
      {
	word=b;
      }else{
	word=a;
      }
    }else{
      word="*";
    }

    if(parses)
    {
      if(tmp=parses[word])
      {
	for(e=0;e<sizeof(tmp);e++)
	{
	  sscanf(tmp[e],"%d\b%s\b%s",matches,m,data);
	  if(matches?
	     (sscanf(input,m) == matches):
	     (m==input))
	  {
	    word=nword;
	    break;
	  }
	}
      }
    }

    if(tmp=responses[state][word])
    {
      for(e=0;e<sizeof(tmp);e++)
      {
	sscanf(tmp[e],"%d\b%s\b%s",matches,m,data);
	if(matches?
	   (sscanf(input,m) == matches):
	   (m==input))
	{
	  word=nword;
	  break;
	}
      }
    }
  }
}

void reset(int arg)
{
  ::reset(arg);
  if(!arg) move_object(clone_object("/obj/soul"),this_object());
}
