/*
** acl.c			ACL support functions.
**
** Copyright (c) 1993 Peter Eriksson <pen@lysator.liu.se>
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

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

extern struct svalue *sapply(char *, struct object *, int);

extern struct object *master_ob;
extern struct object *current_interactive;

extern char real_mudlibpath[];
extern char *strip_fullpath(char *str);
extern char *strip_abspath(char *str);

int acl_debug = 0;
int acl_silentfail = 0;


/*
** Return the parent directory part of the pathname
** sent into this function as a dynamically allocated
** string. Append a trailing slash (/) if the "tslash" arg
** is true. Returns "/" if a top-level object.
*/
static char *dirname_of(char *str, int tslash)
{
    char *tmp;
    char *cp;
    
    if (str == NULL)
	return NULL;
    
    tmp = xalloc(strlen(str)+5);
    strcpy(tmp, str);

    /* Loop, removing trailing slashes */
    while ((cp = strrchr(tmp, '/')) && cp > tmp && cp[1] == '\0')
	*cp = '\0';

    /* Truncate pathname at / unless it is the first character */
    if (cp)
    {
	if (cp > tmp)
	    cp[0] = '\0';
	else
	    cp[1] = '\0';
    }
    else
	strcpy(tmp, ".");

    /* Append a trailing slash unless we are at the root dir */
    if (tslash && ((cp && cp > tmp) || !cp))
	strcat(tmp, "/");
    
    return tmp;
}



/*
** Return a clean-up:ed "object" name. The string returned
** is dynamically allocated.
*/
char *acl_fixobjname(char *str)
{
    char *cp;
    char *tmp;

    if (str == NULL)
	return NULL;
    
    tmp = xalloc(strlen(str)+2);
    
    if (*str != '/')
	strcpy(tmp, "/");
    else
	*tmp = '\0';
    
    strcat(tmp, str);

    cp = strrchr(tmp, '#');
    if (cp)
	*cp = '\0';

    return tmp;
}


/*
** Get the following information about the (LPC object) caller:
**
**  oname = LPC object pathname, with leading / and no "#4534" number.
**  pname = The name of the current interactive player/wizard..
**
** All strings returned are dynamically allocated.
*/
static int get_info(char **oname, char **pname)
{
    struct svalue *arg1;
    

    if (oname)
    {
	if (current_object)
	    /* Get name of calling object,
	       with leading / and no #1231 number */
	    *oname = acl_fixobjname(current_object->name);
	else
	    *oname = NULL;
    }

    if (pname)
    {
	/* We have a current interactive player/wizard? Get the name */
	if (current_interactive &&
	    !(current_interactive->flags & O_DESTRUCTED))
	{
#if 0
	    arg1 = sapply("query_real_name", current_interactive, 0);
	    if (!arg1 || arg1->type != T_STRING)
		return -1;
	    *pname = string_copy(arg1->u.string);
#else
	    if (current_interactive->living_name)
		*pname = string_copy(current_interactive->living_name);
	    else
		*pname = NULL;
#endif    
	}
	else
	    *pname = NULL;
    }

    return 0;
}


/*
** Verify the access rights applying for oname/name in the ACL.
** Returns 0 if access is denied, any other number means access allowed.
*/
static int checkrights(ACL *acl,
		       char *oname,
		       char *pname,
		       int rights,
		       int *is_trusted)
{
#if 1
    char *tmp;
    int o_rights = 0;
    int r_rights = 0;
    
    if (acl_debug >= 2)
	printf("\tcheckrights(..., \"%s\", \"%s\", %s, %d)\n",
	       oname ? oname : "<null>",
	       pname ? pname : "<null>",
	       acl_rights2str(rights),
	       *is_trusted);
    
    /* Get calling object rights */
    if (oname)
    {
	o_rights = acl_getrights(acl, oname);

	/* Calling object have access? */
	if (r_rights = (o_rights & rights))
	{
	    if (acl_debug >= 2)
		printf("\t    -> %s\n", acl_rights2str(r_rights));
	    
	    return r_rights;
	}

	if (!*is_trusted)
	{
	    *is_trusted = (o_rights & ACL_T);
	    if (acl_debug >= 2 && *is_trusted)
		printf("\t    (Trusted Found)\n");
	}
	
	/* Directory containing object acess rights? */
	tmp = dirname_of(oname, 1);
	if (tmp == NULL)
	{
	    if (acl_debug >= 2)
		printf("\t    -> NONE (dirname_of failure)\n");
	    
	    return 0;
	}
	
	oname = NULL;
	do
	{
	    if (oname)
		free(oname);

	    if (acl_debug >= 3)
		printf("\t    (Checking rights for \"%s\" -> ", tmp);
	    
	    r_rights = acl_getrights(acl, tmp);

	    if (acl_debug >= 3)
		printf("%s)\n", acl_rights2str(r_rights));
	    
	    if (!*is_trusted)
	    {
		*is_trusted = (r_rights & ACL_T);
		if (acl_debug >= 2 && *is_trusted)
		    printf("\t    (Trusted Found)\n");
	    }
    
	    r_rights &= rights;
	    oname = tmp;
	    
	} while (r_rights == ACL_NONE &&
		 strcmp(oname, "/") != 0 && (tmp = dirname_of(oname, 1)));
	
	if (oname)
	    free(oname);
	
	if (r_rights)
	{
	    if (acl_debug >= 2)
		printf("\t    -> %s\n", acl_rights2str(r_rights));
	    
	    return r_rights;
	}
    }
    
    /* Calling wizard/player (?) access rights? */
    if (pname && *is_trusted)
    {
	if (acl_debug >= 2)
	    printf("\t    * Object is trusted, checking wizard rights:");
	
	if (r_rights = (acl_getrights(acl, pname) & rights))
	{
	    if (acl_debug >= 2)
		printf("%s\n", acl_rights2str(r_rights));
	    
	    return r_rights;
	}
/*	else
	    puts("NONE\n");   /Orjan */
    }

    if (acl_debug >= 2)
	printf("    -> Default rights returned: %s\n",
	       acl_rights2str(acl->rest_rights & rights));
    
    return acl->rest_rights & rights;
#else
    char *tmp;
    int o_rights = 0;
    int r_rights = 0;

    
    /* Get calling object rights */
    if (oname)
	o_rights = acl_getrights(acl, oname);
    
    
    /* Calling wizard/player (?) access rights? */
    if (pname)
    {
	if (o_rights & ACL_T)
	    if (r_rights = (acl_getrights(acl, pname) & rights))
		return r_rights;
    }

    /* Calling object access rights? */
    if (oname)
    {
	/* Calling object access rights? */
	if (r_rights = (o_rights & rights))
	    return r_rights;

	/* Directory containing object acess rights? */
	tmp = dirname_of(oname, 1);
	if (tmp == NULL)
	    return 0;
	
	oname = NULL;
	do
	{
	    if (oname)
		free(oname);
	    
	    r_rights = (acl_getrights(acl, tmp) & rights);
	    
	    oname = tmp;
	} while (r_rights == ACL_NONE &&
		 strcmp(oname, "/") != 0 && (tmp = dirname_of(oname, 1)));
	
	if (oname)
	    free(oname);
    }
	
    if (r_rights)
	return r_rights;
    else
	return acl->rest_rights & rights;
#endif
}


