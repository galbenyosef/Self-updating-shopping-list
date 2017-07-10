


#include "board.h"

void shmInit();

int main(int argc, char** argv) {

    shmInit();
}

void shmInit(){

    int shmid,file,file_size,cur_board_entry,cur_file_content,cur_copied_content;
    char* file_content;
    char date[B_DATE_LEN];char cat[B_CAT_LEN];char txt[B_TEXT_LEN];
    board_t* board=NULL;
    board_ent_t next_entry;
    pid_t pid;
            
    shmid = shmget( B_KEY,sizeof(board_t), 0644 | IPC_CREAT | IPC_EXCL );
    if (shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    
    if( ( file = open( FILE_NAME, O_RDONLY ) ) == -1 ){
        perror("open\n");
        exit(EXIT_FAILURE);
    }
    
    file_size = (lseek(file,0,SEEK_END));
    file_content = (char*)malloc(sizeof(char)*file_size); 
    
    if (file_content==NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    lseek(file,0,SEEK_SET);
   
    if (read(file,file_content,file_size) != file_size){
        perror("read");
        exit(EXIT_FAILURE);
    }
    
    if (close(file) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }
    
    board = (board_t*)shmat(shmid,NULL,0);

    if (board==NULL){
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    pid=fork();
    if (pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid==0){
        char ch;
        read(STDIN_FILENO,&ch,1);
        board->flag &= ~(B_BOARD_FLAG);
        exit(EXIT_SUCCESS);
    }
    cur_board_entry=cur_file_content=cur_copied_content = 0;
    board->flag |= B_BOARD_FLAG;

    while(board->flag & B_BOARD_FLAG){
        
        if (cur_file_content==file_size)
            cur_file_content=cur_copied_content=0;
        
        for (;cur_file_content<file_size;cur_file_content++){
            if (cur_board_entry == B_ENT_CNT)
                cur_board_entry = 0;
            
            if(file_content[cur_file_content]=='\n'){
                strncpy(date,file_content+cur_copied_content,B_DATE_LEN);
                date[B_DATE_LEN-1]='\0';
                cur_copied_content+=B_DATE_LEN;
                strncpy(cat,file_content+cur_copied_content,B_CAT_LEN);
                cat[B_CAT_LEN-1]='\0';
                cur_copied_content+=B_CAT_LEN;
                strncpy(txt,file_content+cur_copied_content,cur_file_content-cur_copied_content);
                txt[cur_file_content-cur_copied_content-1]='\0';
                cur_copied_content=cur_file_content+1;
                char delimeter[2]="/";

                next_entry.date.day = atoi(strtok(date,delimeter));
                next_entry.date.month = atoi(strtok(NULL,delimeter));
                next_entry.date.year = atoi(strtok(NULL,delimeter));

                strncpy(next_entry.category,cat,B_CAT_LEN);
                strncpy(next_entry.text,txt,B_TEXT_LEN);

                board->entries[cur_board_entry++]=next_entry;
                printf("%d: %02d/%02d/%04d %s %s\n",cur_board_entry,next_entry.date.day,
                        next_entry.date.month,next_entry.date.year,next_entry.category
                        ,next_entry.text);
                cur_file_content++;
                sleep(1);
                break;
            }
        }
    }
    
    if (shmdt(board)== -1){
        perror ("board shmdt");
        exit(EXIT_FAILURE);
    }
    
    if (shmctl( shmid, IPC_RMID, NULL )==-1){
        perror ("shmctl");
        exit(EXIT_FAILURE);
    }
    free(file_content);
    exit(EXIT_SUCCESS);
}