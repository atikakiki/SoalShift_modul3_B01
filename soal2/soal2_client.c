#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PORT_JUAL 7777
#define PORT_BELI 8888
#define SHM_KEY 420420

int *stock;

int disp_opt(int input, int socket){
    char buffer[256];
    int op, res, readval;

    //system("clear")
    
    puts("What do you want to do?");
    if(input == 1)
        puts("1. Tambah");
    else if(input == 2)
        puts("1. Beli");
    puts("2. Close");
    
    scanf("%d", &op);

    switch (op)
    {
        case 1:
            if(input == 1)
                strcpy(buffer, "tambah");
            else if(input == 2)
                strcpy(buffer, "beli");
            res = 1;
            break;
        case 2:
            strcpy(buffer, "");
            exit(EXIT_SUCCESS);
            break;
        default:
            puts("Wrong input!\n");
            return 1;
            break;
    }
    //send message to connected server
    send(socket, buffer, strlen(buffer), 0);
    
    memset(buffer, 0, 256);
    readval = read(socket, buffer, 256);
    printf("%s\n",buffer);
    
    return 1;
}

int main() {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    int port, input;
    // char *hello = "Hello from client";

    char buffer[1024] = {0};

    printf("\nWhere do you want to connect?\n1. Server penjual\n2. Server pembeli\n");
    scanf("%d", &input);

    switch (input)
    {
        case 1:
            port = PORT_JUAL;
            break;
        case 2:
            port = PORT_BELI;
            break;
        default:
            puts("Wrong input\n");
            exit(EXIT_FAILURE);
            break;
    }


    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer);
    
    if(strcmp(buffer, "Server refused your request\n") == 0)
        exit(EXIT_FAILURE);

    while(disp_opt(input, sock));

    return 0;
}
