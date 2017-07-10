/*
 * reads from shared memory
 * by given 'categorial'
 * parameters
 * also prints results
 */
#include <stdlib.h>

#include "board.h"


void listcat();

int main(int argc, char** argv) {
    
    if (argc < 2){
        perror("args");
        exit (EXIT_FAILURE);
    }

    
    listcat(argc,argv);
    
}

void listcat(int params_count, char** params){

    char fixedtxt[LC_TEXT_LEN];
    int shmid,cur_board_entry,cur_param;
    board_t* board=NULL;
    board_ent_t next_entry;
    pid_t pid;
    
    //get id of shared memory allocated by board
    //for READ ONLY
    //and attach board;
    shmid = shmget( B_KEY,sizeof(board_t), SHM_RDONLY );
    if (shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    board = (board_t*)shmat(shmid,NULL,0);
    if (board==NULL){
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    //title
    printf("Listing categories");
        for (cur_param = 1; cur_param < params_count; cur_param++){
            printf(" %s",params[cur_param]);
    }
    printf("\n");
    
    //initers
    cur_board_entry=0;
    
    pid=fork();
    if (pid < 0){
        perror("fork");
        return;
    }
    else if (pid==0){
        char ch;
        read(STDIN_FILENO,&ch,1);
        board->flag &= ~(B_CAT_FLAG);
        return;
    }
    
    board->flag |= B_CAT_FLAG;
    
    while(board->flag & B_CAT_FLAG){
        //after 100 entries has been read, print out
        if (cur_board_entry == B_ENT_CNT){
            //prepare for next board iteration
            cur_board_entry=0;
            sleep(5);
            printf("\33[2J\n");
        }
        //read next entry
        next_entry=board->entries[cur_board_entry++];
        //if entry's category is one of params
        char cat[B_CAT_LEN];
        strncpy(cat,next_entry.category,B_CAT_LEN);
        char* catp = strtok(cat," ");
        for (cur_param = 1; cur_param < params_count; cur_param++){
            if (catp!=NULL){
                if (!strcmp(catp,params[cur_param])){
                    //filtered entries contains it
                    strncpy(fixedtxt,next_entry.text,LC_TEXT_LEN);
                    fixedtxt[LC_TEXT_LEN-1]='\0';
                    printf("%s %s\n",next_entry.category,
                            fixedtxt);
                    sleep(1);
                }
            }
            strtok(NULL," ");
        }
        
    }
    if (shmdt(board)== -1){
        perror ("board shmdt");
        exit(EXIT_FAILURE);
    }
}
