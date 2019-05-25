#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

//create server socket
int srv_sock;

void sigintHandler(int sig_num){
    close(srv_sock);
    exit(0);
}

int main(){
    srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    //specify address
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(8080);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    //bind server socket
    if(bind(srv_sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr))==-1){
        printf("Error binding address\n");
        return -1;
    }
    if(listen(srv_sock, 5)==-1){
        printf("Error in listen function\n");
        return 0;
    }

    //read html file to serve
    FILE *html_file = fopen("index.html", "r");
    if (html_file==NULL){
        printf("File not found!\n");
        return -1;
    }
    char html[1024];
    fgets(html, sizeof(html), html_file);

    char resp_data[2048] = "HTTP/1.0 200 OK\r\n\n";
    strcat(resp_data, html);

    int client;
    while(1){
        client = accept(srv_sock, NULL, NULL);
        send(client, resp_data, sizeof(resp_data), 0);
        close(client);
        signal(SIGINT, sigintHandler);
    }
    return 0;
}