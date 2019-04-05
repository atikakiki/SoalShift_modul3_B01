#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <pthread.h> //library thread
#define SHM_KEY 7777

char name[80];

/* status */
int pet_hp = 300,
    enemy_hp = 100,
    hunger_pt = 200,
    hygiene_pt = 100,
    fridge = 0,
    *shop_stock = NULL;

//--sce[0] = idle, sce[1] = battle, sce[2] = shop
int sce[3] = {0},
    bathtimeout = 0,
    game_stat = 0;

// simulate conio.h kbhit()
int kbhit(){
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

//display menu
void *menu(){
    //block until game is started
    while(!game_stat);
    sce[0] = 1;

    while(game_stat){
        //clear screen
        system("clear");
        
        if(sce[0]){
            printf("========Standby Mode========\nHealth : %d\nHunger : %d\nHygiene : %d\nFood left : %d\n", pet_hp, hunger_pt, hygiene_pt, fridge);
            if(bathtimeout > 0)
                printf("Bath will be ready in %ds\n", bathtimeout);
            else
                printf("Bath is ready\n");
            puts("Choices:\n1. Eat\n2. Bath\n3. Battle\n4. Shop\n5. Exit");
        }
        else if(sce[1]){
            printf("========Battle Mode========\nMonster’s Health : %d\nEnemy’s Health : %d\n", pet_hp, enemy_hp);
            puts("Choices: \n1. Attack\n2. Run");
        }
        else if(sce[2]){
            printf("========Welcome to shop========\nFood stock : %d\nYour food stock : %d\n", *shop_stock, fridge);
            puts("Choices: \n1. Buy\n2. Back");
        }
        
        bathtimeout--;
        sleep(1);
    }
}

//keyboard handler
void keyHandler(){
   while(!kbhit());         //block until key is pressed

   char ch = getchar();

    //main menu
    if(sce[0]){
        switch (ch){   
           //eat
            case '1':
                if(fridge == 0){
                    printf("\nYou have no food left!\n");
                    sleep(0.3);
                }
                else{
                    printf("\nOm nom nom..\n");
                    hunger_pt += 15;
                    fridge--;
                    sleep(0.2);
                }
                break;
            //bath
            case '2':
                if(bathtimeout > 0){
                    printf("\nBath is not ready just yet.. wait for %ds\n", bathtimeout);
                    sleep(0.3);
                }
                else{
                    printf("\nBathing %s..\n", name);
                    hygiene_pt += 30;
                    bathtimeout = 20;
                    sleep(0.2);
                }
                break;
            //enter the arena
            case '3':
                sce[0] = 0;
                enemy_hp = 100;
                sce[1] = 1;
                break;
            //entering shop
            case '4':
                sce[0] = 0;
                sce[2] = 1;
                break;
            //exit
            case '5':
                exit(EXIT_SUCCESS);
                break;
       
           default:
               break;
       }
   }

    //battle
    else if(sce[1]){
        switch (ch){   
           //attack
            case '1':
                pet_hp -= 20;
                enemy_hp -= 20;

                if(pet_hp <= 0)
                    game_stat = 0;
                else if(enemy_hp <= 0){
                    printf("\nYour %s wins!\n", name);
                    sleep(0.3);
                    sce[1] = 0;
                    sce[0] = 1;
                }
                break;
            //run
            case '2':
                sce[1] = 0;
                sce[0] = 1;
                break;

            default:
               break;
        }
    }

    //shop
    else if(sce[2]){
        switch (ch){   
            //buy
            case '1':
                if(*shop_stock <= 0){
                    printf("\nShop is out of stock!\n");
                    sleep(0.3);
                }
                else{
                    *shop_stock = *shop_stock - 1;
                    fridge++;
                }
                
            //back
            case '2':
                sce[2] = 0;
                sce[0] = 1;
                break;

            default:
               break;
        }
    }
}

//hunger --sce[1]
void *hunger(){
    while(game_stat){
        while(!sce[0]);
        
        sleep(10);
        
        hunger_pt -= 5;
        //game over
        if(hunger_pt <= 0)
            game_stat = 0;
    }
}

void *hygiene(){
    while(game_stat){
        while(!sce[0]);
        
        sleep(30);
        
        hygiene_pt -= 10;
        //game over
        if(hygiene_pt <= 0)
            game_stat = 0;
    }
}

void *hpregen(){
    while(game_stat){
        while(!sce[0]);
        sleep(10);
        pet_hp += 5;
    }
}

void createThread(pthread_t menuhdlr, pthread_t reg, pthread_t hgr, pthread_t hyg){
    int thstat[4] = {0};
    thstat[0] = pthread_create( &menuhdlr, NULL, menu, NULL);
    if(thstat[0]){
        fprintf(stderr,"Error - pthread_create() return code: %d\n",thstat[0]);
        exit(EXIT_FAILURE);
    }

    thstat[1] = pthread_create( &reg, NULL, hpregen, NULL);
    if(thstat[1]){
        fprintf(stderr,"Error - pthread_create() return code: %d\n",thstat[1]);
        exit(EXIT_FAILURE);
    }

    thstat[2] = pthread_create( &hgr, NULL, hunger, NULL);
    if(thstat[2]){
        fprintf(stderr,"Error - pthread_create() return code: %d\n",thstat[2]);
        exit(EXIT_FAILURE);
    }

    thstat[3] = pthread_create( &hyg, NULL, hygiene, NULL);
    if(thstat[3]){
        fprintf(stderr,"Error - pthread_create() return code: %d\n",thstat[3]);
        exit(EXIT_FAILURE);
    }
}

//  void battle(){
//
//  }

int main(){
    pthread_t menuhdlr, reg, hgr, hyg;

    key_t key = SHM_KEY;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    shop_stock = shmat(shmid, NULL, 0);

    printf("Name your pet: ");
    gets(name);

    struct termios info;
    tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
    info.c_lflag &= ~ICANON;      /* disable canonical mode */
    info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
    info.c_cc[VTIME] = 0;         /* no timeout */
    tcsetattr(0, TCSANOW, &info);
   
    //start on standby
    createThread(menuhdlr, reg, hgr, hyg);
    game_stat = 1;

    //start keyboard handler on main program
    while(game_stat)
        keyHandler();

    if(!game_stat){
        system("clear");
        printf("======GAME OVER======\nYour %s has died from ", name);

        if(hunger_pt <= 0)
            printf("malnutrition\n");
        else if(hygiene_pt <= 0)
            printf("illness\n");
        else if(pet_hp <= 0)
            printf("fatal injury\n");
    }
    
    //set the terminal back into canonical mode
    tcgetattr(0, &info);
    info.c_lflag |= ICANON;
    tcsetattr(0, TCSANOW, &info);
    
    exit(EXIT_SUCCESS);
}

