/*
** acl.h
**
** Copyright (c) 1993 Peter Eriksson <pen@lysator.liu.se>
*/

#ifndef __ACL_H__
#define __ACL_H__

struct acl_entry
{
    char          *object;
    unsigned short rights;
};

typedef struct
{
    int refcnt;
    unsigned short rest_rights;
    int n_entries;
    int s_entries;
    struct acl_entry entries[16];
} ACL;

typedef struct
{
    time_t dtm;
    ACL *acl;
} ACL_Cache;

#define ACL_ALL  0xFFFF
#define ACL_NONE 0x0000

#define ACL_P    0x0001
#define ACL_D    0x0002
#define ACL_A    0x0004
#define ACL_L    0x0008
#define ACL_U    0x0010
#define ACL_R    0x0020
#define ACL_W    0x0040
#define ACL_X    0x0080
#define ACL_T    0x0100

extern void acl_init(void);
extern ACL *_acl_read(char *path);
extern ACL *acl_read(char *path);
extern ACL *acl_get(char *path,char **opath);
extern int acl_write(ACL *acl, char *path);
extern void acl_print(ACL *acl, char *path, char *aclpath);
extern int acl_edit(ACL *acl,char *object, int rights);
extern int acl_getrights(ACL *acl, char *object);

extern int acl_str2rights(char *rights);
extern char *acl_rights2str(int rights);

extern ACL *acl_alloc(int size);
extern ACL *acl_grow(ACL *acl, int size);
extern void acl_free(ACL *acl);

#endif
