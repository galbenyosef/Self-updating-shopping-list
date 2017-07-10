/*
 * reads from shared memory
 * by given 'categorial'
 * parameters
 * also prints results
 */
#include <stdlib.h>

#include "board.h"


void listdate();
void printdate();

int main(int argc, char** argv) {

    if (2 > argc || argc > 3){
        perror("args");
        exit (EXIT_FAILURE);
    }

    listdate(argc,argv);
    
}

void printdate(board_ent_t next_entry){
    char fixedtxt[LC_TEXT_LEN];
    strncpy(fixedtxt,next_entry.text,LC_TEXT_LEN);
    fixedtxt[LC_TEXT_LEN-1]='\0';
    printf("%02d/%02d/%04d %s\n", next_entry.date.day, next_entry.date.month, next_entry.date.year,fixedtxt );
    sleep(1);
}

void listdate(int params_count, char** params){

    char entry_date[B_DATE_LEN];
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
    printf("Listing dates");
        for (cur_param = 1; cur_param < params_count; cur_param++){
            if (cur_param > 1)
                    printf(" up to %s",params[cur_param]);
            else
                printf(" %s",params[cur_param]);
        }
    printf("\n");
    
    cur_board_entry=0;
    
    pid=fork();
    if (pid < 0){
        perror("fork");
        return;
    }
    else if (pid==0){
        char ch;
        read(STDIN_FILENO,&ch,1);
        board->flag &= ~(B_DATE_FLAG);
        return;
    }
    
    board->flag |= B_DATE_FLAG;
    
    while(board->flag & B_DATE_FLAG){
        //after 100 entries has been read, print out
        if (cur_board_entry == B_ENT_CNT){
            //prepare for next board iteration
            cur_board_entry=0;
            sleep(5);
        }
        //read next entry
        next_entry=board->entries[cur_board_entry++];
        //if entry's category is one of params
        
        int entry_day = next_entry.date.day;
        int entry_month = next_entry.date.month;
        int entry_year = next_entry.date.year;

        int start_day,end_day,start_month,end_month,start_year,end_year;

        strncpy(entry_date,"\0\0\0\0",4);
        
        start_day = atoi(strncpy(entry_date,params[1],2));
        start_month = atoi(strncpy(entry_date,params[1]+3,2));
        start_year = atoi(strncpy(entry_date,params[1]+6,4));

        strncpy(entry_date,"\0\0\0\0",4);
        
        if (params_count > 2){
            end_day = atoi(strncpy(entry_date,params[2],2));
            end_month = atoi(strncpy(entry_date,params[2]+3,2));
            end_year = atoi(strncpy(entry_date,params[2]+6,4));
            
            if (start_year == end_year){
                if (start_month == end_month){
                    if (entry_day >= start_day && entry_day <= end_day ){
                        printdate(next_entry);
                    }
                }
                else if (start_month < end_month){
                    if (entry_month == start_month && entry_day >= start_day){
                        printdate(next_entry);
                    }
                    else if (entry_month == end_month && entry_day <= end_day){
                        printdate(next_entry);
                    }
                    else if (entry_month > start_month && entry_month < end_month){
                        printdate(next_entry);
                    }
                }
            }
            else if (start_year < end_year){
                
                if (entry_month == start_month && entry_day >= start_day){
                    printdate(next_entry);
                }
                else if (entry_month == end_month && entry_day <= end_day){
                    printdate(next_entry);
                }
                else if (entry_year == start_year && entry_month > start_month){
                    
                }
                else if (entry_year == end_year && entry_month < end_month){
                    printdate(next_entry);
                }
            }     
        }
        else if (params_count ==2){
            if (entry_year >= start_year && 
                    entry_day >= start_day &&
                    entry_month >= start_month){
                printdate(next_entry);
            }
            
        }
    }
    
    if (shmdt(board)== -1){
        perror ("board shmdt");
        exit(EXIT_FAILURE);
    }
}
