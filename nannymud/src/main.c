#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#if defined(__sun__) && defined(__svr4__)
#include <sys/procfs.h>
#endif
#include <string.h>
#include <signal.h>
#include <math.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#if defined(sun)
#include <alloca.h>
#endif
#include "lint.h"
#include "interpret.h"
#include "object.h"
#include "config.h"
#include "lex.h"
#include <sys/param.h>

extern char *prog;

char real_mudlibpath[MAXPATHLEN];

extern int current_line;
int start_time;

int d_flag = 0;	/* Run with debug */
int t_flag = 0;	/* Disable heart beat and reset */
int e_flag = 0;	/* Load empty, without castles. */
int comp_flag = 0; /* Trace compilations */
char *swap_file_name; /* Filename for the swapfile */
long time_to_swap = TIME_TO_SWAP; /* marion - invocation parameter */

#ifdef YYDEBUG
extern int yydebug;
#endif

#if defined(__sun__) && defined(__svr4__)
static int proc_fd;
void open_proc_fd();
#endif

int port_number = PORTNO;
char *reserved_area;	/* reserved for malloc() */
struct svalue const0, const1;

double consts[5];

extern jmp_buf error_recovery_context;
extern int error_recovery_context_exists;

extern struct object *master_ob;

struct wiz_list *back_bone_uid;

#ifdef OLD_STUPID_IP_DEMON
static void start_ip_demon();
#endif

void sig_handler()
{
   int st;
   puts("Got child signal");
   waitpid(-1, &st, WNOHANG);
}

