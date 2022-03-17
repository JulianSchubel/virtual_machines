#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

/* *nix */
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

/* Architecture definitions */
#include "./include/main_memory.h"
#include "./include/registers.h"
#include "./include/opcodes.h"
#include "./include/condition_flags.h"

/* Utility functions */
#include "./include/utilities/usage.c"
#include "./include/utilities/switch_endian.c"
#include "./include/utilities/read_image_file.c"

uint16_t check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

//Set up terminal input
struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

//Interrupt handling
void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}


int main(int argc, char * argv[])
{
	signal(SIGINT, handle_interrupt);
	disable_input_buffering();
	//Check that there is at least one argument
	if(argc < 2)
	{
		usage(argc);	
	}		

	for( int i = 1; i < argc; ++i )
	{
		if( !read_image(argv[i]) )
		{
			printf("Failed to load image: %s\n", argv[i]);
			restore_input_buffering();
			exit(1);
		}	
	}

	/* Exactly one condition flag should be set at any given time */
	registers[R_COND] = FL_ZERO;


	restore_input_buffering();
	return 0;
}
