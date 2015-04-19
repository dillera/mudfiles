string lines,bib;
int money,num_lines;

set_dir(dir) {
  bib = dir + "/";
  lines = "";
  num_lines = 0;
  money = 0;
  write("\nThe lines cost 10 coins each.\n");
  write("Maximum number of lines is 17.\n");
  write("No lines may have more than 80 letters.\n");
  write("Terminate with ** or abort with ~q\n");
  input_to("readline");
  write("1>");
  return 1;
}

readline(str) {
  if (str == "**") return skriv_ut();
  if (num_lines == 17 || str == "~q") return abort();
  if (str && strlen(str) > 80) {
    write("------------Line Too Long (Max 80)----------------\n");
    input_to("readline");
    return write((num_lines+1)+">");
  }
  lines = lines + str + "\n";
  num_lines++;
  if (num_lines == 17)
    write("You must type '**' or this will be aborted, that was the last line.\n");
  else
    money += 10;
  input_to("readline");
  return write((num_lines+1)+">");
}

abort() {
  write("As you wish\n");
  destruct(this_object());
}

fix_delimiter(name) {
  string str;
  int i;

  str = "--- The above written by: "+capitalize(name)+" ";
  for(i = 0; i < (80 - strlen(name) - 30);  i++)
    str += "-";
  return str+"\n";
}

skriv_ut() {
  string files;

  if (this_player()->query_money() < money) {
    write("Aborted, you need more money.\n");
    return destruct(this_object());
  }
  write("You notice that it costs "+money+" coins.\n");
  this_player()->add_money(-money);
  write("You put the money in some magic slot that appears,\nthe slot also takes your piece of paper.\n");
  files = get_dir(bib);
  if (files == ({}) || !files || sizeof(files) == 1)
    write_file(bib+"page1",lines+fix_delimiter(this_player()->query_real_name()));
  else
    if (sizeof(explode(read_file(bib+files[sizeof(files)-1]),"\n"))+num_lines < 17)
      write_file(bib+files[sizeof(files)-1],lines+fix_delimiter(this_player()->query_real_name()));
    else
      write_file(bib+"page"+sizeof(files),lines+fix_delimiter(this_player()->query_real_name()));
  write("It will be added to the paper in our next issue.\n");
  destruct(this_object());
  return 1;
}


