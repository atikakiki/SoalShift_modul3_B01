#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <pthread.h> //library thread
#define SHM_KEY 7777

int *shop_stock;

int kbhit(){
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

void *menu(){
    while(1){
        system("clear");
        printf("Shop\nFood stock : %d\n", *shop_stock);
        puts("Choices\n1. Restock\n2. Back\n");
        sleep(1);
    }
}

void keyHandler(){
    while(!kbhit());         //block until key is pressed
    char ch = getchar();
    
    switch (ch)
    {
        case '1':
            *shop_stock = *shop_stock + 1;
            break;
        case '2':
            exit(EXIT_SUCCESS);
            break;
    
        default:
            break;
    }
}

void createThread(pthread_t menuhdlr){
    int thstat[1] = {0};
    thstat[0] = pthread_create( &menuhdlr, NULL, menu, NULL);
    if(thstat[0]){
        fprintf(stderr,"Error - pthread_create() return code: %d\n",thstat[0]);
        exit(EXIT_FAILURE);
    }
}

int main(){
    pthread_t menuhdlr;

    key_t key = SHM_KEY;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    shop_stock = shmat(shmid, NULL, 0);

    struct termios info;
    tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
    info.c_lflag &= ~ICANON;      /* disable canonical mode */
    info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
    info.c_cc[VTIME] = 0;         /* no timeout */
    tcsetattr(0, TCSANOW, &info);

    createThread(menuhdlr);

    while(1)
        keyHandler();

    tcgetattr(0, &info);
    info.c_lflag |= ICANON;
    tcsetattr(0, TCSANOW, &info);
    exit(EXIT_SUCCESS);
}