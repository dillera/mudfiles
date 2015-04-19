/* 
 *        The Shadow for Champions of Khorne.
 */

object player,guildobj;

start_shadow(object who)
{
  guildobj=previous_object();
  player=who;
  return shadow(who,1);
}

attack()
{
  if(previous_object()==player && !query_verb())
  {
    guildobj->do_attacks();
  }else{
    return player->attack();
  }
}


query_spell_points(){
 return 0;
}

query_real_spell_points(){
 return player->query_spell_points();
}

remove_ghost(){
 if(player->query_real_ghost()) {
  player->remove_ghost();
  call_other("/guilds/khorne/rooms/guild_room","leave_final",this_player());
  return 1;
 }
 write_file("/guilds/khorne/log/fake_deaths",player->query_real_name()+
 " Time: "+ctime(time())+"\n");
 write("[Khorne] You are saved by the Blood God and will not lose anything.\n");
 player->remove_ghost();
 return 1;
}

heal_self(int i)
{
  if(previous_object()->is_weapon()||previous_object()->is_armour()||previous_object()->query_property("ALLOW_KHORNE_HEAL")||i<0)
    player->heal_self(i);
}

reduce_hit_point(int i)
{
  if(previous_object()->is_weapon()||previous_object()->is_armour()||previous_object()->query_property("ALLOW_KHORNE_HEAL")||i>0)
    player->reduce_hit_point(i);
}

restore_spell_points(int i)
{
  if(previous_object()->is_weapon()||previous_object()->is_armour()||previous_object()->query_property("ALLOW_KHORNE_HEAL")||i<0)
  player->restore_spell_points(i);
}


drink_alcohol(strength) { return 0;}
drink_alco(strength) { return 0;}
drink_soft(strength) { return 0;}


