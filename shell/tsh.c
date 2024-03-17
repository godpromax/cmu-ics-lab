/* 
 * tsh - A tiny shell program with job control
 * zzh 202008010418
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size 最大行数 */
#define MAXARGS     128   /* max args on a command line 命令行最大128个字符 */
#define MAXJOBS      16   /* max jobs at any point in time 最大前台和后台任务数16个 */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground 前台运行 */
#define BG 2    /* running in background 后台运行 */
#define ST 3    /* stopped 停止 */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.前台最多只有一个作业运行
 */

/* Global variables 全局变量 */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */

struct cmdline_tokens {
    int argc; /* Number of arguments */
    char *argv[MAXARGS]; /* The arguments list */
    char *infile; /* The input file */
    char *outfile; /* The output file */
    enum builtins_t { /* Indicates if argv[0] is a builtin command */
        BUILTIN_NONE, BUILTIN_QUIT, BUILTIN_JOBS, BUILTIN_BG, BUILTIN_FG
    } builtins;
};
/* Function prototypes */

/* Here are the functions that you will implement 需要补充的函数 */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you 可用的函数 */
int parseline(const char *cmdline, char **argv); //命令行输入
void sigquit_handler(int sig);                   //quit的函数

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
     char *argv[MAXARGS];        /* 参数列表execve() */
    char buf[MAXLINE];          /* 保存修改的命令行 */
    int bg;                     /* 这个作业应该在后台进行？ */
    pid_t pid;                  /* 进程id*/
    strcpy(buf,cmdline);
    bg = parseline(buf,argv);
    if(argv[0] == NULL)
        return;                 /* 忽略空命令 */
 
    sigset_t mask_all,mask_one,prev_one;
    if(!builtin_cmd(argv)){
        sigfillset(&mask_all);  /* 保存当前的阻塞信号集合(blocked位向量) */
        sigemptyset(&mask_one); //初始化mask_one为空集
        sigaddset(&mask_one,SIGCHLD);//添加SIGCHLD到mask_one中
        //以上三句保存了当前的已阻塞信号集合
 
        sigprocmask(SIG_BLOCK,&mask_one,&prev_one);         /* 添加mask_one中的信号到信号集合（blocked位向量），从而父进程保持SIGCHLD的阻塞*/
        if((pid = fork()) == 0){    /* 子程序运行用户作业 */
 
            sigprocmask(SIG_SETMASK,&prev_one,NULL); /* 因为子进程继承了它们父进程的被阻塞集合，所以在调用execve之前，必须
                                                         解除子进程对SIGCHLD的阻塞，避免子进程fork出来的进程无法被回收*/
 
 
       if(setpgid(0,0) < 0){               /* 把子进程放到一个新进程组中，该进程组ID与子进程的PID相同。这将确保前台进程组中只有一个进程，即shell进程。*/
        printf("setpgid error");
        exit(0);
    }
            if(execve(argv[0],argv,environ) < 0){
                printf("%s: Command not found.\n",argv[0]);
       //第一个错误处理，直接在这里进行提示信息输出
                exit(0);
            }
        }
        
        sigprocmask(SIG_BLOCK,&mask_all,NULL);   /* 恢复信号集合(blocked位向量) */
        addjob(jobs,pid,bg==1 ? BG : FG,cmdline); /* 将子任务添加到任务列表中 */
        sigprocmask(SIG_SETMASK,&prev_one,NULL);  /* 解除子进程对SIGCHLD的阻塞 */
        /* 这样子sigchld_handler处理程序在我们将其添加到工作队列
            中之前是不会运行的。因为直到addjob()之后，我们才解除对SIGCHLD的阻塞
         */
 
 
        /* 父任务等待前台任务结束 */
        if (!bg){ //如果不是后台进程，就等待当前的前台进程
            waitfg(pid);
        }else{  /* 否则就是后台进程，开始在后台工作 */
            printf("[%d] (%d) %s",pid2jid(pid),pid,cmdline);
        }
    }
 
    return;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);       //buf=cmdline
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces 忽略空格 */
	buf++;

    /* Build the argv list 建立argv数组，就是分开割裂cmdline中的各个参数 */
    argc = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');//查找第一个出现\的位置
    }
    else {
	delim = strchr(buf, ' ');//查找第一个出现 的位置
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{
    if(!strcmp(argv[0],"quit")) //如果是内置命令quit
        exit(0);               //就结束当前进程
   if(!strcmp(argv[0],"jobs")){    /* jobs内置指令 */
        listjobs(jobs);
        return 1;
    }
   if(!strcmp(argv[0],"&")) /* 忽略单& */
        return 1;  //然后返回1，因为如果一个命令以&结尾，shell应该在后台运行它，否则在前台运行；
 
  if(!strcmp(argv[0],"bg")){      /* bg内置指令 */
    do_bgfg(argv);
    return 1;
}
   if(!strcmp(argv[0],"fg")){      /* fg内置指令 */
    do_bgfg(argv);
    return 1;
  }

    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    pid_t pid;                      /* 进程id */
    int jid;                        /* job的id */
    struct job_t * job;
    if (argv[1] == NULL){
        printf("%s command requires PID or %%jobid argument\n",argv[0]);
        return;
    }
    //第二个错误是没有传入pid或者jid（为空），就报错并返回
 
    if (argv[1][0] == '%'){   /* 如果输入的是jid（作业） */
        if(argv[1][1] < '0' || argv[1][1] >'9'){
            printf("fg: argument must be a PID or %%jobid\n");
            return;
        }
   //第三个错误命令是传入了，但是传入的数据不是不符合pid或jid的规范（输入必须为数字）
   //在这里判断并输出错误信息：fg: argument must be a PID or %%jobid\n
        jid = atoi(argv[1]+1);
        job = getjobjid(jobs,jid);//通过jid找到需要执行的job
        if(job == NULL){
            printf("%%%d: No such job\n",jid);
            return;
        }
   //第四个错误就是通过jid找到的job==null，因此“NO such job”
        pid = job->pid;
    }else{                              /* 给的是pid */
        if(argv[1][0] < '0' || argv[1][0] >'9'){
            printf("bg: argument must be a PID or %%jobid\n");
            return;
        }
        pid = atoi(argv[1]);
        job = getjobpid(jobs,pid);
        if(job == NULL){
            printf("(%d): No such process\n",pid);
            return;
        }
//第五个错误就是通过jid找到的job==null，因此“NO such job”
        jid = job->jid;
    }
    if(pid > 0){
        if(!strcmp(argv[0],"bg")){          /* bg内置指令 */
            printf("[%d] (%d) %s",jid,pid,job->cmdline);
            job->state = BG;                /* 更改状态 */
            kill(-pid,SIGCONT);              /* 传递SIGCONT信号给进程组中的所有进程 */
            
        }else
        if(!strcmp(argv[0],"fg")){          /* fg内置指令 */
            job->state = FG;                /* 更改状态 */
            kill(-pid,SIGCONT);             /* 传递SIGCONT信号给进程组中的所有进程 */
            waitfg(pid);                    /* 等待前台job完成 */
        }
    }
 
    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    /* 唯一的前台作业结束后，被sigchld_handler回收，deletejob()后，jobs列表中就没有前台作业了，
        循环fpgid(..)
    */
    while(pid==fgpid(jobs)){
        sleep(0);
    }

    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    int olderrno = errno;
    sigset_t mask_all,prev_all;
    pid_t pid;
    int status;
 
    sigfillset(&mask_all);                          /* 保存当前的信号集合(blocked位向量) */
    while((pid = waitpid(-1,&status,WNOHANG | WUNTRACED)) > 0){    /* WNOHANG:非阻塞的 */
        /* 通过调用exit或者一个返回(return)正常终止 */
        if(WIFEXITED(status)){
            sigprocmask(SIG_BLOCK,&mask_all,&prev_all); /* 恢复信号集合(blocked位向量) */
            deletejob(jobs,pid);     
            sigprocmask(SIG_SETMASK,&prev_all,NULL);                   
        }
        /* 子进程是因为一个未被捕获的信号终止的(SIGINT) */
        if(WIFSIGNALED(status)){
            int jid = pid2jid(pid);
            printf("Job [%d] (%d) terminated by signal %d\n",jid,pid,WTERMSIG(status));
            deletejob(jobs,pid);/* 终止就删除pid的job */
        }
        /* 引起返回的子进程当前是停止的(SIGTSTP) */
        if(WIFSTOPPED(status)){
            struct job_t * job = getjobpid(jobs,pid);
            int jid = pid2jid(pid);
            printf("Job [%d] (%d) stopped by signal %d\n",jid,pid,WSTOPSIG(status));
            job->state = ST;			/* 状态设为停止(ST) */
        }
        
    }
    errno = olderrno;

    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    pid_t pid = fgpid(jobs);	/* 获取前台进程id */
    if(pid > 0){
        kill(-pid,sig);     	/* 转发信号sig给进程组|pid|中的每个进程 */
    }
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
     pid_t pid = fgpid(jobs);    /* 获取前台进程id */
 
    if(pid > 0){
        kill(-pid,sig);         /* 转发信号sig给进程组|pid|中的每个进程 */
    }
    return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == pid) {
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG)
	    return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid)
	    return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("listjobs: Internal error: job[%d].state=%d ", 
			   i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
	}
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}



