#ifndef BOARD_H
#define BOARD_H
/**
 *  common definitions for bulletin board
 */

#define B_KEY      5551 	/* common key for shared memory */
#define B_ENT_CNT  100		/* number of entries in board */

#define B_DATE_LEN 12 		/* length of the date area in input */
#define B_CAT_LEN  18 		/* length of 'category' field in input */
#define B_TEXT_LEN 200		/* max length of 'text' field in input */

#define LC_TEXT_LEN 70 /* for listcat 'text' field print */

#define FILE_NAME "classified.txt"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>


/**
 * A structure for describing a date.
 */
typedef struct 
{
    int  day;
    int  month;
    int  year;
} date_t;    

/**
 * The structure of each classified entry in the shared memory. 
 * The board consistes of an array of such entries.
 */
typedef struct board_ent
{
    date_t date;
    char   category[B_CAT_LEN + 1];
    char   text[B_TEXT_LEN + 1];
} board_ent_t;


/**
 * values for the 'flag' field: each one notifies the associated application
 * it should terminate.
 */
#define B_BOARD_FLAG 01		/* controls main bulletin board app */
#define B_CAT_FLAG   02		/* controls category selection app */
#define B_DATE_FLAG  04		/* controls date selection app */

/**
 * board structure: the organization of the shared memory block
 */
typedef struct board
{
    int flag;
    board_ent_t entries[B_ENT_CNT];
    
} board_t;

void shmInit();

#endif