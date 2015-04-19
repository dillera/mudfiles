/*
** acl.c
**
** Copyright (c) 1993 Peter Eriksson <pen@lysator.liu.se>
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#include "acllib.h"
#include "grplib.h"

#define PRIORITY_ACL_PATH "etc/priority_acl"


/*
** Global priority ACL
*/
static ACL *priority_acl = NULL;
static time_t last_read  = 0;


static update_priacl()
{
  extern int current_time;
  struct stat sb;

  if (priority_acl)
  {

    /* Don't bother to read if last check was within 60 seconds */
    if (current_time - last_read < 60)
      return;

    /* Don't bother to read if the file hasn't been modified */
    if (stat(PRIORITY_ACL_PATH, &sb) >= 0 && sb.st_mtime <= last_read)
    {
      last_read = current_time;
      return;
    }
    
    acl_free(priority_acl);
  }
    
  priority_acl = _acl_read(PRIORITY_ACL_PATH);
  last_read = current_time;
}



int acl_str2rights(char *rights)
{
    int val;

    
    if (strcasecmp(rights, "ALL") == 0)
	return ACL_ALL;

    if (strcasecmp(rights, "NONE") == 0)
	return ACL_NONE;
    
    val = 0;
    
    while (*rights)
	switch (toupper(*rights++))
	{
	  case 'P':
	    val |= ACL_P;
	    break;
	    
	  case 'D':
	    val |= ACL_D;
	    break;
	    
	  case 'A':
	    val |= ACL_A;
	    break;
	    
	  case 'L':
	    val |= ACL_L;
	    break;
	    
	  case 'U':
	    val |= ACL_U;
	    break;
	    
	  case 'R':
	    val |= ACL_R;
	    break;
	    
	  case 'W':
	    val |= ACL_W;
	    break;
	    
	  case 'X':
	    val |= ACL_X;
	    break;

	  case 'T':
	    val |= ACL_T;
	    break;

	  default:
	    /* Just ignore */
	}
    
    return val;
}


char *acl_rights2str(int rights)
{
    static char buf[32];
    char *cp;
    
    if (rights == ACL_ALL)
	return "ALL";

    if (rights == ACL_NONE)
	return "NONE";
    
    cp = buf;
    
    if (rights & ACL_P)
	*cp++ = 'P';
    if (rights & ACL_D)
	*cp++ = 'D';
    if (rights & ACL_A)
	*cp++ = 'A';
    if (rights & ACL_L)
	*cp++ = 'L';
    if (rights & ACL_U)
	*cp++ = 'U';
    if (rights & ACL_R)
	*cp++ = 'R';
    if (rights & ACL_W)
	*cp++ = 'W';
    if (rights & ACL_X)
	*cp++ = 'X';
    if (rights & ACL_T)
	*cp++ = 'T';
    *cp = '\0';

    return buf;
}



static char *acl_basename(char *path, char **filename)
{
    int i;
    static char pathbuf[MAXPATHLEN];
    

    if (path == NULL)
	return NULL;
    
    if(pathbuf != path) /* profezzorn */ 
      strcpy(pathbuf, path);

    path = pathbuf;
    
    i = strlen(path) - 1;

    /* Remove trailing / */
    if (i > 0 && path[i] == '/')
	i--;
    
    while (i > 0 && path[i] != '/')
	i--;

    if (i > 0)
    {
	path[i] = '\0';
	if (filename)
	    *filename = path+i+1;
    }
    else if (path[0] == '/')
    {
	if (filename)
	    *filename = path+1;
	path = "/";
    }
    else
    {
	if (filename)
	    *filename = path;
	path = ".";
    }

    return path;
}

#define EXPANDHASHSIZE   97

struct expand_cache_t
{
   char ibuf[MAXPATHLEN];
   char obuf[MAXPATHLEN];
   int tstamp;
};

static struct expand_cache_t expand_cache[EXPANDHASHSIZE];
int expand_cache_calls = 0;
int expand_cache_hits = 0;

