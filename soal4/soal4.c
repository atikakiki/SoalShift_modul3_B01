#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>//13
#define PATH1 "/home/izzud/Documents/FolderProses1/SimpanProses1.txt"
#define PATH2 "/home/izzud/Documents/FolderProses2/SimpanProses2.txt"

typedef struct files{
    int i;
    char fName[47];
}f;

int status[2]={0};

void zipZap(char *path, char *name){
    char buffer[250];
    snprintf(buffer, 250, "chdir %s && zip Kompres%s.zip %s.txt", path, &name[6], name);
    system(buffer);
}

void extract(char *path, char *name){
    char buffer[250];
    snprintf(buffer, 250, "chdir %s && unzip Kompres%s.zip", path, &name[6]);
    system(buffer);
}

void *makeFile(void *arg){
    char path[37],cmd[90], name[50], *fName;
    int i = (intptr_t)arg,
        next = !i;
    if(i)
        fName = PATH2;
    else
        fName = PATH1;
    
    //printf("que?\n");
    strncpy(path, fName, 35);       //get folder path
    strncpy(name, &fName[36], 13);       //get filename
    puts(&fName[36]);
    puts(path);
    puts(name);
    if(mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO)){
        snprintf(cmd, 62, "touch %s", fName);
        system(cmd);
    }

    status[i] = 1;
    //chdir(path);
    snprintf(cmd, 90, "ps aux | head -11 > %s", fName);
    
    while(!status[next]);
    system(cmd);
    // // printf("que?\n");

    status[i] = 0;

    while(status[next]);
    zipZap(path, name);
    remove(fName);

    printf("Menunggu 15 detik untuk mengekstrak kembali\n");
    sleep(15);
    extract(path, name);
    pthread_exit(0);
}

int main(){

    pthread_t thd[2];
    printf("que?\n");
    

    int i = 0;
    for(i = 0; i<2 ; i++)
        pthread_create(&thd[i], NULL, makeFile, (void *)(intptr_t) i);

    //system("ps aux | head -11 > touch.txt");
    pthread_join(thd[0], NULL);
    pthread_join(thd[1], NULL); 

}