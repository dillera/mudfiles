reset(a){
  if(!a)
    set_light(1);
  if(!present("soul", this_object()))
     move_object(clone_object("/obj/soul"), this_object());
}
