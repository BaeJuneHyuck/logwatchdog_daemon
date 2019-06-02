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
#include <time.h>
#include <utmp.h>

#define KST 9
#define MAXLOG 5
const char* TMPDIR = "/tmp";
const char* TMPFILE = "/tmp/cse";
const char* UTMP = "var/run/utmp";
const char* WTMP = "var/log/wtmp";
int count = 0;
FILE *logfile;

void writelog(char* message);
void check();
void test(struct utmp *log);

int main(){
    pid_t pid, sid;
    
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
    //close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    //close(STDERR_FILENO);

    signal(SIGTTIN, SIG_IGN); // Background Process Read
    signal(SIGTTOU, SIG_IGN); // Background Process Write
    signal(SIGSTOP, SIG_IGN); // ctrl+z

    while(1){
        check();
        count++;
        if(count==15){
            count = 0;
            // display
        }
        sleep(2);
    }
    return 0;
}

void check(){
    logfile = fopen(TMPFILE,"w+");
    FILE *utmpfile;
    struct utmp log;
    struct utmp login[128];
    struct utmp logout[128];
    utmpfile = fopen(WTMP,"r");
    int login_count = 0 ;
    int logout_count = 0;

    while(fread((char*)&log, sizeof(log), 1, utmpfile) == 1) {
        if(log.ut_type == 7){
            login[login_count++] = log;
        }
        if(log.ut_type == 8){
            logout[logout_count++] = log;
        }
    }
    for(int i = 0 ; i < login_count ; i++){
        int foundLogout = 0;
        for(int j = 0 ; j < logout_count ; j++){
            if(login[i].ut_pid == logout[j].ut_pid){
                char buffer[1024];
                time_t t = login[i].ut_tv.tv_sec;
                struct tm *info;
                char timelogin[20], timelogout[20];
                info = gmtime(&t);
                info->tm_hour = (info->tm_hour + KST) % 24;
                strftime(timelogin, 20, "[%m/%d %H:%M:%S] ",info);
                
                t = logout[j].ut_tv.tv_sec;
                info = gmtime(&t);
                info->tm_hour = (info->tm_hour + KST) % 24;
                 strftime(timelogout, 20, "[%m/%d %H:%M:%S] ",info);
                
                sprintf(buffer,"%-10s \tlogin: %-20s \tlogout: %-20s\n" ,
                    login[i].ut_user, timelogin, timelogout);
                fwrite(buffer,sizeof(buffer),sizeof(char), logfile);
                foundLogout = 1;
                break;
            }  
        }
        if(foundLogout == 0){
                time_t t = login[i].ut_tv.tv_sec;
                struct tm *info;
                char timelogin[20];
                char buffer[1024];
                info = gmtime(&t);
                info->tm_hour = (info->tm_hour + KST) % 24;
                strftime(timelogin, 20, "[%m/%d %H:%M:%S] ",info);
                
                sprintf(buffer,"%-10s \tlogin: %-20s \tlogout: [still running]\n" ,
                    login[i].ut_user, timelogin);
                fwrite(buffer,sizeof(buffer),sizeof(char), logfile);
        }
    }
    fclose(logfile);
}

void test(struct utmp *log){
    //printf("{ut_type: %i, utPid: %i, ut_line: %s, ut_id: %s, ut_user: %s, ut_host: %s, ut_exit: { e_termination: %d, e_exit: %d}, ut_session : %i, timeval: {tv_sec :%i, tv_usec: %i}, ut_addr_v6: %d}\n\n", log->ut_type, log->ut_pid, log->ut_line, log->ut_id, log->ut_user, log->ut_host, log->ut_exit.e_termination, log->ut_exit.e_exit, log->ut_session, log->ut_tv.tv_sec,log->ut_tv.tv_usec, log->ut_addr_v6);
}

void writelog(char* message){
    time_t t;
    struct tm *info;
    char buffer[1024];
    time(&t);
    info = gmtime(&t);
    info->tm_hour = (info->tm_hour + KST) % 24;
    strftime(buffer, 15, "[%I:%M:%S %p] ",info);
    //strcat(buffer, message);
    fwrite(buffer,sizeof(buffer),sizeof(char), logfile);
}
