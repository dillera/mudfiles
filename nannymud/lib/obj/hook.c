/* Qqqq, 95 Bugfree :)*/

static private mapping hooks = ([ ]); /* No editing this directly */

/*************************************************************************/
/* First we present the functions that the users of the hooks shall use. */
/*************************************************************************/

/*
 * add_hook:
 *           string hook_name, Is the name of the hook you want
 *                             to register to.
 * 
 *           object obj,       Optional, if sent along that will be the 
 *                             object you register instead of previous_object()
 *
 *           returns 1 if succeded otherwise 0.
 *           
 * You will register the object making the add_hook call as
 * the object being called in the function 'hook_name' 
 * when the hook is triggered.
 * In /doc/hooks/'hook_name' it tells you when the hook is triggered
 * and with which standard argument is sent along.
 */
varargs int add_hook(string hook_name,object obj) {
  if (!stringp(hook_name))
    return 0;
  if (!obj) 
    obj = previous_object();
  if (file_name(obj)[0..3] == "open" ||
      file_name(obj)[0..2] == "log") {
    log_file("HOOK_LOG","Attempt to hook to an object in open or log.\n");
    log_file("HOOK_LOG",ctime(time())+" Hook: "+hook_name+" in Object:\n"+
	     file_name(this_object())+"("+this_object()->query_real_name()+")"+
	     "\nHooked to Object: "+
	     file_name(obj)+" Call made by: "+file_name(previous_object())+"\n"+
	     "Notice ABOVE ILLEGAL ATTEMPT!\n");
    return 0;
  }
  log_file("HOOK_LOG",ctime(time())+" Hook: "+hook_name+" in Object:\n"+
	   file_name(this_object())+"("+this_object()->query_real_name()+")"+
	   "\nHooked to Object: "+
	   file_name(obj)+" Call made by: "+file_name(previous_object())+"\n");
  if (!pointerp(hooks[hook_name]))
    hooks[hook_name] = ({ obj });
  else {
    hooks[hook_name] -= ({ obj,0 });    
    hooks[hook_name] += ({ obj });    
  return 1;
  }
}

/*
 * remove_hook:
 *           string hook_name, Is the name of the hook you want
 *                             to deregister to.
 *
 *           object obj,       Optional, if sent along that will be the 
 *                             object you deregister instead of 
 *                             previous_object().
 *
 *           returns 1 if succeded otherwise 0.           
 *
 * You will deregister the object making the remove_hook call.
 */
varargs int remove_hook(string hook_name,object obj) {
  if (!stringp(hook_name))
    return 0;
  if (!pointerp(hooks[hook_name]))
    return 0;
  else {
    if (!obj)
      obj = previous_object();
    hooks[hook_name] -= ({ obj,0 });    
    if (!sizeof(hooks[hook_name]))
      hooks = m_delete(hooks,hook_name);
  }
  return 1;
}

/*
 * query_hook:
 *           string hook_name, Is the name of the hook you want
 *                             get information about.
 *            
 *           status arg,       Optional, if true and hook_name set, then it 
 *                             will return true if the caller has registered
 *                             itself as a hook in the object.
 *           
 *           object obj,       Optional, if sent along that will be the 
 *                             object you get info about instead of 
 *                             previous_object().
 *
 *           If no args is given an array with all hook names that
 *           has been set is returned.
 *           If the first arg is given then it will return an 
 *           array with all objects that has registred.
 *           If both args are given, it does what it says above.
 *
 * You can check who have registred to what hooks.
 */
varargs mixed query_hook(string hook_name,int arg,object obj) {
  if (!hook_name)
    return m_indices(hooks);
  if (arg) {
    if (!obj) 
      obj = previous_object();
    return member_array(obj,(hooks[hook_name] || ({ })))+1;
  }
  else
    return (hooks[hook_name] || ({}))+({});
}


/************************************************************************/
/* Here we present the functions the installers of the hooks shall use. */
/************************************************************************/

/*
 * hook:
 *           string hook_name,  Is the name of the hook you want
 *                              to activate.
 *            
 *           mixed standardarg, Is the first argument sent along
 *                              to the objects that has registred.
 *                             
 *           status block,      True if this is blocking hook,      
 *                              should never be given, use block_hook().
 * 
 *           If we assume block flag is never used, it returns 0,
 *           if there is no objects to call or there is a bug and otherwise it
 *           returns an array with what the users return.
 *           
 * You use this fucntion when you just want the users to be able to 
 * notice that something has happened without having any control.
 * This can also be used as a value hook that is you use
 * the array the users return in your code where the hook is called.
 */
varargs mixed hook(string hook_name,mixed standardarg,int block) {
  mixed tmp,catres;
  if (tmp = hooks[hook_name]) {
    if (!tmp[0])
      {
	tmp-=({0});
	if (!sizeof(tmp)) {
	  hooks = m_delete(hooks,hook_name);
	  return 0;
	}
	hooks[hook_name]=tmp;
      }
    if (block) {
      int max,result;
      max = sizeof(tmp);
      while(max--) {
	if (catres = catch(result = (mixed)call_other(tmp[max],hook_name,
				  standardarg,this_object()))) {
	_syslog("BUG in hooked object (check /log/HOOK_BUG), "+
		file_name(this_object())+"\n"+
		catres);
	  log_file("HOOK_BUG",catres+"\n");
	  if (objectp(tmp[max]))
	    log_file("HOOK_BUG",sprintf("BUG in Object: %O\n",
					tmp[max]));
	  else
	    log_file("HOOK_BUG",sprintf("BUG in Destructed Object!\n",));
	  log_file("HOOK_BUG",sprintf("Objects: %O\nFunction:%O\nArg:%O\nTime:%s\n\n",
				      tmp,hook_name,standardarg,ctime(time())));

	}
	if (result)
	  return 1;
      }
    }
    else {
      if (catres = catch(tmp = (mixed) call_other(tmp,hook_name,
					  standardarg,this_object()))) {
	_syslog("BUG in hooked object (check /log/HOOK_BUG), "+
		file_name(this_object())+"\n"+
		catres);
	log_file("HOOK_BUG",catres+"\n");
	if (this_object())
	  log_file("HOOK_BUG",sprintf("BUG when Object: %O\n",
				      this_object()));
	else
	  log_file("HOOK_BUG",sprintf("BUG when Destructed Object!\n",));
	log_file("HOOK_BUG",sprintf("Objects: %O\nFunction:%O\nArg:%O\nTime:%s\n\n",
				    tmp,hook_name,standardarg,ctime(time())));
	return 0;
      }
      return tmp;
    }
  }
  return 0;
}

/*
 * block_hook:
 *           string hook_name,  Is the name of the hook you want
 *                              to activate.
 *            
 *           mixed standardarg, Is the first argument sent along
 *                              to the objects that has registred.
 *                             
 *           This hook returns 1 if any of the registred objects
 *           return true, otherwise it returns 0.
 *
 * You use this function when you want he users to be able to control
 * what the event that is happening.
 */
mixed block_hook(string hook_name,mixed standardarg) {
  return hook(hook_name,standardarg,1);
}  













