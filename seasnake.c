/***********************************************************************************************************************
*                                                                                                                      *
*  - SNAKE GAME in C -                                                                                                 *
*  by Abigail Fenerty, Nick Sabia, Justin Merville, Jacob Pelletier                                                    *
*  CS 355 Systems Programming final project, Spring 2021.                                                              *
*                                                                                                                      *
*  To run: 1) compile 2) execute compiled code 3) follow prompts                                                       *
*                                                                                                                      *
*  ** TODO: type up rules here **                                                                                      *
***********************************************************************************************************************/

/** headers **/
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define INFO_ROW 1
#define STDIN_FD 0

/** prototypes **/
/* RE: snake pit */
void pit_size();
void init_pit_border();
void update();

/* RE: snake */

/* RE: logic */
char choose_random_direction();

/* RE: Input */
void tty_mode (int action);
void set_settings();
void set_nodelay_mode();
int get_movement_input();
void end_snake(int signum);

/** statics and constants **/
/* window dimensions*/
static int window_row;
static int window_col;

/* game stats */
static int score = 0;
static int snake_len = 3;

/* logic */
static char move_up = 'w';
static char move_down = 's';
static char move_left = 'a';
static char move_right = 'd';

/***********************************************************************************************************************
*  DRIVER CODE                                                                                                         *
***********************************************************************************************************************/
/*
 *  main()
 *  Purpose: initiate and run a game of snake according to the defined rules
 *  Method: use snake pit, snake, trophy, and logic methods to initialize a game of snake
 *  Returns: interactive game on terminal
 */
int main(){
    /* Initialize global random number generator */
    time_t t;
    srand((unsigned) time(NULL));

    /* Manage terminal settings */
    tty_mode(0); // Save original settings
    signal(SIGINT, end_snake); // Revert to original settings on program termination
    signal(SIGQUIT, SIG_IGN);
    set_settings(); // Set terminal settings for the program.
    set_nodelay_mode(); /* I'm not sure if this is , but I'm keeping it here for now just in case. */

    /* set window grid */
    pit_size();                                                             // get/set dimensions of window
    char grid[window_row][window_col];

    /* initialize snake pit */
    init_pit_border(grid);                                                            // initilize pit
    update(grid);

    /* get user input */
    get_movement_input();

    /* TODO: Figure out how to match user input up with updating the snake and snake pit */

    /* logic */

}

/*
 *  MAIN TODO:
 *
 */

/***********************************************************************************************************************
*  SNAKE PIT                                                                                                           *
*  1. draw_pit_border                                                                                                  *
*  2. pit_size                                                                                                         *
*  3. update                                                                                                           *
***********************************************************************************************************************/
/*
 *  1. draw_pit_border()
 *  Purpose: initialize snake pit
 *  Method: matrix of array, populated by nested for loops
 *  Returns: grid matrix printed to terminal
 */
void init_pit_border(char array[window_row-1][window_col-1]){
    /* draw window border */
    for(int i = 0; i < window_row; i++){
        for(int j = 0; j < window_col; j++){
            if (i == 0 || i == window_row-1)
                array[i][j] = 'X';
            else if(j == 0 || j == window_col-1)
                array[i][j] = 'X';
            else
                array[i][j] = ' ';
        }
    }
}

/*
 *  2. pit_size()
 *  Purpose: get number of rows and columns of current terminal window
 *  Method: use ioctl to access terminal attributes
 *  Returns: assign window row and column values to static (global) row and col variables
 *  Reference: Moley Chapter 5, page 164.
 */
void pit_size(){

    /* the address wbuf is the argument to that device controlled function. */
    struct winsize wbuf;

    /*
     *  The ioctl system call provides access to attributes of the device driver (terminal in this case) connected to fd.
     *  TIOCGWINSZ is the function code needed to access this attribute.
     */
    if (ioctl(0, TIOCGWINSZ, &wbuf) != -1){
        /* DEBUG LINE */
        printf("%d rows x %d cols\n", wbuf.ws_row, wbuf.ws_col);
        /* assign global variable values here: */
        window_row = wbuf.ws_row - INFO_ROW;                                   // assign global row. Subtract INFO_ROW to make room for the score line.
        window_col = wbuf.ws_col;                                              // assign global column
    }
}
/*
 *  3. update()
 *  Purpose: print game grid including border, snake, trophies
 *  Method: call update() after user input
 *  Returns: updated game grid
 */
