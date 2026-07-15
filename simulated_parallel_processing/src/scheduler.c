#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h> 
#include <string.h>


volatile sig_atomic_t msg_num_core1 = 0;
volatile sig_atomic_t msg_num_core2 = 0;
volatile sig_atomic_t msg_num_core3 = 0;
void sig_handler_core1(int signum){++msg_num_core1;}
void sig_handler_core2(int signum){++msg_num_core2;}
void sig_handler_core3(int signum){++msg_num_core3;}


void no_interrupt_sleep(int sec)
{
    // * advanced sleep which will not be interfered by signals
    struct timespec req, rem;

    req.tv_sec = sec;  // The time to sleep in seconds
    req.tv_nsec = 0; // Additional time to sleep in nanoseconds

    while(nanosleep(&req, &rem) == -1)
        if(errno == EINTR)
            req = rem;
}

int task_creator (int task_id, int max_bits, int write_pipe){
    // generate n (number of bits to extract)
    int n = rand() % max_bits + 1; 

    // create message 
    char buffer[64]; 
    sprintf(buffer, "%d_%d", task_id, n);

    //write message to pipe 
    int nr;
    if((nr=write(write_pipe, buffer, strlen(buffer)+1)) == -1){
        perror("parent failed to write to pipe");
        exit(1);
    }
    return 0;
}

unsigned int execute_task(int id, int bits){
    unsigned int task_id = (unsigned int) id;
    unsigned mask = (1 << bits) - 1; 
    unsigned int result = task_id & mask;
    return result;
}

int task_handler (int read_pipe, int write_pipe, int signum){
    char buffer[64];
    int nr; 
    int task_id, n;

    //read 
    while((nr=read(read_pipe, buffer, sizeof(buffer)-1))>0){
        // parse and execute
        buffer[nr] = '\0';
        sscanf(buffer, "%d_%d", &task_id, &n);
        printf("[LOG] Core (PID %d): Received task %d with n=%d.\n", getpid(), task_id, n);
        no_interrupt_sleep(1);
        unsigned int result = execute_task(task_id, n);
        printf("[LOG] Core (PID %d): Completed task %d, result=%u. Sending to main.\n", getpid(), task_id, result);

        // signal parent
        kill(getppid(), signum);
        printf("[LOG] Core (PID %d): Sending signal to main process.\n", getpid());

        //write back to parent
        if (write(write_pipe, &result, sizeof(result)) == -1){
            perror("child failed to write to pipe");
            exit(1);
        }
       
    }
    if(nr == -1){
        perror("child failed to read from pipe");
        exit(1);
    }
    
    return 0;
}


