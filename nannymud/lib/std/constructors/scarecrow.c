object make()
{
  object o;
  
  o=clone_object("/std/stationary");
  o->set_name("scarecrow");
  o->set_short("a scarecrow");
  o->set_long("It's made of hay and some old clothes, looking about as\n"+
	      "scary as a paper clip.\n");
  o->set_item("hay","It's slightly damp and smells bad.\n");
  o->set_item(({"clothes","old clothes"}),
	      "The old clothes are damp and full of mould. ICK!.\n");
  o->set_item("mould","ICK!\n");

  return o;
}
