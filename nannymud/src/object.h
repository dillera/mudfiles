
#include "config.h"

/*
 * Definition of an object.
 * If the object is inherited, then it must not be destructed !
 *
 * The reset is used as follows:
 * 0: There is an error in the reset() in this object. Never call it again.
 * 1: Normal state.
 * 2 or higher: This is an interactive player, that has not given any commands
 *		for a number of reset periods.
 */

#define O_HEART_BEAT		0x01  /* Does it have an heart beat ?      */
#if 0
#define O_IS_WIZARD		0x02  /* Is it a wizard player.c ?         */
#else
#define O_VIRTUAL		0x02  /* Is it a virtual object ?          */
#endif
#define O_ENABLE_COMMANDS	0x04  /* Can it execute commands ?         */
#define O_CLONE			0x08  /* Is it cloned from a master copy ? */
#define O_DESTRUCTED		0x10  /* Is it destructed ?                */
#define O_SWAPPED		0x20  /* Is it swapped to file             */
#define O_ONCE_INTERACTIVE	0x40  /* Has it ever been interactive ?    */
#define O_APPROVED		0x80  /* Is std/object.c inherited ?       */
#define O_RESET_STATE		0x100 /* Object in a 'reset':ed state ?    */
#define O_WILL_CLEAN_UP		0x200 /* clean_up will be called next time */
#define O_IN_DESTRUCT           0x400 /* we are executing in destruct()    */
#define O_EFUN_SOCKET           0x800 /* efun socket references object     */
#define O_OBJECT_LOCKED         0x1000 /* object in move-obj-to locked state */
#define O_NEVER_RESET           0x2000 /* Never ever reset this object again */
#define O_PROGRAM_THROWN        0x4000 /* Program freed, so there! */
#define O_PROGRAM_NOT_THROWN    0x8000 /* Program not freed, so there! */

struct object {
    unsigned short flags;	/* Bits or'ed together from above */
    short total_light;
    int next_reset;		/* Time of next reset of this object */
    int time_of_load;           /* Time when this object was created */
    int time_of_ref;		/* Time when last referenced. Used by swap */
#ifndef NOSHORTOBJREFS
    short ref;			/* Reference count. */
#ifdef DEBUG
    short extra_ref;		/* Used to check ref count. */
#endif
#else
    int ref;			/* Reference count. */
#ifdef DEBUG
    int extra_ref;		/* Used to check ref count. */
#endif
#endif
    int cpu;
#if TIME_TO_SWAP > 0
    long swap_num;		/* Swap file offset. -1 is not swapped yet. */
#endif
    struct program *prog;
    char *name;
#ifndef NO_BACKLINK_OBJS
    struct object *prev_all;
#endif
    struct object *next_all;
    struct object *next_inv;
    struct object *next_heart_beat;
    struct object *next_hash;
    struct object *contains;
    struct object *super;		/* Which object surround us ? */
    struct object *shadowing;		/* Is this object shadowing ? */
    struct object *shadowed;		/* Is this object shadowed ? */
    struct interactive *interactive;	/* Data about an interactive player */
    struct sentence *sent;
    struct wiz_list *user;		/* What wizard defined this object */
    struct wiz_list *eff_user;		/* Used for permissions */
    struct object *next_hashed_living;
    char *living_name;			/* Name of living object if in hash */
    struct svalue variables[1];		/* All variables to this program */
    /* The variables MUST come last in the struct */
};

extern struct object *load_object PROT((char *, int,int)),
        *find_object PROT((char *));
extern struct object *get_empty_object(), *find_object PROT((char *)),
	*find_object2 PROT((char *));
extern struct object *current_object, *command_giver;

extern struct object *obj_list;
extern struct object *obj_list_destruct;

struct value;
void remove_destructed_objects(), save_object PROT((struct object *, char *)),
    move_object PROT((struct object *, struct object *)),
    tell_npc PROT((struct object *, char *)),
    free_object PROT((struct object *, char *)),
    reference_prog PROT((struct program *, char *));

#ifdef DEBUG
void add_ref PROT((struct object *, char *));
#else
#define add_ref(X,Y) ((X)->ref++)
#endif
int restore_object PROT((struct object *, char *));

void tell_object_low PROT((struct object *, char *));