static int expand_cache_hash(char *istr)
{
  extern char real_mudlibpath[];
  char *str = istr;
  int len;

  if(istr[0]=='/' && strlen(istr) > (len = strlen(real_mudlibpath)))
    str += len;
  return hashstr(str, 20, EXPANDHASHSIZE);
}

static int lookup_expand_cache(char *ipath, char *opath)
{
   extern int current_time;
   int h = expand_cache_hash(ipath);
   if(!expand_cache[h].tstamp ||
      expand_cache[h].tstamp < current_time - 600 ||
      strcmp(ipath, expand_cache[h].ibuf))
   {
      return h;
   }
   strcpy(opath, expand_cache[h].obuf);
   return -1;
}

static char *expand_path(char *path)
{
#if 0
   extern char *getcwd(char *buf,int size);
#endif
   extern char real_mudlibpath[];
   extern int current_time;

   static char epath[MAXPATHLEN];
   struct stat sbuf;
   int s_errno, h;
   char *fname;

   expand_cache_calls++;

   if(path[0] != '/')
   {
      strcpy(epath, real_mudlibpath);
      strcat(epath, "/");
      strcat(epath, path);
   }
   else
      strcpy(epath, path);

   if(stat(epath, &sbuf) < 0)
      return NULL;

   fname = NULL;
   if(!S_ISDIR(sbuf.st_mode))
      path = acl_basename(epath, &fname);
   else
      path = epath;

   if((h = lookup_expand_cache(path, epath)) != -1)
   {
      if(chdir(path) < 0)
	 return NULL;
      if(getcwd(epath, MAXPATHLEN) == NULL)
      {
	 s_errno = errno;
	 chdir(real_mudlibpath);
	 errno = s_errno;
	 return NULL;
      }
      if(chdir(real_mudlibpath) < 0)
	 return NULL;
      strcpy(expand_cache[h].ibuf, path);
      strcpy(expand_cache[h].obuf, epath);
      expand_cache[h].tstamp = current_time;
   }
   else
      expand_cache_hits++;

   if(fname)
   {
      strcat(epath, "/");
      strcat(epath, fname);
   }
    
   return epath;
}




ACL *acl_grow(ACL *acl, int size)
{
    acl->s_entries += size;
    acl = realloc(acl, sizeof(ACL) +
		  acl->s_entries * sizeof(acl->entries[0]));

    return acl;
}

ACL *acl_alloc(int size)
{
    ACL *acl;

    acl = malloc(sizeof(ACL) + size * sizeof(acl->entries[0]));
    if (acl == NULL)
	return NULL;
    
    acl->s_entries = size;
    acl->refcnt = 1;
    acl->rest_rights = ACL_NONE;
    acl->n_entries = 0;

    return acl;
}

ACL *_acl_read(char *path)
{
  ACL *acl;
  FILE *fp;
  char object[256];
  char rights[32];
  int s_errno;

  fp = fopen(path, "r");
  if (fp == NULL)
    return NULL;
    
  acl = acl_alloc(32);
  if (acl == NULL)
  {
    s_errno = errno;
    fclose(fp);
    errno = s_errno;
    return NULL;
  }

    
  while (fscanf(fp, " %255[^: ] : %31s", object, rights) == 2)
  {
    if (strcasecmp(object, "$REST") == 0)
      acl->rest_rights = acl_str2rights(rights);
    else
    {
      if (acl->n_entries >= acl->s_entries)
      {
	acl = acl_grow(acl, 32);
	if (acl == NULL)
	{
	  s_errno = errno;
	  fclose(fp);
	  errno = s_errno;
	  return NULL;
	}
      }
	    
      acl->entries[acl->n_entries].object = strdup(object);
      if (acl->entries[acl->n_entries].object == NULL)
      {
	s_errno = errno;
	free(acl);
	fclose(fp);
	errno = s_errno;
	return NULL;
      }
	    
      acl->entries[acl->n_entries].rights = acl_str2rights(rights);
	    
      acl->n_entries++;
    }
  }

  fclose(fp);

  return acl;
}


