#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>

const char* TMPDIR = "/tmp";
const char* TMPFILE = "/tmp/cse.txt";
const char* UTMP = "var/run/utmp";

void writelog(char* message){
    FILE *logfile;
    logfile = fopen(TMPFILE,"a");
    fprintf(logfile, "%s\n",message);
    fclose(logfile);
}

int main(){
    pid_t pid, sid;
    FILE *logfile = NULL;
    char buffer[]  = {'x','y','z'};
    
    pid = fork();
    if( pid < 0){
        exit(EXIT_FAILURE);
    }
    
    if(pid>0){
        exit(EXIT_SUCCESS);
    }
    
    umask(0);
    sid = setsid();
    if(sid < 0){
       exit(EXIT_FAILURE);
    }
    if((chdir("/"))<0){
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGTTIN, SIG_IGN); // Background Process Read
    signal(SIGTTOU, SIG_IGN); // Background Process Write
    signal(SIGSTOP, SIG_IGN); // ctrl+z

    while(1){
        sleep(30);
        logfile = fopen(TMPFILE, "w");
        //fwrite(buffer,1,sizeof(buffer), logfile);
        writelog("hello World!");
    }
    fclose(logfile);
    return 0;
}


