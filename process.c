#include "headers.h"

/* Modify this file as needed*/
int remainingtime,excutiontime;
//int arrivaltime;
int currTime;
int main(int agrc, char * argv[])
{
    initClk();
    excutiontime=remainingtime=atoi(argv[1]);
    printf("%d",excutiontime);
    currTime=getClk();
    /*int shmid;
    key_t key;
    key=ftok("Down",'s'); 
    shmid = shmget(key, 4096, IPC_CREAT | 0644);///shared memory brtween processes and schedular
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;*/
    //int currTIme=
    while (remainingtime > 0)
    {
        remainingtime = excutiontime-(getClk()-currTime);
        currTime=getClk();
    }
    kill(SIGUSR2,getppid());
    destroyClk(false);
    return 0;
}