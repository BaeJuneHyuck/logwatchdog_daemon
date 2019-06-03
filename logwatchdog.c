#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <utmp.h>
#include <fcntl.h>// O_WRONLY, O_NOCTTY

#define KST 9
#define MAXLOG 5
const char* TMPDIR = "/tmp/mydir";
const char* TMPFILE = "/tmp/mydir/cse";
const char* UTMP = "var/run/utmp";
const char* WTMP = "var/log/wtmp";
int count = 0;
FILE *logfile;

void writelog(char* message);
void check();
void writeLog();

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
    
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGTTIN, SIG_IGN); // Background Process Read
    signal(SIGTTOU, SIG_IGN); // Background Process Write
    signal(SIGSTOP, SIG_IGN); // ctrl+z

    while(1){
        check();
        count++;
        if(count==5){
            count = 0;
            writeLog();
        }
        sleep(2);
    }
    return 0;
}

void writeLog(){
    FILE* mytemp;
    char* buffer;
    long filesize;
    mytemp = fopen(TMPFILE,"r");
    fseek(mytemp, 0, SEEK_END);
    filesize = ftell(mytemp);
    rewind(mytemp);
    
    buffer = (char*) malloc((filesize)*sizeof(char));
    fread(buffer,filesize,1,mytemp);
    openlog("logwatchdog", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "%s", buffer);
    fclose(mytemp);
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