ACL *acl_read(char *path)
{
    struct stat sbuf;
    char pathbuf[MAXPATHLEN];
    char *cp;

    if (stat(path, &sbuf) < 0)
	return NULL;

    if (S_ISDIR(sbuf.st_mode))
    {
	strcpy(pathbuf, path);
	strcat(pathbuf, "/.acl");
    }
    else
    {
	strcpy(pathbuf, acl_basename(path, &cp));
	strcat(pathbuf, "/.acl:");
	strcat(pathbuf, cp);
    }

    return _acl_read(pathbuf);
}

struct acl_get_cache_t
{
  char ibuf[MAXPATHLEN];
  char obuf[MAXPATHLEN];
  int tstamp;
  ACL *acl;
};

static struct acl_get_cache_t acl_get_cache[EXPANDHASHSIZE];
int acl_get_cache_calls = 0;
int acl_get_cache_hits = 0;

/*
** Get the ACL in effect for a directory.
*/
ACL *acl_get(char *path, char **opath)
{
  extern int current_time;
  int h;

  ACL *acl;
  char *epath;

  if (opath)
    *opath = NULL;
    
  if (path == NULL)
  {
    update_priacl();
	
    if (priority_acl == NULL)
      return NULL;
	
    priority_acl->refcnt++;
    return priority_acl;
  }


  acl = acl_read(path);

  if (!acl)
  {
    epath = acl_basename(expand_path(path), NULL);

    if(epath)
    {

      acl_get_cache_calls ++;
      h = expand_cache_hash(epath);
      if(acl_get_cache[h].tstamp &&
	 acl_get_cache[h].tstamp > current_time - 60 &&
	 !strcmp(epath, acl_get_cache[h].ibuf))
      {
	acl_get_cache_hits ++;
	acl_get_cache[h].acl->refcnt ++;
	if(opath) 
	  *opath = acl_get_cache[h].obuf;
	return acl_get_cache[h].acl;
      }

      /* free old cache entry */
      if(acl_get_cache[h].acl)
	acl_free(acl_get_cache[h].acl);
      acl_get_cache[h].acl=0;
      acl_get_cache[h].tstamp=0;

      strcpy(acl_get_cache[h].ibuf, epath);

      while (epath && (acl = acl_read(epath)) == NULL && strcmp(epath, "/") != 0)
	epath = acl_basename(epath, NULL);

      if(acl)
      {
	/* insert new cache entry */
	acl_get_cache[h].acl = acl;
	acl->refcnt ++;
	acl_get_cache[h].tstamp = current_time;
	strcpy(acl_get_cache[h].obuf, epath);

	if(opath && acl)
	  *opath = epath;
      }
    }
  }

  return acl;
}


int acl_write(ACL *acl, char *path)
{
    FILE *fp;
    struct stat sbuf;
    char spath[MAXPATHLEN];
    char *cp;
    int i, h;

    
    if (stat(path, &sbuf) < 0)
	return -1;

    if (S_ISDIR(sbuf.st_mode))
    {
	strcpy(spath, path);
	strcat(spath, "/.acl");
    }
    else
    {
	strcpy(spath, acl_basename(path, &cp));
	strcat(spath, "/.acl:");
	strcat(spath, cp);
    }

    if (acl)
    {
	fp = fopen(spath, "w");
	if (fp == NULL)
	    return -1;
	
	for (i = 0; i < acl->n_entries; i++)
	    fprintf(fp, "%s:%s\n",
		    acl->entries[i].object,
		    acl_rights2str(acl->entries[i].rights));

	if (acl->rest_rights != ACL_NONE)
	    fprintf(fp, "$REST:%s\n", acl_rights2str(acl->rest_rights));
	fclose(fp);
    }
    else
	unlink(spath);

    /* invalidate cache */
    for(h=0;h<EXPANDHASHSIZE;h++)
    {
      if(acl_get_cache[h].tstamp)
      {
	if(acl_get_cache[h].acl)
	{
	  acl_free(acl_get_cache[h].acl);
	  acl_get_cache[h].acl=0;
	}
	acl_get_cache[h].tstamp=0;
      }
    }

    return i;
}


