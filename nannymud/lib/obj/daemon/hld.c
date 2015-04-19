/* hld.c
 *
 * Brom is hunting the cheaters...
 * A log-daemon for logging strange heals/money.
 *
 */

#define WHERE  "log/SECURE/HEAL_AND_MONEY"
#define BUG "HLD.bug"
#define TIME   300

mapping months = (["Jan":1, "Feb":2, "Mar":3, "Apr":4, "May":5, "Jun":6,
		   "Jul":7, "Aug":8, "Sep":9, "Oct":10, "Nov":11, "Dec":12 ]);
int time_stamp;

reset(arg) {
  if (arg) return;
}

void logs(object tp, object tp1, object ho, object po, int sum, int type)
{
  string datum, tmp, msg;
  object e1, e2, e3;

  if ((time() - time_stamp) > TIME)
  {
    time_stamp = time();
    tmp = ctime(time());

    /* Sigh. Security. Believe it or not, it is necessary.
     */
    if (!mappingp(months))
    {
      months = (["Jan":1, "Feb":2, "Mar":3, "Apr":4, "May":5, "Jun":6,
		 "Jul":7, "Aug":8, "Sep":9, "Oct":10, "Nov":11, "Dec":12 ]);
      log_file(BUG,
	       time() + " Had to re-initialise mapping 'months'.\n");
    }       
    if (!stringp(tmp))
    {
      tmp = "Thu Jan  1 01:00:00 1970";
      log_file(BUG,
	       time() +  " ctime() did not return string.\n");
    }

    /* Yeah, this can be done on about one line, but this
     * will give pin-point line numbers for debug...
     */
    datum =  tmp[22..23];
    datum += months[tmp[4..6]];
    datum += replace(tmp[8..9], " ", "0");
    datum =  datum + " " + tmp[11..18] + "\n";

    write_file(WHERE, datum);
    write_file(WHERE,
	     "Type, verb, TP, TP1 HO : [creator(PO)], PO, " +
	     "E(TP), E(TP1), E(PO), amount.\n");
  }

  /* this_player()'s ... */

  switch (type) {
  case 0: msg = "CONS:   "; break;
  case 1: msg = "MONEY:  "; break;
  case 2: msg = "HEAL:   "; break;
  default: msg = "????:   ";
  }

  if (tmp = query_verb())
    msg += ("Verb: " + tmp + ", ");
  else
    msg += "No verb. ";

  if (!tp) 
    msg += "No TP. ";
  else {
    msg += (tp->query_real_name() + " ");
    if (tp->query_npc()) msg += ("(NPC: " + file_name(tp) + ") ");
    if (tp->query_level() > 19) msg += "(WIZ)";
  }
  if (!tp1) 
    msg += "No TP1. ";
  else 
    if (tp1 != tp)
      msg += (tp1->query_real_name() + " ");
    else
      msg += " TP1==TP ";
  msg += " : ";
  
  /* Places etc. */

  msg += "%% ";
  if (ho) { /* The healed object is called ho. */
    if (interactive(ho))
      msg += (ho -> query_real_name() + " ");
    else
      msg += (file_name(ho) + " ");
  }
  else
    msg += ("No_called_object? ");
  
  if(po) tmp = creator(po);
  else tmp="None";

  if (tmp)
    msg += ("By " + tmp + ". ");
  else
    msg += ("By NONE. ");
  if(po) {
    if (interactive(po))
      msg += (po -> query_real_name() + " (" + po -> query_level() + ") ");
    else
      msg += (file_name(po) + " ");
  }
  else
    msg += "NO PO ";

/* I added the checks, if(tp) ...*/
/* And who are you? */
  if(tp) e1 = environment(tp);
  if(tp1) e2 = environment(tp1);
  if(po) e3 = environment(po);

  if (e1) 
    msg += (file_name(e1) + ", "); 
  else
    msg += "TP no env., ";
  if (e2)
    msg += (file_name(e1) + ", ");
  else
    msg += "TP1 no env., ";
  if (!e3)
    msg += "PO no env., ";
  else
    if (e3 == tp)
      msg += "PO==TP, ";
    else
      if (e3 == tp1)
	msg += "PO == TP1, ";
      else
	msg += (file_name(e3) + ", ");
  msg += ( sum + "\n");
  write_file(WHERE, msg);
} /* logs */

int query_prevent_shadow() { return 1; }
