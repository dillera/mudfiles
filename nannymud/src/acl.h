#include "acllib.h"

extern char *acl_checkrights(char *path, int rights,
			     int ckparent, int is_trusted);
extern int acl_findrights(char **path, int rights,
			     int ckparent, int is_trusted);
extern char *acl_fixobjname(char *name);