int main(argc, argv)
    int argc;
    char **argv;
{
    extern int cfg_use_acls;
    extern int acl_debug;
    extern int game_is_being_shut_down;
    extern int current_time;
#if 0
    struct sigaction sact;
#endif
    int i, new_mudlib = 0;
    int no_ip_demon = 0;
    char *p;

#ifndef COMPAT_MODE
    struct svalue *ret;
#endif
#ifdef MALLOC_gc
    extern void gc_init();
#endif
    extern char driver_version[];
    
#ifdef MALLOC_sysmalloc
    init_malloc();
#endif

    printf("[%s now booting. Please wait...]\n", driver_version);

#if defined(__sun__) && defined(__svr4__)
    open_proc_fd();
#endif

    signal(SIGCHLD, SIG_IGN);
#if 0
    signal(SIGCHLD, sig_handler);
    if(sigaction(SIGCHLD, NULL, &sact) != -1)
    {
       sact.sa_flags &= ~SA_RESETHAND;
       sigaction(SIGCHLD, &sact, NULL);
    }
#endif
    
#ifdef MALLOC_gc
    gc_init();
#endif
    const0.type = T_NUMBER; const0.u.number = 0;
    const1.type = T_NUMBER; const1.u.number = 1;
    /*
     * Check that the definition of EXTRACT_UCHAR() is correct.
     */
    p = (char *)&i;
    *p = -10;
    if (EXTRACT_UCHAR(p) != 0x100 - 10) {
	fprintf(stderr, "Bad definition of EXTRACT_UCHAR() in config.h.\n");
	exit(1);
    }
    current_time = start_time = get_current_time();
    
    srand(current_time); 
#ifdef DRAND48
    srand48(current_time);
#else
#ifdef RANDOM
    srandom(current_time);
#else
/*    fprintf(stderr, "No random generator specified!\n"); */
#endif /* RANDOM */
#endif /* DRAND48 */
    if (RESERVED_SIZE > 0)
	reserved_area = (char *) malloc(RESERVED_SIZE);

    /* Check the number of file descriptors. Allocate some
     * more if we have to.
     */
/*    if(MAX_PLAYERS + MAX_EFUN_SOCKS + 20 > getdtablesize()) */

    {
      struct rlimit rl;
      getrlimit(RLIMIT_NOFILE, &rl);
      rl.rlim_cur = MAX_PLAYERS + MAX_EFUN_SOCKS + 20;
      if(setrlimit(RLIMIT_NOFILE, &rl) == -1)
	printf("Warning: failed to allocate %d file descriptors\n",
	       MAX_PLAYERS + MAX_EFUN_SOCKS + 20);
      else
	printf("Allocated %d file descriptors.\n", MAX_PLAYERS + 
	       MAX_EFUN_SOCKS + 20);
    }
    

    for (i=0; i < sizeof consts / sizeof consts[0]; i++)
	consts[i] = exp(- i / 900.0);
    init_num_args();
    reset_machine(1);

    /*
     * The flags are parsed twice !
     * The first time, we only search for the -m flag, which specifies
     * another mudlib, and the D-flags, so that they will be available
     * when compiling master.c.
     */
    for (i=1; i < argc; i++) {
	if (argv[i][0] != '-')
	    continue;
	switch(argv[i][1]) {
	  case 'A':
	    cfg_use_acls = 1;
	    if (argv[i][2])
		acl_debug = atoi(argv[i]+2);
	    break;
	
	case 'D':
	    if (argv[i][2]) { /* Amylaar : allow flags to be passed down to
				 the LPC preprocessor */
		struct lpc_predef_s *tmp;
		
		tmp = (struct lpc_predef_s *)
		    alloca(sizeof(struct lpc_predef_s));
		if (!tmp) fatal("alloca failed");
		tmp->flag = argv[i]+2;
		tmp->next = lpc_predefs;
		lpc_predefs = tmp;
		continue;
	    }
	    fprintf(stderr, "Illegal flag syntax: %s\n", argv[i]);
	    exit(1);
	case 'o':
	    fprintf(stderr, "-o is an obsolete flag. Use COMPAT_MODE in config.h.\n");
	    exit(0);
	    break;
	case 'N':
	    no_ip_demon++; continue;
	case 'm':
	    if (chdir(argv[i]+2) == -1) {
	        fprintf(stderr, "Bad mudlib directory: %s\n", argv[i]+2);
		exit(1);
	    }
	    new_mudlib = 1;
	    break;
	}
    }
    if (!new_mudlib && chdir(MUD_LIB) == -1) {
        fprintf(stderr, "Bad mudlib directory: %s\n", MUD_LIB);
	exit(1);
    }

    getcwd(real_mudlibpath,MAXPATHLEN);

    if (setjmp(error_recovery_context)) {
	clear_state();
	add_message("Anomaly in the fabric of world space.\n");
    } else {
	error_recovery_context_exists = 1;

#ifdef COMPAT_MODE
	    master_ob = load_object("obj/master",0,0);
#else
	    master_ob = load_object("secure/master",0,0);
#endif	
    }
    error_recovery_context_exists = 0;
    if (master_ob == 0) {
	fprintf(stderr, "The file secure/master must be loadable.\n");
	exit(1);
    }
    /*
     * Make sure master_ob is never made a dangling pointer.
     * Look at apply_master_ob() for more details.
     */
    add_ref(master_ob, "main");
#ifndef COMPAT_MODE
    ret = apply_master_ob("get_root_uid", 0);
    if (ret == 0 || ret->type != T_STRING) {
	fprintf(stderr, "get_root_uid() in secure/master.c does not work\n");
	exit(1);
    }
    master_ob->user = add_name(ret->u.string);
    master_ob->eff_user = master_ob->user;
    ret = apply_master_ob("get_bb_uid", 0);
    if (ret == 0 || ret->type != T_STRING) {
	fprintf(stderr, "get_bb_uid() in secure/master.c does not work\n");
	exit(1);
    }
    back_bone_uid = add_name(ret->u.string);
#endif
    for (i=1; i < argc; i++) {
	if (atoi(argv[i]))
	    port_number = atoi(argv[i]);
	else if (argv[i][0] != '-') {
	    fprintf(stderr, "Bad argument %s\n", argv[i]);
	    exit(1);
	} else {
	    /*
	     * Look at flags. -m and -o has already been tested.
	     */
	    switch(argv[i][1]) {
	    case 'f':
		push_constant_string(argv[i]+2);
		(void)apply_master_ob("flag", 1);
		if (game_is_being_shut_down) {
		    fprintf(stderr, "Shutdown by master object.\n");
		    exit(0);
		}
		continue;
	    case 'e':
		e_flag++; continue;
	    case 'A':
		continue;
	    case 'D':
		continue;
	    case 'N':
		continue;
	    case 'm':
		continue;
	    case 'd':
		d_flag++; continue;
	    case 'c':
		comp_flag++; continue;
	    case 't':
		t_flag++; continue;
#if TIME_TO_SWAP > 0
	    case 's':
		time_to_swap = atoi (&argv[i][2]);
		continue;
#endif
#ifdef YYDEBUG
	    case 'y':
		yydebug = 1; continue;
#endif
	      case 'S':
		swap_file_name = strdup(argv[i] + 2); continue;
	    default:
		fprintf(stderr, "Unknown flag: %s\n", argv[i]);
		exit(1);
	    }
	}
    }

    init_sockets();

#if 0
    load_predefs_strings();
#endif

    get_simul_efun(apply_master_ob("get_simul_efun", 0));
    if (game_is_being_shut_down)
	exit(1);
    load_wiz_file();
    set_inc_list(apply_master_ob("define_include_dirs", 0));

#ifdef COMPAT_MODE
	load_first_objects();
	(void)apply_master_ob("epilog", 0);
#else
	preload_objects(e_flag);
#endif

    backend();
    return 0;
}

