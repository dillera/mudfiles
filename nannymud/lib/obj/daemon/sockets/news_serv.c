/*
 * The news-server was completely rewritten by Zell, May 28th 1992.
 *
 */

#include <config.h>
#include <rn.h>

/*
 * This mapping contains the data structure, on the form :
 * group : ({ offset, articles })
 *
 */
mapping groups;

void create() {
    int i, j;
    string *group_list;
    mixed *files;

    seteuid(ROOT_UID);
    if (!restore_object(LIST)) {
	/*
	 * First, we figure out what groups we have using get_dir.
	 *
	 */
	files = get_dir(DATA);
	files = filter_array(files, "is_directory", this_object());
	files = files - ({ "userD" });

	groups = allocate_mapping(sizeof(files));
	for (i = 0; i < sizeof(files); i ++) {
	    /*
	     * Set the data to a silly value, to force a read below.
	     *
	     */
	    groups[files[i]] = ({ 0, 0 });
	}	    
    }
    /*
     * Now to figure out which of these entries may be too old and need
     * updating, we check the reliability by accessing the files.
     */
    group_list = indices(groups);
    for (i = 0; i < sizeof(group_list); i ++) {
	string  group;
	int    *data;

	group = group_list[i];
	data  = groups[group];
	if (sizeof(get_dir(DATA + group + "/")) != data[ART] ||
	    file_size(DATA + group + "/" + (data[OFF] + 1)) < 1 ||
	    file_size(DATA + group + "/" + (data[OFF] + data[ART])) < 1) {
	    /*
	     * This test should be sufficient. It updates if the number of
	     * files are wrong, or if the first or last files don't exist.
	     */
	    files = get_dir(DATA + group + "/");
	    if (files && sizeof(files)) {
		files = map_array(files, "string_to_num", this_object());
		/*
		 * string_to_num will return zero for all files not being
		 * a number. The following array subtraction will clean it.
		 */
		files = files - ({ 0 });
		files = sort_array(files, "integer_sort", this_object());
		data[OFF] = files[0] - 1;
		/*
		 * Now, we scan the list of files until we find some sort
		 * of discontinuity - that is where we break.
		 */
		if (sizeof(files) > MAX_ARTICLES) {
		    files = files[sizeof(files) - MAX_ARTICLES .. 1000];
		}
		for (j = sizeof(files) - 1;
		     j >= 0 && files[j] == j + data[OFF] + 1;
		     j --);

		if (j >= 0) {
		    /*
		     * We did find a discontinuity - CLEAN UP!
		     */
		    filter_array(files[0 .. j], "remove_file",
				 this_object(), group);
		}
		data[ART] = sizeof(files) - j - 1;
	    }
	}
    }
    call_out("save", 600);
}

/*
 * Extra functions for create() to use in filters, maps, and sorts.
 */

status is_directory(string file) {
    return file_size(DATA + file) == -2;
}

int string_to_num(string foo) {
    int bar;
    return sscanf(foo, "%d", bar) ? bar : 0;
}

status integer_sort(int bing, int bong) {
    return bing > bong;
}

void remove_file(string file, string group) {
    rm(DATA + group + "/" + file);
}

/*
 * Functions for the client to call.
 */

string get_sub(string group, int art) {
    /*
     * From the given text, extract the subject using text-
     * handling methods. Sort of ugly.
     */
    string text, subject;
    text = read_file(DATA + group + "/" + art);
    if (!text || sscanf(text, "%s\nSubject: %s\n", text, subject) != 2) {
	/*
	 * This should cover all cases, such as "group" not being a group.
	 */
	return "";
    }
    return subject;
}

string *q_grps() { return indices(groups); }

int q_off(string group) {
    mixed *data;
    data = groups[group];
    return undefinedp(data) ? -1 : data[OFF];
}

int q_art(string group) {
    mixed *data;
    data = groups[group];
    return undefinedp(data) ? -1 : data[ART] + data[OFF];
}

status secure_poster(object ob) {
    return file_name(ob)[0 .. 6]  == "/obj/rn";
}

status post(string group, string file) {
    int    foo, *data;
    string base;

    data = groups[group];

    if (undefinedp(data) || !secure_poster(previous_object())) {
	return 0;
    }
    /*
     * First check : The directory.
     * Is there a file there? RM it. Nothing there? Make a directory.
     */
    base = DATA + group;
    foo  = file_size(base);
    if (foo > -2) {
	if (foo == -1) {
	    rm(base);
	}
	mkdir(base);
    }

    /*
     * Now, maintain the correct number of articles.
     */
    if (data[ART] >= MAX_ARTICLES) {
	rm(base + "/" + data[OFF]);
	data[OFF] ++;
    } else {
	data[ART] ++;
    }
    /*
     * Make sure there aren't articles ahead, but a gap of at least one.
     */
    rm(base + "/" + (data[OFF] + data[ART]));
    rm(base + "/" + (data[OFF] + data[ART] + 1));
    /*
     * Finally... Just copy the damn thing over.
     */
    write_file(base + "/" + (data[OFF] + data[ART]), read_file(file));
    return 1;
}

void save() { save_object(LIST); call_out("save", 600); }
int remove() { save(); destruct(this_object()); return 1; }
void clean_up() { remove(); }

