int array;

reset(arg) {
  if (arg)
    return;
  remove_call_out("reclaim_objects");
  call_out("reclaim_object",3600+random(3600));
}

reclaim_object() {
  int tmp,i;
  remove_call_out("reclaim_objects");
  call_out("reclaim_object",3600+random(3600));
  if (catch(array = _get_all_obj())) {
    log_file("DAEMONBUG","To many objects in the game and to small array limit.\n");
    return;
  }
  log_file("RECLAIMED", ctime(time()) + ": Trying to reclaim...\n");
  tmp = sizeof(array)/10;
  for(i=0;i<10;i++) {
    call_out("do_actual_reclaim",(i+1)*2,i*tmp);
  }
}

do_actual_reclaim(startindex) {
  int i,slutindex,res;
  string str;
  object env;

  str = "";
  slutindex = startindex+sizeof(array)/10;
  for(i=startindex;i<slutindex;i++)
    if (objectp(array[i]) && !living(array[i]) &&
	!catch(res = array[i]->query_property("unique_item")) && res) {
      str += file_name(array[i])+": Had uniq property and was not living.\n";
      if ((env=environment(array[i])) && !living(env) &&
	  !transfer(array[i], "room/void")) {
	str += file_name(array[i])+" "+file_name(env)+"\n";
	catch(destruct(array[i]));
      }
    }
  log_file("RECLAIMED",str);
}

