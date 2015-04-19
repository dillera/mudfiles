/*
** grp_lib.c                   Group handling functions
**
** Copyright (C) 1993 Peter Eriksson <pen@lysator.liu.se>
**                    Jones Desougi  <axel_f@lysator.liu.se>
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "grplib.h"

#define GLOBAL_GROUP      "etc/group"
#define PLAYER_DIR        "players"
#define CHECK_EVERY 10  /* seconds */

static GRP *head = NULL;
extern int current_time;

static void free_group(GRP *grp)
{
   int i;
   for(i = 0; i < grp->m_entries && grp->object[i]; i++)
      free(grp->object[i]);
   free(grp->name);
   free(grp);
}

static GRP *grp_load(char *group)
{
   struct stat st;
   FILE *fp;
   GRP *grp, *old, *new;
   char buf[1024], *cp, *tmp, *ptr;
   int i;

   
   if(*group == '.')
      ++group;
   
   if((cp = strchr(group, '.')) == NULL)
   {
      cp = group;
      strcpy(buf, GLOBAL_GROUP);
   }
   else
   {
      *cp = '\0';
      sprintf(buf, "%s/%s/.group", PLAYER_DIR, group);
      *cp++ = '.';
   }

/* Now done inside next loop /Hubbe
   if(stat(buf, &st) == -1)
       return NULL;
*/

   for(old = NULL, grp = head; grp; old = grp, grp = grp->next)
   {
      if(!strcasecmp(group, grp->name))
      {
	 if(current_time <= grp->mtime)
	 {
	    return grp;
	 }else{
	   if(stat(buf,&st) ==-1) return NULL;
	   if(st.st_mtime <= grp->mtime)
	   {
	     grp->mtime=current_time+CHECK_EVERY;
	     return grp;
	   }else{
	     if(old)
	       old->next = grp->next;
	     else
	       head = grp->next;
	     free_group(grp);
	     break;
	   }
	 }
      }
   }
   
   if((fp = fopen(buf, "r")) == NULL)
      return NULL;
   
   while(ptr = fgets(buf, sizeof buf, fp))
   {
      if(buf[0] == '#' || (tmp = strchr(buf, ':')) == NULL)
	 continue;
      
      *tmp++ = '\0';
      if(!strcasecmp(buf, cp))
	 break;

  }
   fclose(fp);
   
   if(!ptr || (grp = (GRP *) xalloc(sizeof(GRP))) == NULL)
      return NULL;
   if((ptr = strchr(tmp, '\n')) != NULL)
      *ptr = '\0';
   memset(grp, 0, sizeof(GRP));
   if((grp->name = (char *) xalloc(strlen(group) + 1)) == NULL)
   {
      free(grp);
      return NULL;
   }
   strcpy(grp->name, group);
   grp->m_entries = 0;
   for(i = 0; tmp; tmp = cp)
   {
      if(cp = strchr(tmp, ','))
	 *cp++ = '\0';
      if(tmp[0] == '\0')
	 continue;
      if(i >= grp->m_entries)
      {
	 int size = sizeof(GRP) + sizeof(char *) * (grp->m_entries + 5);
	 if((new = realloc(grp, size)) == NULL)
	 {
	    free_group(grp);
	    return NULL;
	 }
	 grp = new;
	 grp->m_entries += 5;
      }
      if((grp->object[i] = (char *) xalloc(strlen(tmp) + 1)) == NULL)
      {
	 free_group(grp);
	 return NULL;
      }
      strcpy(grp->object[i], tmp);
      grp->object[++i] = NULL;
   }
   grp->mtime = current_time+CHECK_EVERY;
   grp->next = head;
   head = grp;
   return grp;
}

int grp_ismember(char *group, char *object)
{
   GRP *grp;
   int i;
   if((grp = grp_load(group)) == NULL)
      return 0;
   for(i = 0; i < grp->m_entries && grp->object[i]; i++)
   {
      if(!strcasecmp(grp->object[i], object))
	 return 1;
   }
   return 0;
}
