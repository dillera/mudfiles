struct sentence {
    struct sentence *next;
    char *verb;
    struct object *ob;
    struct object *super;
    char *function;
    unsigned char short_verb;	/* Only leading characters count */
    unsigned char no_space;
    unsigned short hval;
};

struct sentence *alloc_sentence();

void remove_sent PROT((struct object *, struct object *)),
    free_sentence PROT((struct sentence *));