char *string_copy(str)
    char *str;
{
    char *p;

    p = xalloc(strlen(str)+1);
    (void)strcpy(p, str);
    return p;
}

char *strip_fullpath(char *str)
{
    int len = strlen(real_mudlibpath);
    
    if (strncmp(str, real_mudlibpath, len) == 0)
	return str + len;
    else
	return str;
}

char *strip_abspath(char *str)
{
    static char pathbuf[MAXPATHLEN];
    int len;

    
    while (*str == '/')
	++str;

    if (!*str)
	str = ".";

    strcpy(pathbuf, str);

    do
    {
	len = strlen(pathbuf);
    } while ((pathbuf[len-1] == '/') && 
	       ((pathbuf[len-1] = '\0'), 1));
    
    return pathbuf;
}


/*VARARGS1*/
void debug_message(a, b, c, d, e, f, g, h, i, j)
    char *a;
    int b, c, d, e, f, g, h, i, j;
{
    static FILE *fp = NULL;
    char deb[100];
    char name[100];

    if (fp == NULL) {
	gethostname(name,sizeof name);
	sprintf(deb,"syslog/%s.debug.log",name);
	fp = fopen(deb, "w");
	if (fp == NULL) {
	    perror(deb);
	    abort();
	}
    }
    (void)fprintf(fp, a, b, c, d, e, f, g, h, i, j);
    (void)fflush(fp);
}

void debug_message_svalue(v)
    struct svalue *v;
{
    if (v == 0) {
	debug_message("<NULL>");
	return;
    }
    switch(v->type) {
    case T_NUMBER:
	debug_message("%d", v->u.number);
	return;
    case T_STRING:
	debug_message("\"%s\"", v->u.string);
	return;
    case T_OBJECT:
	debug_message("OBJ(%s)", v->u.ob->name);
	return;
    case T_LVALUE:
	debug_message("Pointer to ");
	debug_message_svalue(v->u.lvalue);
	return;
    default:
	debug_message("<INVALID>\n");
	return;
    }
}

#ifdef malloc
#undef malloc
#endif

int slow_shut_down_to_do = 0;

char *xalloc(size)
    int size;
{
    char *p;
    static int going_to_exit;

    if (going_to_exit)
	exit(3);
    if (size == 0)
	fatal("Tried to allocate 0 bytes.\n");
    p = (char *) malloc(size);
    if (p == 0) {
	if (reserved_area) {
	    free(reserved_area);
	    p = "Temporary out of MEMORY. Freeing reserve.\n";
	    write(1, p, strlen(p));
	    reserved_area = 0;
	    slow_shut_down_to_do = 6;
	    return xalloc(size);	/* Try again */
	}
	going_to_exit = 1;
	p = "Totally out of MEMORY.\n";
	write(1, p, strlen(p));
	(void)dump_trace(0);
	abort();
    }
    return p;
}

#ifdef OLD_STUPID_IP_DEMON
static void start_ip_demon()
{
    extern FILE *f_ip_demon, *f_ip_demon_wr;
    char path[100];
    int tochild[2], fromchild[2];
    int pid;
    char c;

    if(pipe(tochild) < 0)
        return;
    if(pipe(fromchild) < 0) {
        close(tochild[0]);
        close(tochild[1]);
        return;
    }
    if((pid = fork()) == 0) {
        /* Child */
        dup2(tochild[0], 0);
        dup2(fromchild[1], 1);
        close(tochild[0]);
        close(tochild[1]);
        close(fromchild[0]);
        close(fromchild[1]);
	if (strlen(BINDIR) + 7 <= sizeof path) {
	    sprintf(path, "%s/hname", BINDIR);
	    execl((char *)path, "hname", 0);
	}
	write(1, "0", 1);	/* indicate failure */
        fprintf(stderr, "exec of hname failed.\n");
        _exit(1);
    }
    if(pid == -1) {
        close(tochild[0]);
        close(tochild[1]);
        close(fromchild[0]);
        close(fromchild[1]);
        return;
    }
    close(tochild[0]);
    close(fromchild[1]);
    read(fromchild[0], &c, 1);
    if (c == '0') {
        close(tochild[1]);
        close(fromchild[0]);
	return;
    }
    f_ip_demon_wr = fdopen(tochild[1], "w");
    f_ip_demon = fdopen(fromchild[0], "r");
    if (f_ip_demon == NULL || f_ip_demon_wr == NULL) {
	f_ip_demon = NULL;
        close(tochild[1]);
        close(fromchild[0]);
    }
}
#endif

