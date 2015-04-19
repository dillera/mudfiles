/*
** acl_efuns.c			ACL handling EFUNs
**
** Copyright (c) 1993 Peter Eriksson <pen@lysator.liu.se>
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "memory.h"
#include "debug.h"
#include "lint.h"
#include "lang.h"
#include "exec.h"
#include "interpret.h"
#include "config.h"
#include "object.h"
#include "instrs.h"
#include "comm.h"

#include "acl.h"
#include "acl_efuns.h"

extern struct svalue *sapply(char *, struct object *, int);

extern struct object *master_ob;

extern char real_mudlibpath[];
extern char *strip_fullpath(char *str);
extern char *strip_abspath(char *str);

extern int acl_debug;


/*
** Convert a LPC vector representation of an ACL into a C-code
** ACL struct object.
*/
static ACL *vector_to_acl(struct vector *v)
{
    ACL *acl;
    int i;
    struct vector *vv;
    
    
    if (v == NULL || v->size != 3)
	return NULL;

    if (v->item[1].type != T_NUMBER)
	return NULL;
    
    if (v->item[2].type != T_POINTER)
	return NULL;
    
    vv = v->item[2].u.vec;
    
    acl = acl_alloc(vv->size);
    
    if (acl == NULL)
	return NULL;
    
    acl->rest_rights = v->item[1].u.number;
    
    for (i = 0; i < vv->size; i++)
    {
	struct vector *av;
	
	if (vv->item[i].type != T_POINTER)
	    goto Error;
	
	av = vv->item[i].u.vec;
	
	if (av->item[0].type != T_STRING)
	    goto Error;
	
	if (av->item[1].type != T_NUMBER)
	    goto Error;
	
	acl->entries[i].object = strdup(av->item[0].u.string);
	acl->entries[i].rights = av->item[1].u.number;
    }
    acl->n_entries = i;
    return acl;
    
  Error:
    acl->n_entries = i;
    acl_free(acl);
    return NULL;
}


/*
** Convert a C-code ACL struct object into an LPC vector
** representation useful for LPC code.
*/
static struct vector *acl_to_vector(ACL *acl, char *path)
{
    struct vector *v, *vv;
    int i;


    v = allocate_array(3);

    if (path)
    {
	v->item[0].type = T_STRING;
	v->item[0].string_type = STRING_MALLOC;
	v->item[0].u.string = string_copy(strip_fullpath(path));
    }
    else
    {
	v->item[0].type = T_NUMBER;
	v->item[0].u.number = 0;
    }

    v->item[1].type = T_NUMBER;
    v->item[1].u.number = acl->rest_rights;

    vv = allocate_array(acl->n_entries);

    for (i = 0; i < acl->n_entries; i++)
    {
	struct vector *am;

	am = allocate_array(2);
	
	am->item[0].type = T_STRING;
	am->item[0].string_type = STRING_MALLOC;
	am->item[0].u.string = string_copy(acl->entries[i].object);

	am->item[1].type = T_NUMBER;
	am->item[1].u.number = acl->entries[i].rights;
	
	vv->item[i].type = T_POINTER;
	vv->item[i].u.vec = am;
    }

    v->item[2].type = T_POINTER;
    v->item[2].u.vec = vv;

    return v;
}



/*
** Set ACL protection on a filesystem object.
**
** Permission is granted if caller have P rights on object or
** containing directory.
*/
void efun_acl_put(struct svalue *sp)
{
    char *path;

    int res  = 0;
    ACL *acl = NULL;
    
    
    /* We must be called from an existing object.. */
    if (current_object->flags & O_DESTRUCTED)
	goto End;

    /* Get new ACL */
    if ((sp-1)->type == T_POINTER)
    {
	acl = vector_to_acl((sp-1)->u.vec);
	if (acl == NULL)
	    goto End;
    }
    else
	acl = NULL;

    /* Get target pathname */
    path = sp->u.string;
    
    if (path != NULL && *path != '\0' &&
	(path = acl_checkrights(path, ACL_P, 1, 0)))
	res = acl_write(acl, path) < 0 ? 0 : 1;
	
  End:
    if (acl)
	acl_free(acl);
    
    pop_n_elems(2);
    push_number(res);
}



/*
** Get ACL protecting a filesystem object.
**
** Permission is granted if caller have L rights to object or
** parent directory.
*/
void efun_acl_get(struct svalue *sp)
{
    /*
     * Args: string path - filesys object to get ACL info for
     *                     if 0, get priority ACL.
     *
     * Returns: compound object:
     *
     *      [0] = 0 or string path where ACL was found
     *      [1] = $REST rights
     *      [2] = Array of ({ "object" : right }) pairs
     */
    char *path;

    ACL *acl = NULL;
    char *apath = NULL;

    
    path = sp->u.string;
    
    if (path == NULL || !*path)
	acl = acl_get(NULL, &apath); /* Get Priority ACL */
    else
	if (path = acl_checkrights(sp->u.string, ACL_L, 1, 0))
	    acl = acl_get(path, &apath);

    pop_n_elems(1);
    
    if (acl)
    {
	push_vector(acl_to_vector(acl, apath));
	sp->u.vec->ref--; /* Was set to 1 at allocation */

	acl_free(acl);
    }
    else
	push_number(0);
}


void efun_acl_modify(struct svalue *sp)
{
    /* Args: string path - filesys object to modify ACL for
     *       string object - wizard/LPC object to modify rights for
     *	     string rights - new rights to give object
     *
     * If object & rights is 0 -> Remove specific ACL for filesys obj
     *
     * Returns: 1 if successful, else 0
     */
    pop_n_elems(3);
    push_number(0);
}



/*
** Return a string containing the expanded pathname for a
** certain filesystem object target if the calling wizard/object
** have the rights specified.
*/
void efun_acl_access(struct svalue *sp, int num_arg)
{
    char *inpath;
    char *outpath;
    int rights;
    int ckparent;

/* Urk
    inpath = (sp-2)->u.string;
    rights = (sp-1)->u.number;
 */
    inpath = (sp - num_arg + 1)->u.string;
    rights = (sp - num_arg + 2)->u.number;

    if (num_arg > 2)
	ckparent = sp->u.number;
    else
	ckparent = 0;

    outpath = acl_checkrights(inpath, rights, ckparent, 0);
	
    pop_n_elems(num_arg);

    if (outpath)
	push_string(outpath, STRING_MALLOC);
    else
	push_number(0);
}




void efun_acl_str2num(struct svalue *sp)
{
    char *path;
    int i;
    
    i = -1;
    
    path = sp->u.string;
    if (path)
	i = acl_str2rights(path);
    
    pop_n_elems(1);
    if (i == -1)
	i = ACL_NONE;
    
    push_number(i);
}


void efun_acl_num2str(struct svalue *sp)
{
    char *str;
    
    str = acl_rights2str(sp->u.number);
    pop_n_elems(1);
    if (str)
	push_string(str, STRING_MALLOC);
    else
	push_number(0);
}



void efun_acl_debug(struct svalue *sp)
{
    acl_debug = sp->u.number;
    pop_n_elems(1);
}
