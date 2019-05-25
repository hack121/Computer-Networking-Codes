#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int server_sock;

void sigintHandler(int sig_num){
    close(server_sock);
    exit(0);
}

int main(){
    FILE *fp;
    //create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    //address
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(8080);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    //bind
    if(bind(server_sock, (struct sockaddr*)&srv_addr, sizeof(srv_addr))==-1){
        printf("Error binding socket to address\n");
        return -1;
    }
    listen(server_sock, 1);
    int client;
    char c[1];
    while(1){
        client = accept(server_sock, NULL, NULL);
        //recv filename
        char filename[60];
        recv(client, &filename, 60, 0);
        printf("Trying to send %s..\n", filename);
        fp = fopen(filename, "r");
        if(fp==NULL){
            send(client, "No such file\n", 13, 0);
        }else{
            printf("Sending file %s...\n", filename);
            while(fscanf(fp, "%c", &c[0])==1)
                send(client, c, sizeof(c), 0);
            printf("Done.\n");
        }
        close(client);
        signal(SIGINT, sigintHandler);
    }
    return 0;
}