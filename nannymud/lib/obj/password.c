static mapping bad_passwords=(mapping)"/obj/password"->the_bad_ones();

the_bad_ones()
{
  return bad_passwords|| (bad_passwords=(["qwerty":1,"abcdef":1,"asdfgh":1,"castle":1,"dragon":1,"enigma":1,"foobar":1,"foo-bar":1,"general":1,"grails":1,"knight":1,"lucifer":1,"martin":1,"password":1,"pass-word":1,"people":1,"shadow":1,"silver":1,"smurfl":1,"terror":1,"zxcvbn":1]));
}


query_bad_password(string str)
{
  if(str&&stringp(str))
    return bad_passwords[str];
  return 0;
}
