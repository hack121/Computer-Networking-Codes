// standard utility
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
// socket and connections
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// file management
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

// declare globally for interrupt signal handling
int server_sock;

void sigintHandler(int sig_num)
{
    close(server_sock);
    printf("\n");
    exit(0);
}

unsigned long long int toInt(char *port)
{
    unsigned long long int num = 0;
    long long int i = 0;
    while (port[i])
    {
        num = num * 10;
        unsigned int x = port[i] - '0';
        if (x < 0 || x > 9)
            return -1;
        num += x;
        i++;
    }
    return num;
}

void toChar(long long int num, char *arr)
{
    int l = ceil(log10(num + 1));
    while (l--)
    {
        arr[l] = '0' + num % 10;
        num /= 10;
    }
}

void getFileName(char filename[], char command[], int offset)
{
    int i = 0;
    while (command[offset + i])
    {
        filename[i] = command[offset + i];
        i++;
    }
}

int main(int argc, char *argv[])
{
    // check valid command line params
    if (argc != 2)
    {
        printf("Invalid number of command line parameters...Aborting\n");
        return -1;
    }

    // read port no. and convert to integer
    char *port = argv[1];
    unsigned int portno = toInt(port);
    if (portno == -1 || portno > 65535)
    {
        printf("Invalid port number...Aborting\n");
        return -1;
    }

    // create socket for server
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    // specify address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = ntohs(portno);
    address.sin_addr.s_addr = INADDR_ANY;

    // bind to port
    if (bind(server_sock, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        printf("Error binding server to port...Aborting\n");
        return -1;
    }

    // start listen
    if (listen(server_sock, 5) == -1)
    {
        printf("Error listening...Aborting\n");
        return -1;
    }

    // some declarations
    int client, sizeno;
    char command[50], output[1 << 15], filename[50], size[10], *buff;
    FILE *fp;
    struct stat obj;

    while (1)
    {
        // accept client connections
        client = accept(server_sock, NULL, NULL);
        while (1)
        {
            // zero out existing data
            bzero(command, sizeof(command));
            bzero(output, sizeof(output));
            bzero(filename, sizeof(filename));
            bzero(size, sizeof(size));

            // recieve a command
            recv(client, &command, sizeof(command), 0);

            if (!strcmp(command, "s_ls"))
            {
                // do server ls
                system("ls > cli_output.txt");
                fp = fopen("cli_output.txt", "r");
                fread(output, 1, sizeof(output), fp);
                fclose(fp);
                send(client, output, sizeof(output), 0);
            }

            else if (!strcmp(command, "s_pwd"))
            {
                // do server pwd
                system("pwd > cli_output.txt");
                fp = fopen("cli_output.txt", "r");
                fread(output, 1, sizeof(output), fp);
                fclose(fp);
                send(client, output, sizeof(output), 0);
            }

            else if (!strncmp(command, "s_cd", 4))
            {
                // do server cd
                getFileName(filename, command, 5);
                if (chdir(filename) == 0)
                    send(client, "0", 1, 0);
                else
                    send(client, "1", 1, 0);
            }

            else if (!strncmp(command, "put", 3))
            {
                // recieve file
                getFileName(filename, command, 4);
                recv(client, &size, sizeof(size), 0);
                sizeno = toInt(size);
                
                fp = fopen(filename, "w");
                buff = malloc(sizeno);

                recv(client, &buff, sizeno, 0);
                fwrite(buff, sizeof(char), sizeof(buff) / sizeof(char), fp);
                fclose(fp);
            }

            else if (!strncmp(command, "get", 3))
            {
                // send file
                getFileName(filename, command, 4);
                fp = fopen(filename, "r");
                if (fp == NULL)
                {
                    send(client, "-1", 2, 0);
                    fclose(fp);
                    continue;
                }

                stat(filename, &obj);
                sizeno = obj.st_size;
                toChar(sizeno, size);
                send(client, size, sizeof(size), 0);
                
                sendfile(client, fp, NULL, sizeno);
                fclose(fp);
            }

            else if (!strcmp(command, "quit"))
            {
                close(client);
                break;
            }
            signal(SIGINT, sigintHandler);
        }
        signal(SIGINT, sigintHandler);
    }
    return 0;
}