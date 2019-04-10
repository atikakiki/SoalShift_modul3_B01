#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h> //library thread

/* status */
int start = 1,
    wakeUp_status = 0,
    spirit_status = 100;

int stat[2] = {0},          //block thread
    count[2] = {0},         //counter
    isSleep[2] = {0};       //is Sleep    
    
//display menu
void menu(){
    printf("\n============ Menu =============\n");
    printf("1. Display status\n2. Agmal Ayo Bangun\n3. Iraj Ayo Tidur\n");
}

//display status
void displayStat(){
    system("clear");
    printf("========= Status page =========\n");
    printf("Agmal's WakeUp_Status = %d\nIraj's Spirit_Status = %d\n\n", wakeUp_status, spirit_status);
}

//Agmal Ayo Bangun function stat[0]
void *wakes(){

    while(1){

        if(isSleep[0]){
            sleep(10);
            isSleep[0] = 0;
        }
        //block thread until main unblocks it
        
        if(stat[0]){
            wakeUp_status += 15;
            if(count[0] >= 3){
                printf("Fitur Iraj Ayo Tidur disabled for 10s\n");
                isSleep[1] = 1;
                count[0] = 0;       //reset count
            }
            else
                count[0]++;
                
            stat[0] = 0;
        }        

        if(wakeUp_status >= 100){
            //displayStat();
            start = 0;
            printf("Agmal Terbangun, mereka bangun pagi dan berolahraga\n");
            pthread_exit(0);
        }
    }
}

//Iraj Ayo Tidur function stat[1]
void *sleeps(){

    while(1){
        if(isSleep[1]){
            sleep(10);
            isSleep[1] = 0;
        }

        if(stat[1]){
            
            spirit_status -= 20;
            if(count[1] >= 3){
                printf("Fitur Agmal Ayo Bangun disabled for 10s\n");
                isSleep[0] = 1;
                count[1] = 0;       //reset count
            }
            else count[1]++;

            stat[1] = 0;
        }   

        if (spirit_status <= 0){
            //displayStat();
            start = 0;
            printf("Iraj ikut tidur, dan bangun kesiangan bersama Agmal\n");
            pthread_exit(0);
        }
    }
}

int main()
{
    pthread_t thread1, thread2;//inisialisasi awal
    
    int  iret1, iret2, input;

    iret1 = pthread_create( &thread1, NULL, wakes, NULL); //membuat thread pertama
    if(iret1) //jika eror
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
        exit(EXIT_FAILURE);
    }

    iret2 = pthread_create( &thread2, NULL, sleeps, NULL);//membuat thread kedua
    if(iret2)//jika gagal
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
        exit(EXIT_FAILURE);
    }

    while(start){            
        menu();
        if(start)
            scanf("%d", &input);

        switch (input){
            case 1:
                displayStat();
                break;
            case 2:
                if(isSleep[0]) 
                    printf("Agmal Ayo Bangun have been disabled temporarily!\n");
                else
                    stat[0] = 1;
                break;
            case 3:
                if(isSleep[1]) 
                    printf("Iraj Ayo Tidur have been disabled temporarily!\n");
                else
                    stat[1] = 1;
                break;
        
            default:
                printf("Input salah!\n");
                break;
        }
        
    }
    exit(EXIT_SUCCESS);
}
