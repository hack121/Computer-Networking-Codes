#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

void sigintHandler(int sig_num){
	printf("Killed\n");
	signal(SIGINT, sigintHandler);
	kill(getpid(), SIGKILL);
	fflush(stdout);
}

void sigabrtHandler(int sig_num){
	exit(0);
}

int main(){
	char s[1000];
	while(1){
		printf("$ ");
		char *line = fgets(s, 1000, stdin);
		if (line==NULL){
			printf("\n");
			exit(0);
		}
		system(s);
		signal(SIGINT, sigintHandler);
		signal(SIGABRT, sigabrtHandler);
	}
	return 0;
}