#if defined(__sun__) && defined(__svr4__)
void open_proc_fd()
{
  char proc_name[20];

  sprintf(proc_name, "/proc/%05d", getpid());
  proc_fd = open(proc_name, O_RDONLY);
  if(proc_fd < 0)
    fprintf(stderr, "Couldn't open %s\n", proc_name);
}
#endif

#if defined(__sun__) && defined(__svr4__)
static struct vector *get_time_vec(timestruc_t val)
{
  struct vector *v;

  v = allocate_array(2);
  
  v->item[0].u.number = val.tv_sec;
  v->item[1].u.number = val.tv_nsec;

  return v;
}
#endif

struct vector *get_rusage()
{
  struct vector *v;

#if defined(__sun__) && defined(__svr4__)
  prusage_t  pru;
  prstatus_t prs;

  if(ioctl(proc_fd, PIOCUSAGE, &pru) < 0)
  {
    return NULL;
  }
  
  if(ioctl(proc_fd, PIOCSTATUS, &prs) < 0)
  {
    return NULL;
  }
#endif
  
  v = allocate_array(25);

#if defined(__sun__) && defined(__svr4__)
  v->item[0].u.number = pru.pr_minf;
  v->item[1].u.number = pru.pr_majf;
  v->item[2].u.number = pru.pr_nswap;
  v->item[3].u.number = pru.pr_inblk;
  v->item[4].u.number = pru.pr_oublk;
  v->item[5].u.number = pru.pr_msnd;
  v->item[6].u.number = pru.pr_mrcv;
  v->item[7].u.number = pru.pr_sigs;
  v->item[8].u.number = pru.pr_vctx;
  v->item[9].u.number = pru.pr_ictx;
  v->item[10].u.number = pru.pr_sysc;
  v->item[11].u.number = pru.pr_ioch;
  v->item[12].type     = T_POINTER;
  v->item[12].u.vec    = get_time_vec(pru.pr_rtime);
  v->item[13].type     = T_POINTER;
  v->item[13].u.vec    = get_time_vec(pru.pr_utime);
  v->item[14].type     = T_POINTER;
  v->item[14].u.vec    = get_time_vec(pru.pr_stime);
  v->item[15].type     = T_POINTER;
  v->item[15].u.vec    = get_time_vec(pru.pr_ttime);
  v->item[16].type     = T_POINTER;
  v->item[16].u.vec    = get_time_vec(pru.pr_tftime);
  v->item[17].type     = T_POINTER;
  v->item[17].u.vec    = get_time_vec(pru.pr_dftime);
  v->item[18].type     = T_POINTER;
  v->item[18].u.vec    = get_time_vec(pru.pr_kftime);
  v->item[19].type     = T_POINTER;
  v->item[19].u.vec    = get_time_vec(pru.pr_ltime);
  v->item[20].type     = T_POINTER;
  v->item[20].u.vec    = get_time_vec(pru.pr_slptime);
  v->item[21].type     = T_POINTER;
  v->item[21].u.vec    = get_time_vec(pru.pr_wtime);
  v->item[22].type     = T_POINTER;
  v->item[22].u.vec    = get_time_vec(pru.pr_stoptime);
  v->item[23].u.number = prs.pr_brksize;
  v->item[24].u.number = prs.pr_stksize;
#endif

  return v;
}

int get_mem_used(void)
{
   static int old_mem;
   int i;
#ifdef MALLOC_smalloc
   extern int cur_mem_use(void);
   i = cur_mem_use() - old_mem;
#else
#if defined(__sun__) && defined(__svr4__)
   prstatus_t prs;
   if(ioctl(proc_fd, PIOCSTATUS, &prs) < 0)
      return 0;
   i = prs.pr_brksize - old_mem;
#else
   i = 0;
#endif
#endif
   old_mem += i;
   return i;
}
