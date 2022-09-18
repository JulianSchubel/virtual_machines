#ifndef TERMINAL_IO_H
#define TERMINAL_IO_H

#include <sys/termios.h>

//Set up terminal input
struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    /* Turn off canonical (line-by-line) input, and echoing of input characters */
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    /* Apply terminal configuration immediately (TCSANOW) */
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
