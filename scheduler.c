#include "headers.h"
static void finishHandler(int signum);
static void stoploop(int signum);
static void msgArrival(int signum);
bool finished;
int noMessages;

/*struct process{
int id;
int piriority;
struct PCB* saved;
};*/

/*struct process* dequeue(struct process** queue){
//
}*/
struct msgbuff
{
    long mtype;
    //char mtext[70];
    struct process* p;
};
pqueue* q;
int  timeStep;
struct process* currentProcess; //current running process
FILE* fout;
FILE* statistics;
struct msgbuff received_message;
float avgWTA;
float avgWAITING;
int noProcesses;
float* WTAS; //array that saves weighting arround times to use it to calculate standard deviation
float stdWTA;
float utilization;

/*float calculateSD(float data[]) {
    float sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < 10; ++i) {
        sum += data[i];
    }
    mean = sum / 10;
    for (i = 0; i < 10; ++i)
        SD += pow(data[i] - mean, 2);
    return sqrt(SD / 10);
}
*/
void hpfSchedule(){
  printf("Entered");
  utilization=100;
  noProcesses=0;
  avgWTA=0;
  fout=fopen("scheduler.log","w");
  statistics=fopen("statistics.perf","w");
  fprintf(fout,"#At\ttime\tx\tprocess\ty\tstate\tarr\tw\ttotal\tz\tremain\ty\twait\tk");
  // get the shared varible with the clock to get the current time
  //int shmid = shmget(SHKEY, 4, IPC_CREAT | 0644);
  
  int key_id = ftok("keyfile", 65);
  WTAS=malloc(sizeof(float));
  int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
  int rec_val;
  while(1){
    printf("Entered");
     // rec_val = msgrcv(msgq_id, &received_message, sizeof(received_message.p), 0,     !IPC_NOWAIT);
  /*while(rec_val = msgrcv(msgq_id, &received_message, sizeof(received_message.p), 0,     IPC_NOWAIT)!=-1){
      //call enqueue inpiriority queue
     //q.enqueue(received_message.p);
     enqueue(q,received_message.p,received_message.p->piriority);
    }*/
          while(noMessages>0){
      //call enqueue inpiriority queue
     //q.enqueue(received_message.p);
     rec_val = msgrcv(msgq_id, &received_message, sizeof(received_message.p), 0,     !IPC_NOWAIT);
     noMessages--;
     enqueue(q,received_message.p,received_message.p->piriority);
      }
    if(finished&&empty(q))break;
    if(currentProcess=dequeue(q)){
    noProcesses++;
    WTAS=realloc(WTAS,noProcesses*sizeof(float));
    //call dequeue from piriority queue
    timeStep= getClk();
    currentProcess->saved->responceTime=timeStep-currentProcess->saved->arrivalTime;
    currentProcess->saved->waitingTime=timeStep-currentProcess->saved->arrivalTime;
    currentProcess->saved->pState=RUNNING;
    fprintf(fout,"At\ttime\t%d\tprocess\t%d\tstarted\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",timeStep,currentProcess->id,currentProcess->saved->arrivalTime,currentProcess->saved->executionTime,currentProcess->saved->executionTime,currentProcess->saved->waitingTime);
    int id=fork();
    // if it is child change its code with the code of the process file and send it the remainig time parameter equals execution time
    //change process.c with the full path
    
    if(id==0){
      int length=snprintf(NULL,0,"%d",currentProcess->saved->executionTime);
      char* str=malloc(length+1);
      snprintf(str,length+1,"%d",currentProcess->saved->executionTime);
       execl("./process.out","process.out",str,NULL);
      }
    //else p->id=id;
    //pause untill the process awakens it after it finishes execution
  //  pause();
   while(currentProcess->saved->pState!=finished){
      /*while(rec_val = msgrcv(msgq_id, &received_message, sizeof(received_message.p), 0,     IPC_NOWAIT)!=-1){
      //call enqueue inpiriority queue
     //q.enqueue(received_message.p);
     enqueue(q,received_message.p,received_message.p->piriority);
      }*/
      while(noMessages>0){
      //call enqueue inpiriority queue
     //q.enqueue(received_message.p);
     rec_val = msgrcv(msgq_id, &received_message, sizeof(received_message.p), 0,     !IPC_NOWAIT);
     noMessages--;
     enqueue(q,received_message.p,received_message.p->piriority);
      }
    }
    timeStep= getClk();
    currentProcess->saved->turnArroundTime=timeStep-currentProcess->saved->arrivalTime;
    currentProcess->saved->weightedTurnArroundTime=currentProcess->saved->turnArroundTime/currentProcess->saved->executionTime;
    WTAS[noProcesses-1]=currentProcess->saved->weightedTurnArroundTime;
    avgWTA=avgWTA+currentProcess->saved->weightedTurnArroundTime;
    avgWAITING=avgWAITING+currentProcess->saved->waitingTime;
    
    /*while(rec_val = msgrcv(msgq_id, &received_message, sizeof(received_message.p), 0,     IPC_NOWAIT)!=-1){
      //call enqueue inpiriority queue
     //q.enqueue(received_message.p);
     enqueue(q,received_message.p,received_message.p->piriority);
    }*/
    }
  }
  /*avgWAITING=avgWAITING/noProcesses;
  avgWTA=avgWTA/noProcesses;
  stdWTA=calculateSD(WTAS);
  fprintf(statistics,"CPU\tutilization\t=\t%f%%",utilization);
  fprintf(statistics,"Avg\tWTA\t=\t%f\n",avgWTA);
  fprintf(statistics,"Avg\tWaiting\t=\t%f\n",avgWAITING);
  fprintf(statistics,"Std\tWTA\t=\t%f\n",stdWTA);*/
      fclose(fout);
    fclose(statistics);
}
int main(int argc, char * argv[]){
  printf("Scheduler Started\n");
  initClk();
  signal(SIGUSR1,stoploop);
  signal(SIGUSR2,finishHandler);// THE SIGNAL THE PROCESS CALLS WHEN IT FINISHE
  signal(SIGCONT,msgArrival);
  finished=false;
  noMessages=0;
    //printf("Before clock %d\n",atoi(argv[1]));
    //printf("After clock2 %d\n",atoi(argv[0]));
    //schedTech technique=HPF;
    //printf("After clock3 %d\n",atoi(argv[0]));
    //printf("nenenenenen %d",atoi(argv[0]));
    hpfSchedule();
 /*switch(technique){
      case HPF:
      printf("hpf starts");
      hpfSchedule();
      printf("hpf ends");
      break;

    }*/
    //int tech=0;
    //if(tech==0){}
    // after finishing report ang ouput the file
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    destroyClk(true);
}
void finishHandler(int signum){
  int stat_loc;
  int sid=wait(&stat_loc);
if(!(stat_loc& 0x00FF)){
    currentProcess->saved->pState=finished;
    fprintf(fout,"At\ttime\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%d\n",timeStep,currentProcess->id,currentProcess->saved->arrivalTime,currentProcess->saved->executionTime,currentProcess->saved->executionTime,currentProcess->saved->waitingTime,currentProcess->saved->turnArroundTime,currentProcess->saved->weightedTurnArroundTime);
  //clean process resources
}
}
static void stoploop(int signum){
  finished=true;
}
static void msgArrival(int signum){
  noMessages++;
}