void update(char array[window_row-1][window_col-1]){
    /* draw window border */
    printf("Welcome to Snake\tScore: %d\tUser input here: \n", score);    // name of game, score, space for user inputs
    for(int i = 0; i < window_row; i++){
        for(int j = 0; j < window_col; j++){
            printf("%c", array[i][j]);
        }
    }
}

/*
 *  PIT TODO:
 *  1. update will need parameters for snake and trophies
 */
/***********************************************************************************************************************
*  LOGIC                                                                                                               *
***********************************************************************************************************************/
/*
 *  1. choose_random_direction()
 *  Purpose: return a random direction to start snake in
 *  Method: call update() after user input
 *  Returns: updated game grid
 */
char choose_random_direction(){
    int random_integer = rand() % 4; // DETERMINE RANDOM NUMBER BETWEEN 0 AND 3
    if (random_integer == 0){
        return 'w';
    } else if (random_integer == 1){
        return 's';
    } else if (random_integer == 2){
        return 'a';
    } else if (random_integer == 3){
        return 'd';
    } else {
        return 'x';
    }
}

/*
 *  LOGIC TODO:
 *  1. create a random function, returns char for direction
 */

/***********************************************************************************************************************
*  INPUT                                                                                                               *
***********************************************************************************************************************/

/*
 * Saves the original terminal settings.
 * This is useful for when we want to revert to the original settings when the user exits this program.
 * PARAMS:
 * action is an integer which governs whether the original settings are saved or loaded. 0 means to save, 1 means to load.
 */
void tty_mode (int action) {
    static struct termios original_settings;
    static int original_flags;
    static int stored = 0;
    if (action == 0) {
        // Save the original terminal settings
        tcgetattr(STDIN_FD, &original_settings);
        original_flags = fcntl(STDIN_FD, F_GETFL);
        stored = 1;
    }
    else if (stored) {
        // Restore the original terminal settings
        tcsetattr(STDIN_FD, TCSANOW, &original_settings);
        fcntl(0, F_SETFL, original_flags);
    }
}

/*
 * Set terminal driver settings.
 */
void set_settings() {
    struct termios settings;
    int result = tcgetattr(STDIN_FD, &settings); /* Read values from driver */
    if (result == -1) {
        perror("Unable to get values from stdin via tcgetattr");
        exit(1);
    }
    settings.c_lflag   &= ~ICANON; /* No buffering */
    settings.c_lflag   &= ~ECHO; /* Turn off echo. */
    settings.c_cc[VMIN] = 1; /* get 1 char at a time */
    tcsetattr(STDIN_FD, TCSANOW, &settings);
}

/*
 * Converts I/O into non-blocking mode.
 * Turns on nodelay mode by using fcntl.
 * I'm uncertain if we need this, but I'm keeping the function here just in case.
 */
void set_nodelay_mode() {
    int termflags;
    termflags = fcntl(STDIN_FD, F_GETFL);
    termflags |= O_NDELAY;
    fcntl(STDIN_FD, F_SETFL, termflags);
}

/*
 * Filters out invalid characters, only returning when a valid character is pressed.
*/
int get_valid_input() {
    int c;
    while((c=getchar()) != EOF && strchr("wasd", c) == NULL) { // Get user input using getchar, then skip illegal characters using strchr.
        return c;
    }
}

/*
 * Translates WASD key inputs into integer values.
 * 1  = Up
 * 2  = Left
 * 3  = Down
 * 4  = Right
 */
int get_movement_input() {
    int input;
    while(1) {
        switch(input = tolower(get_valid_input())) {
            case 'w': return 1;
            case 'a': return 2;
            case 's': return 3;
            case 'd': return 4;
        }
    }
}

/*
 * This is the function which runs when the user terminates the program.
 */
void end_snake(int signum) {
    tty_mode(1);
    exit(1);
}