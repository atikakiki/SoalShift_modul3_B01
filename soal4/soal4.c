#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#define PATH1 "/home/izzud/Documents/FolderProses1/SimpanProses1.txt"
#define PATH2 "/home/izzud/Documents/FolderProses2/SimpanProses2.txt"

//status index [0] = first thread; [1] = second thread
int status[2]={0};

//Compress .txt
void zipZap(char *path, char *name){
    char buffer[250];
    snprintf(buffer, 250, "chdir %s && zip Kompres%s.zip %s.txt", path, &name[6], name);
    system(buffer);
}

//Extract .txt
void extract(char *path, char *name){
    char buffer[250];
    snprintf(buffer, 250, "chdir %s && unzip Kompres%s.zip", path, &name[6]);
    system(buffer);
}

void *makeFile(void *arg){
    char path[37],cmd[90], name[50], *fName;

    int i = (intptr_t)arg,                  //  0 -> first thread, 1 -> second thread
        next = !i;                          //  the other thread

    if(i)
        fName = PATH2;
    else
        fName = PATH1;
    
    strncpy(path, fName, 35);               //get folder path
    strncpy(name, &fName[36], 13);          //get filename
    // puts(&fName[36]);
    // puts(path);
    // puts(name);
    if(mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO)){
        snprintf(cmd, 62, "touch %s", fName);
        system(cmd);
    }

    status[i] = 1;
    snprintf(cmd, 90, "ps aux | head -11 > %s", fName);
    
    //block until other thread finishes
    while(!status[next]);
    system(cmd);

    status[i] = 0;
    //block until other thread finishes
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

    int i = 0;
    for(i = 0; i<2 ; i++)
        pthread_create(&thd[i], NULL, makeFile, (void *)(intptr_t) i);

    pthread_join(thd[0], NULL);
    pthread_join(thd[1], NULL); 

}