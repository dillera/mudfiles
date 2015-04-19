inherit "std/basic_thing";

reset(arg)
{
  if(arg) return;
  set_name("word");
  set_short("The staff of Word");
  set_long("With this staff you are able to speak.\n"+
"Use 'give word to <player>' if you want to give it to someone.\n");
  add_alias("has_the_word");
  add_alias("staff");
  add_alias("staff of word");
  set_value(0);
  set_weight(0);
}