void acl_free(ACL *acl)
{
    int i;

    acl->refcnt--;

    if (acl->refcnt <= 0)
    {
	for (i = 0; i < acl->n_entries; i++)
	    free(acl->entries[i].object);
	free(acl);
    }
}


void acl_print(ACL *acl, char *path, char *aclpath)
{
    int i;
    int show_rest = 0;


    if (acl)
    {
	if (aclpath)
	    printf("\"%s\" protected by default ACL (from \"%s\"):\n",
		   path, aclpath);
	else
	    printf("ACL protecting \"%s\":\n", path);
	
	for (i = 0; i < acl->n_entries; i++)
	    printf("\t%s:%*s%s\n",
		   acl->entries[i].object,
		   16 - strlen(acl->entries[i].object), "",
		   acl_rights2str(acl->entries[i].rights));
	
	printf("\t$REST:           %s\n",
	       acl_rights2str(acl->rest_rights));
    }
    else
	show_rest = 1;
    
    update_priacl();
    if (priority_acl)
    {
	printf("\nPriority ACL in effect for \"%s\":\n",
	       path);
	for (i = 0; i < priority_acl->n_entries; i++)
	    printf("\t%s:%*s%s\n",
		   priority_acl->entries[i].object,
		   16 - strlen(priority_acl->entries[i].object), "",
		   acl_rights2str(priority_acl->entries[i].rights));
	if (show_rest)
	    printf("\t$REST:           %s\n",
		   acl_rights2str(priority_acl->rest_rights));
    }
}


static int acl_comparefun(struct acl_entry *e1, struct acl_entry *e2)
{
    return -strcmp(e1->object, e2->object);
}


void acl_sort(ACL *acl)
{
    qsort((char *) acl->entries,
	  acl->n_entries,
	  sizeof(acl->entries[0]),
	  (int (*)()) acl_comparefun);
}


int acl_edit(ACL *acl, char *object, int rights)
{
    int i, j;


    if (strcasecmp(object, "$REST") == 0)
    {
	acl->rest_rights = rights;
	return 0;
    }
    
    for (i = 0; i < acl->n_entries && strcmp(acl->entries[i].object,
					     object); i++)
	;

    if (rights == ACL_NONE && i >= acl->n_entries)
	return 0;

    if (rights == ACL_NONE)
    {
	free(acl->entries[i].object);
	for (j = i + 1; j < acl->n_entries; j++)
	{
	    acl->entries[j-1].object = acl->entries[j].object;
	    acl->entries[j-1].rights = acl->entries[j].rights;
	}
	acl->n_entries--;
    }
    else
    {
	if (i >= acl->n_entries)
	{
	    if (acl->n_entries >= acl->s_entries)
	    {
		acl = acl_grow(acl, 32);
		if (acl == NULL)
		    return -1;
	    }
	    
	    acl->entries[acl->n_entries].object = strdup(object);
	    if (acl->entries[acl->n_entries].object == NULL)
		return -1;
	    
	    acl->entries[acl->n_entries].rights = rights;
	    acl->n_entries++;
	    
	    acl_sort(acl);
	}
	else
	    acl->entries[i].rights = rights;
    }

    return i;
}


int acl_getrights(ACL *acl, char *object)
{
    int i;
    

    for (i = 0; i < acl->n_entries; i++)
    {
	if (acl->entries[i].object[0] == '.')
	{
	    /* "Group" object */
	    if (grp_ismember(acl->entries[i].object, object))
		return acl->entries[i].rights;
	}
	else
	{
	    if (strcasecmp(object, acl->entries[i].object) == 0)
		return acl->entries[i].rights;
	}
    }
    
    return 0;
}


int acl_access(char *path, char *object)
{
    ACL *acl;
    int rights;

    
    update_priacl();
    if (priority_acl)
    {
	rights = acl_getrights(priority_acl, object);
	if (rights)
	    return rights;
    }
    
    acl = acl_get(path, NULL);
    if (acl == NULL)
    {
	if (priority_acl)
	    return priority_acl->rest_rights;
	
	return 0;
    }

    rights = acl_getrights(acl, object);
    if (rights == 0)
	rights = acl->rest_rights;
    
    acl_free(acl);

    return rights;
}
