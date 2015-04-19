#define TP this_player()

#define TP_RN (string)TP->query_real_name()

#define CHECK_DEFAULTS ([ "/log/"+TP_RN:({"$D",-2}),"/log/"+TP_RN+".rep":({"$C",0}) ])

#define GET_CHECK (mixed)TP->getenv("CHECK")

#define OUT(str) write("The Check File Daemon tells you: "+str+"\n")
#define DEBUG 0

inherit "/obj/simplemore";
/* #define SHOW(str) simplemore(str);flush() */
#define SHOW(str) write(str);

static int res = 0;

void set_env(mapping files)
{
   switch(TP->setenv("CHECK",files)) {
    case 1: if(DEBUG) OUT("Changed the Check variable."); break;
    case 0: if(DEBUG) OUT("Setting Check defaults."); break;
    case -1: OUT("An error has occured during the setting of the environment variable CHECK.");break;
   }
}


void reset_checker()
{
  if(res) return;
  if(!_isclone(this_object())) return;
  if(!mappingp(TP->getenv("CHECK")))
   set_env(CHECK_DEFAULTS);  
  if(_isclone(this_object()))  call_out("check_files",2);
  res=1;
}

string unpack_text(string text,string file)
{
  string st;
  int cols;
  cols=TP->query_cols();
  text=replace(text,"$N",TP_RN);
  text=replace(text,"$F",file);
  st=sprintf("%|"+cols+"s","---> Found "+file+" <---");
  text=replace(text,"$D",st);
  st=sprintf("%|"+cols+"s","---> Changed "+file+" <---");
  text=replace(text,"$C",st);
  return text;
}

void out_line(string text,string file)
{
  text=unpack_text(text,file);
  write("\n"+text+"\n");
}

int query_safe(string file)
{     
  if(file=="/log/"+TP_RN) return 1;
  if(file=="/log/"+TP_RN+".rep") return 1;
  return 0;
}


int list()
{
  mapping check;
  mixed ind,val;
  int i;
  check = (mapping) TP->getenv("CHECK");
  ind=m_indices(check);
  val=m_values(check);
  write("These are your Watches:\n");
  for(i=0;i<sizeof(ind);i++) 
    {      
      write("File: "+ind[i]+" ");
      if(query_safe(ind[i])) write("(Safe) ");
      write("Type: ");
      switch(val[i][1])
	{
	case -2: write("Exist");break;
	case -1: write("Tail");break;
	default: write("Changed");break;
	}
      write("\n");
      write("Text:\n"+unpack_text(val[i][0],ind[i])+"\n");
    }
  return 1;
}


int _watch()
{
 mapping check,ch;
 mixed values,indices;
 int i,n,size,tmp;
 string str;
 check=(mapping)TP->getenv("CHECK");
 values=m_values(check);
 indices=m_indices(check);
 for(i=0;i<sizeof(indices);i++) 
   {
     size=file_size(indices[i]);
     switch(values[i][1]) 
       {
       case -2: if(size>-1) out_line(values[i][0],indices[i]);break;	
       case -1: if(size>-1) out_line(values[i][0],indices[i]); 
	        tail(indices[i]); break;	
       default:
	 if(size>-1&&(size!=values[i][1]))
	   {
	     if(size!=0)
	       {
		 out_line(values[i][0],indices[i]);
		 str="";
		 if(values[i][1]>size)
		   {
		     tmp=size-1080;
		     if(tmp<0) tmp=0;
		     str+=read_bytes(indices[i],tmp,size);		 
		   }
		 else
		   str+=read_bytes(indices[i],values[i][1],size);
		 SHOW(str);
		 ch=m_delete(check,indices[i]);
		 ch[indices[i]]=({values[i][0],size});
		 set_env(ch);
	       }
	     else 
	       if(size<0&&values[i][1]>0) 
		 {
		   ch=m_delete(check,indices[i]);
		   ch[indices[i]]=({values[i][0],0});
		   set_env(ch);
		 }
	   }
	 break;
       }
   }
 return 1;
}


mixed query_exist(string file)
{
  mapping map;
  map= (mapping) TP->getenv("CHECK");
  return map[file];
}

int help()
{  
  write("Usage: "+query_verb()+" <file> [<message> <size>]\n"+
	"The <message> will be displayed each time the watched file has changed.\n"+
	"<size> -2 means that ONLY the message is show.\n"+
	"<size> -1 means that the message is show AND that the files is tailed.\n"+
	"Another <size> means that the message is shown plus that the file is showed.\n"+
	"This means that the diffrence between the last size and the current is shown.\n");
  return 1;
}


int Watch(string str)
{
  mapping check;
  string file,text;
  int size;
  if(!str) return _watch();

  if(sscanf(str,"-%s",text))
    {
      switch(text)
	{
	case "list": case "l": return list();
	case "help":case "h": return help();
	default: write("Unkown command: "+text+"\n");
	         return 1;
	}
    }	
  
  size=-2;
  if(sscanf(str,"%s %s %d",file,text,size)!=3)
    if(sscanf(str,"%s %d",file,size)!=2)
     if(sscanf(str,"%s %s",file,text)!=2)
      if(sscanf(str,"%s",file)!=1)
	{
	  write("Usage: "+query_verb()+" <file> [<message> <size>]\n");
	  return 1;
	}

  if(!text)
    if(size<0) text="$D";
    else text="$C";
  
  check= (mapping) TP->getenv("CHECK");
  
  if(query_exist(file))  {  
    check=m_delete(check,file);
    write("Changed Watch for "+file+"\n");
  }
  else
  write("Added Watch for "+file+"\n");  
  check[file]=({text,size});
  set_env(check);

  return 1;
}

void print_array(mixed arr)
{
  int i,n;
  if(pointerp(arr)) {
    write("({");
    for(n=0;n<(sizeof(arr)-1);n++) write(arr[n]+", ");
    write(arr[sizeof(arr)-1]);
    write("})");
  }
  else
   write(arr);
}


int RmWatch(string file)
{
  mapping check;
  switch(file)
    {
    case "help": case "-help": case "-h":
      write("Usage "+query_verb()+" <filename>\n"+
	    "Removes the watch for the file with the specified filename.\n");
      return 1;
     default: 
      if(query_exist(file)) 
	{
	  if(!query_safe(file)) 
	    {
	      check = (mapping) TP->getenv("CHECK");
	      check = m_delete(check,file);
	      set_env(check);
	      write("Ok!\n");
	}
	  else
	    write("The file ("+file+") is a Safe file, Not Removed.\n");
	}
      else 
	write("You are not Watching that file.\n");
      return 1;
    }
}


int check_files()
{
  while(find_call_out("check_files") > -1)
    remove_call_out("check_files");
  _watch();
}

void init_checker()
{
  add_action("Watch","Watch");
  add_action("RmWatch","RmWatch");
}

void long(){ write("Commands: Watch and RmWatch.\n");}
void reset(int arg){if(arg) return; reset_checker();}
void init(){init_checker();}
int id(string str){return str=="checker";}
int get(){return 1;}
int drop(){return 1;}
string short(){return "Checker";}
string query_auto_load(){ return "secure/check_files:"; }



