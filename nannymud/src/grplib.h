/*
** grplib.h
**
** Copyright (c) 1993 Jones Desougi  <axel_f@lysator.liu.se>
*/

typedef struct oum
{
   char *name;
   time_t mtime;
   struct oum *next;
   int m_entries;
   char *object[1];
} GRP;

extern int grp_ismember(char *group, char *object);
