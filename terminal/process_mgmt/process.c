#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h> 

void forkAchild(){
    printf("creating forked process\n");
    int p = fork();
    if (p==0){
        printf("Child Process ID: %d\t\tIts Parent ID: %d\n", getpid(), getppid());
        printf("Child is sleeping...\n");
        sleep(2);
        printf("Child awake\n");
        printf("Exiting Child Process\n");
        exit(0);
    }else if (p>0){
        printf("Parent Process ID: %d\n", getpid());
        printf("Parent waiting for child\n");
        wait(NULL);
        printf("Exiting Parent Process\n");
    }else{
        printf("Process fork failed!!!\n");
    }
}

int main(){
    forkAchild();
    return 0;
}