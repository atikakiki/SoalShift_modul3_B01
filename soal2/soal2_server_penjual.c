#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#define PORT_JUAL 7777
#define SHM_KEY 420420

int *stock;
int conn_count=0;

void *server_handler(void* fd_ptr){
    printf("Server handler started\n");

    int socket = *(int *)fd_ptr;
    int read_size,
        write_size;
    // char *server_msg = "Hello there!\n";

    //message buffer
    static char client_msg[2048]={0};
    char serv_msg[256];

    // static int sent = 0;

    // if(sent < 1)
    //     sent++;

    //receive message from client(s)
    while((read_size = recv(socket, client_msg, 2048, 0) > 0)){
        // printf("Read size: %d \n", read_size);
        // write(socket, client_msg, strlen(client_msg));

        if(strcmp(client_msg, "tambah") == 0){
            *stock = *stock + 1;

            strcpy(serv_msg, "Your request has been accepted\n");   
            write(socket, serv_msg, strlen(serv_msg));
        }

        memset(client_msg, 0, 2000);
    }
    
    if(read_size == 0){
        printf("Client disconnected\n");
        
        fflush(stdout);
    }
    else if (read_size == -1){
        perror("recv failed");
    }

    conn_count--;

    free(fd_ptr);
    close(socket);
    pthread_exit(0);
}

void *disp_stock(){
    while(1){
        system("clear");
        puts("====== SERVER PENJUAL ======\n");
        printf("Stock(s) : %d\n", *stock);
        sleep(5);
    }
}

int main(int argc, char const *argv[]) {
    pthread_t menuhdlr;

    key_t key = SHM_KEY;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    stock = shmat(shmid, NULL, 0);


    int disp = pthread_create( &menuhdlr, NULL, disp_stock, NULL);
    if(disp){
        fprintf(stderr,"Error - pthread_create() return code: %d\n", disp);
        exit(EXIT_FAILURE);
    }

    int server_fd,              //server
        connfd,                 //client 
        *new_socket;
    char buff[2048];
    struct sockaddr_in srvr_addr, cli_addr;
    int opt = 1;

    socklen_t client_len;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    } 
    else{
        printf("Socket created\n");
    }

    bzero(&srvr_addr, sizeof(srvr_addr));
    
    srvr_addr.sin_family = AF_INET;
    srvr_addr.sin_addr.s_addr = INADDR_ANY;
    srvr_addr.sin_port = htons( PORT_JUAL );

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    if (bind(server_fd, (struct sockaddr *)&srvr_addr, sizeof(srvr_addr))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("bind success\n");
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection..\n");
    client_len = sizeof(cli_addr);

    while ((connfd = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t*)&client_len))){
        if(conn_count < 1){
            printf("Connection accepted\n");

            pthread_t srvr_thread;
            new_socket = malloc(1);
            *new_socket = connfd;

            //notify client
            strcpy(buff, "Your connection has been accepted\n");   
            write(connfd, buff, strlen(buff));
            
            //start handler
            pthread_create(&srvr_thread, NULL, server_handler, (void *) new_socket);
            conn_count++;
        }
        else{
            printf("Connection refused, too many connection at a time\n");
            strcpy(buff, "Server refused your request\n");   
            write(connfd, buff, strlen(buff));
            close(connfd);
        }

    }

    if(connfd < 0){
        perror("Accept failed\n");
        exit(EXIT_FAILURE);
    }

    // printf("hey\n");
    
    return 0;
}
