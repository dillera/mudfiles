/* printer.c - newspaper database
 * Last changed: 930322 Anvil
 */

int totpage;
mixed *pages,*indexpage;

/* getpages - read pages for the newspaper from different locations
 * string dir - directory to read pages from
 */

static string *getpages(string dir) {
  string *new_files;
  mixed *files;
  int i,num;

  new_files = ({});
  if(file_size(dir) != -2) { /* ensure dir is directory */
    log_file("NEWSPAPER", dir+" not present.\n");
    return ({});
  }
  files = get_dir(dir);
  if (!files || files == ({})) {
    log_file("NEWSPAPER", dir+" is empty.\n");
    return ({});
  }
  if((sizeof(files)) && (!read_file(dir+files[0]))) {
    log_file("NEWSPAPER", dir+" is unreadable.\n");
    return ({});
  }
  if(member_array("INDEX",files) != -1) {
    files -= ({"INDEX"});
    indexpage += ({explode(read_file(dir+"INDEX"),"\n")[0], sizeof(files)});
  }
  else
    indexpage += ({ "The "+dir+" pages",sizeof(files) });
  for(i = 0; i < sizeof(files); i++)
    new_files = new_files + ({dir+files[i]});
  return new_files;
} /* getpages */

/* Reset - read info and compile the newspaper
 * argument ignored, occure every reset.
 */
void reset(int arg) {
  int tot,num;
  string *file;

  file = explode(read_file("/etc/newssources"),"\n");
  tot = sizeof(file);
  pages = ({});
  indexpage = ({});
  for(num = 0; num < tot; num++)
    pages += getpages(file[num]);
  totpage = sizeof(pages);
  move_object(this_object(),"room/adv_inner");
} /* reset */
 
/* update_paper - manually update the info in paper
 */
int update_paper() {
  reset(0);
  return 1;
} /* update_paper */

void init() {
  add_action("update_paper","update_paper");
} /* init */

void long(string str) {
  write("A small, magic device that prints the newspapers.\n");
} /* long */
     
string short() {
  return "A magic device";
} /* short */
     
status id(string str) {
  return str == "device" || str == "printer";
} /* id */

/* query_page - test if a certain page exists in the paper
 * int num - page to test for
 */
status query_page(int num) {
  return num > 0  && num  <= totpage;
} /* query_page  */

/* print_out - call when wishing to see a page
 * int num - the number of the page
 */
status print_out(int num) {
  this_player()->more(pages[num-1]);
  return 1;
} /* print_out */

/* index - print the index of the paper
 */
status index() {
  int num,i,tot,page_nr;

  write("\t\tThe NannyMUD Times\n");
  write("\t\t------------------\n\n");
  page_nr = 0;
  tot = sizeof(indexpage);
  for (num = 0; num < tot; num++) {
    write(indexpage[num++]);
    for(i = strlen(indexpage[num-1]); i < 50 ; i++)
      write(".");
    if (!indexpage[num])
      write("None\n");
    else
      write( (page_nr +1) + "-"+(page_nr += indexpage[num])+"\n");
  } /* for */
  return 1;
} /* index */

