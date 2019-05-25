#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[]){
    char *filename;
    filename = argv[1];
    FILE *fp;
    fp = fopen(filename, "w");
    //create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    //connect
    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr))==-1){
        printf("Connection failed...\n");
        return -1;
    }
    send(sock, filename, 60, 0);
    char c[1];
    while(recv(sock, &c, sizeof(c), 0))
        fprintf(fp, "%c", c[0]);
    fclose(fp);
    printf("Done.\n");
    close(sock);
    return 0;
}
