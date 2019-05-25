// standard utility
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// socket and connections
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// file management
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

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
    if (argc != 3)
    {
        printf("Invalid number of command line parameters...Aborting\n");
        return -1;
    }

    // read IP address
    char *IP_addr = argv[1];

    // read port no. and convert to integer
    char *port = argv[2];
    unsigned int portno = toInt(port);
    if (portno == -1 || portno > 65535)
    {
        printf("Invalid port number...Aborting\n");
        return -1;
    }

    // create socket for server
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // specify address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = ntohs(portno);
    inet_aton(IP_addr, &address.sin_addr.s_addr);

    // connect to server
    if (connect(client_sock, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        printf("Error connecting server...Aborting\n");
        return -1;
    }

    int sizeno;
    char command[50], output[1 << 15], filename[50], size[10], *buff;
    FILE *fp;
    struct stat obj;

    while (1)
    {
        // zero out existing data
        bzero(command, sizeof(command));
        bzero(output, sizeof(output));
        bzero(filename, sizeof(filename));
        bzero(size, sizeof(size));
        fflush(stdin);
        fflush(stdout);
        printf("$");
        scanf("%s", command);

        if (!strcmp(command, "s_ls") || !strcmp(command, "s_pwd"))
        {
            // do server ls / pwd
            send(client_sock, command, sizeof(command), 0);
            recv(client_sock, &output, sizeof(output), 0);
            printf("%s", output);
        }

        else if (!strcmp(command, "c_ls"))
            system("ls");
        
        else if (!strcmp(command, "c_pwd"))
            system("pwd");

        else if (!strncmp(command, "c_cd", 4))
        {
            // do client cd
            getFileName(filename, command, 5);
            printf("%s\n", filename);
            if (chdir(filename) == 0)
                printf("No such directory: %s\n", filename);
            else
                printf("Inside: %s\n", filename);
        }

        else if (!strncmp(command, "s_cd", 4))
        {
            // do server cd
            getFileName(filename, command, 5);
            send(client_sock, command, sizeof(command), 0);
            recv(client_sock, &output, sizeof(output), 0);
            if (output[0] == '0')
                printf("No such directory: %s\n", filename);
            else if (output[0] == '1')
                printf("Inside: %s\n", filename);
        }

        else if (!strncmp(command, "put", 3))
        {
            // send file
            getFileName(filename, command, 4);
            send(client_sock, command, sizeof(command), 0);
            fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("No such file: %s\n", filename);
                fclose(fp);
                continue;
            }

            stat(filename, &obj);
            sizeno = obj.st_size;
            toChar(sizeno, size);
            send(client_sock, size, sizeof(size), 0);

            sendfile(client_sock, fp, NULL, sizeno);
            printf("File sent: %s\n", filename);
            fclose(fp);
        }

        else if (!strncmp(command, "get", 3))
        {
            // recieve file
            getFileName(filename, command, 4);
            send(client_sock, command, sizeof(command), 0);

            recv(client_sock, &size, sizeof(size), 0);

            if (!strcmp(size, "-1"))
            {
                printf("File not found at server: %s\n", filename);
                continue;
            }

            fp = fopen(filename, "w");

            sizeno = toInt(size);
            buff = malloc(sizeno);

            recv(client_sock, &buff, sizeno, 0);
            fwrite(buff, sizeof(char), sizeof(buff) / sizeof(char), fp);
            fclose(fp);
        }

        else if (!strcmp(command, "quit"))
            break;
        else
            printf("Unknown command!\n");
    }
    return 0;
}