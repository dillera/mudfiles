struct call {
    struct call *next;
    int delta;
    char *function;
    struct object *ob;
    struct svalue v;
    struct object *command_giver;
};