static int fexists(char *path)
{
    struct stat sb;

    return stat(path, &sb) < 0 ? 0 : 1;
}

/*
** Test if the calling object have the rights specified on
** the filesystem object in question. If 'ckparent' is true
** any rights the caller have on the parent directory is also
** taken into consideration.
**
** Returns the rights the are available after anding "rights".
*/
int acl_findrights(char **path, int rights, int ckparent, int is_trusted)
{
    ACL *acl = NULL;
    int ok   = 0;
    char *oname = NULL;
    char *pname = NULL;
    
    
    if (acl_debug)
	printf("acl_findrights(\"%s\", %s, %d)\n",
	       *path, acl_rights2str(rights), ckparent);

    if (get_info(&oname, &pname) < 0)
    {
	if (acl_debug)
	{
	    puts("   -> Access Denied (get_info)");
	    fflush(stdout);
	}
	else
	    if (!acl_silentfail)
	    {
		printf("acl_findrights(\"%s\", %s, %d, %d)\n",
		       *path, acl_rights2str(rights), ckparent, is_trusted);
		printf("    Failed get_info() call!\n");
		fflush(stdout);
	    }

	return ACL_NONE;
    }

    if (acl_debug)
	printf("    (for player %s, object %s)\n",
	       pname ? pname : "<null>", oname ? oname : "<null>");

    *path = strip_abspath(*path);

    /* Check priority ACL */
    if (acl = acl_get(NULL,NULL))
    {
	if (acl_debug)
	    printf("    (Checking Priority ACL...");

	ok = checkrights(acl, oname, pname, rights, &is_trusted);
	acl_free(acl);
	if (ok)
	{
	    if (acl_debug)
		printf(" Access OK)\n");
	    goto End;
	}
	else
	    if (acl_debug)
		printf(" Access Denied).\n");
    }
    
    if (ckparent == 2)
    {
	if (fexists(*path))
	{
	    rights &= ACL_W;
	    ckparent = 0;
	}
	else
	    rights &= ACL_A;
    }

    if (ckparent != 2)
    {
	acl = acl_get(*path, NULL);
	if (acl)
	{
	    int tmp_trusted = is_trusted;
	    
	    ok = checkrights(acl, oname, pname, rights, &is_trusted);

	    is_trusted = tmp_trusted;
	    
	    acl_free(acl);
	}
	
    }

    if (ok == ACL_NONE && ckparent)
    {
	char *parent;

	if (acl_debug)
	    puts("   (direct target Access Denied, checking parent)");
	
	parent = dirname_of(*path, 0);
	if (acl_debug)
	    printf("   (parent = %s)\n", parent);
	
	if (parent == NULL)
	    goto End;
	
	acl = acl_get(parent, NULL);
	if (acl)
	{
	    ok = checkrights(acl, oname, pname, rights, &is_trusted);
	    acl_free(acl);
	}
	
	free(parent);
    }

  End:

    if (acl_debug)
    {
	if(ok)
	    puts("   -> Access OK");
	else
	    puts("   -> Access Denied");
	fflush(stdout);
    }
    else
	if (ok == ACL_NONE && !acl_silentfail)
	{
	    int f = fexists(*path);
	    
	    if (!((rights & (ACL_R|ACL_D)) && !f))
	    {
		printf("acl_findrights(\"%s\", %s, %d, %d)\n",
		       *path, acl_rights2str(rights), ckparent, is_trusted);
		printf("    (for player %s, object %s)\n",
		       pname ? pname : "<null>", oname ? oname : "<null>");
		printf("    -> Access Denied.\n");
		
		fflush(stdout);
	    }
	}
	
    if (oname)
	free(oname);
    if (pname)
	free(pname);

    return ok;
}


char *acl_checkrights(char *path, int rights, int ckparent, int is_trusted)
{
    int ok;
    if(strlen(path) > MAXPATHLEN) return 0;
    ok = acl_findrights(&path, rights, ckparent, is_trusted);
    
    return ok == ACL_NONE ? NULL : path;
}