int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Invalid input\n");
        return 1;
    }

    int total_tasks = atoi(argv[1]);
    int max_bits = atoi(argv[2]);

    if(total_tasks <= 0 || max_bits <= 0){
        printf("Invalid input\n");
        return 1;
    }

    printf("Total number of tasks: %d\n", total_tasks);
    printf("Maximum number of bits for each task: %d\n", max_bits);

    srand(time(NULL));

    int main_to_core[3][2];
    int core_to_main[3][2];
    for (int i = 0; i < 3; i++){
        pipe(main_to_core[i]);
        pipe(core_to_main[i]);
    }

    int sig_core1=SIGRTMAX;
    int sig_core2=SIGRTMAX-1;
    int sig_core3=SIGRTMAX-2;

    // set up signal handlers
    struct sigaction sa1, sa2, sa3; 
    sa1.sa_handler = sig_handler_core1;
    sa2.sa_handler = sig_handler_core2;
    sa3.sa_handler = sig_handler_core3;
    sigemptyset(&sa1.sa_mask);
    sigemptyset(&sa2.sa_mask);
    sigemptyset(&sa3.sa_mask);
    sa1.sa_flags = 0;
    sa2.sa_flags = 0;
    sa3.sa_flags = 0;
    sigaction(sig_core1, &sa1, NULL);
    sigaction(sig_core2, &sa2, NULL);
    sigaction(sig_core3, &sa3, NULL);

    //child 1
    pid_t pid1 = fork(); 
    printf("[LOG] Main process: Created Core 1 subprocess (PID %d).\n", pid1);
    if (pid1 == 0){    
        close(main_to_core[0][1]);
        close(core_to_main[0][0]);
        // close all ends of core 2 and 3 pipes 
        for(int i = 1; i < 3; i++){
            close(main_to_core[i][0]); close(main_to_core[i][1]);
            close(core_to_main[i][0]); close(core_to_main[i][1]);
        }
        printf("[LOG] Core 1 (PID %d): Closed main_to_core[0] write end and unused pipe ends.\n", getpid());
        task_handler(main_to_core[0][0], core_to_main[0][1], sig_core1);
        printf("[LOG] Core 1 (PID %d): Finished all tasks, exiting.\n", getpid());
        exit(0);
    }

    // child 2 
    pid_t pid2 = fork();
    printf("[LOG] Main process: Created Core 2 subprocess (PID %d).\n", pid2);
    if (pid2 == 0){
        close(main_to_core[1][1]);
        close(core_to_main[1][0]);
        // close all ends of core 1 and 3 pipes
        for(int i = 0; i < 3; i++){
            if (i != 1){
                close(main_to_core[i][0]); close(main_to_core[i][1]);
                close(core_to_main[i][0]); close(core_to_main[i][1]);
            }
        }
        printf("[LOG] Core 2 (PID %d): Closed main_to_core[1] write end and unused pipe ends.\n", getpid());
        task_handler(main_to_core[1][0], core_to_main[1][1], sig_core2);
        printf("[LOG] Core 2 (PID %d): Finished all tasks, exiting.\n", getpid());
        exit(0);
    }

    // child 3
    pid_t pid3 = fork();
    printf("[LOG] Main process: Created Core 3 subprocess (PID %d).\n", pid3);
    if (pid3 == 0){
        close(main_to_core[2][1]);
        close(core_to_main[2][0]);
        // close all ends of core 1 and 2 pipes
        for(int i = 0; i < 3; i++){
            if (i != 2){
                close(main_to_core[i][0]); close(main_to_core[i][1]);
                close(core_to_main[i][0]); close(core_to_main[i][1]);
            }
        }
        printf("[LOG] Core 3 (PID %d): Closed main_to_core[2] write end and unused pipe ends.\n", getpid());
        task_handler(main_to_core[2][0], core_to_main[2][1], sig_core3);
        printf("[LOG] Core 3 (PID %d): Finished all tasks, exiting.\n", getpid());
        exit(0);
    }

    // parent process 
    for (int i = 0; i < 3; i++){
        close(main_to_core[i][0]);
        close(core_to_main[i][1]);
    }
    printf("[LOG] Main process: Closed unused pipe ends.\n");

    unsigned int core_results[3][total_tasks]; // store results for each core
    int core_results_count[3] = {0, 0, 0}; // how many tasks each core has completed


    // comunicate 
    int write_pipes[3] = {main_to_core[0][1], main_to_core[1][1], main_to_core[2][1]};
    int read_pipes[3] = {core_to_main[0][0], core_to_main[1][0], core_to_main[2][0]};
    int core_idle[3] = {1, 1, 1};
    int tasks_sent = 0; 
    int tasks_completed = 0;
    
    while (tasks_completed < total_tasks){
        // assign tasks to idle cores 
        for (int i = 0; i < 3; i++){
            if (core_idle[i] && tasks_sent < total_tasks){
                printf("[LOG] Main process: Assigning task %d to Core %d.\n", tasks_sent, i + 1);
                task_creator(tasks_sent, max_bits, write_pipes[i]);
                core_idle[i] = 0;
                tasks_sent++;
            }
        }
        // check for completed tasks 
        if(msg_num_core1 > 0){
            unsigned int result;
            if(read(read_pipes[0], &result, sizeof(result)) == -1){
                perror("parent failed to read from pipe");
                exit(1);
            }
            --msg_num_core1;           
            core_results[0][core_results_count[0]] = result; // aggregate results for core 1
            core_results_count[0]++;          
            core_idle[0] = 1;
            tasks_completed++;
            printf("[LOG] Main process: Received result %u from Core 1.\n", result);
        }

        if(msg_num_core2 > 0){
            unsigned int result;
            if(read(read_pipes[1], &result, sizeof(result)) == -1){
                perror("parent failed to read from pipe");
                exit(1);
            }
            --msg_num_core2;
            core_results[1][core_results_count[1]] = result; // aggregate results for core 2
            core_results_count[1]++;
            core_idle[1] = 1;
            tasks_completed++;
            printf("[LOG] Main process: Received result %u from Core 2.\n", result);
        }

        if(msg_num_core3 > 0){
            unsigned int result;
            if(read(read_pipes[2], &result, sizeof(result)) == -1){
                perror("parent failed to read from pipe");
                exit(1);
            }
            --msg_num_core3;
            core_results[2][core_results_count[2]] = result; // aggregate results for core 3
            core_results_count[2]++;
            core_idle[2] = 1;
            tasks_completed++;
            printf("[LOG] Main process: Received result %u from Core 3.\n", result);
        }  
    }



    // clean up 
    for(int i = 0; i<3; i++){
        close(write_pipes[i]);
        close(read_pipes[i]);
    }
    printf("[LOG] Main process: All %d tasks completed. Closing pipes and waiting for children.\n", total_tasks);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    printf("[LOG] Main process: All child processes have beene reaped.\n");

    printf("Results:\n");
    for(int i=0; i<3; i++){
        printf("Core %d:", i+1);
        for(int j=0; j<core_results_count[i]; j++){
            printf(" %u", core_results[i][j]);
            if (j < core_results_count[i] - 1) printf(",");
        }
        printf("\n");
    }

    return 0; 
}
    