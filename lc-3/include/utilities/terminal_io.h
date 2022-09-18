#ifndef TERMINAL_IO_H
#define TERMINAL_IO_H

#include <sys/termios.h>

/* Disable canonical (lin-by-line) input and echoing of input */
void disable_input_buffering();
/* Restore original terminal configuration */
void restore_input_buffering();
/* Callback function for signal handling */
void handle_interrupt(int signal);

/* Structure to save original terminal configuration */
struct termios original_tio;

void disable_input_buffering()
{
    /* Save current terminal configuration */
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    /*  
     *  ICANON: canonical input (line-by-line)
     *  ECHO: echo input
     */
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    /* TCSANOW: apply immediately */
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

/* Restore terminal settings */
void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

//Interrupt handling: Call restore_input_buffering on interrupt
void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

#endif //TERMINAL_IO_H
