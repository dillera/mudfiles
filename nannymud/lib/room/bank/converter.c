string cardlist;
mapping cardmapping;


convert()
{
  int    i;
  string oldlist, newlist, stored;
  object card;
  int cardvalue, cardtype;
  cardmapping=([]);
  cardvalue = 0; cardtype = 0;
  restore_object("players/anna/bank/save.old");
  if (cardlist) oldlist = explode(cardlist, ",");
  if (!oldlist) return -1;
  write(sizeof(oldlist)+"::\n");
  for (i = 0; i < sizeof(oldlist); i++) {
    stored = explode(oldlist[i], ";");
    sscanf(stored[1], "%d", cardvalue);
    sscanf(stored[2], "%d", cardtype);
    cardmapping[stored[0]]=({cardtype,cardvalue});
    /*    write(stored[0]+" "+cardtype+" "+cardvalue+"\n");*/
  }
  cardlist=0;
  save_object("players/anna/bank/save");
}
