#ifndef LC3_READ_IMAGE_H
#define LC3_READ_IMAGE_H

#include <stdio.h>
#include <stdint.h>
#include "switch_endian.c"
#include "../main_memory.h"

/*NOTE: LC3 is a big endian system*/
/*Read an image file and convert the endiannes to big endian*/

void read_image_file(FILE* file)
{
	/* The origin tells us where in memory to place the file/image */
	uint16_t origin;

	/* First 16-bits of program file indicate memory address of program start */
	fread(&origin, sizeof(origin), 1, file);	
	origin = switch_endian(origin);

	/* Maximum file size is known therefore only one fread is required */
	uint16_t max_read = UINT16_MAX - sizeof(origin);	
	/* Pointer to memory offset by origin */
	uint16_t* p = memory + origin;
	/* Read in the remainder of the image file */
	size_t read = fread(p, sizeof(uint16_t), max_read, file);	
	/* Switch the newly read data in memory to big endian */
	while( read-- > 0 )
	{
		*p = switch_endian(*p);
		++p;
	}
}


/*
	Convenience function to wrap read_image_file()
	Opens the image file, checks the success of the opening, calls read_image_file(), closes the file 
	Returns 1 on SUCCESS, 0 on FAILURE. This is to support if( !read_image(x) ){...} statements
*/
int read_image(char * image_path)
{
	FILE* file = fopen(image_path, "rb");
	if( !file )
	{
		return 0;
	}
	read_image_file(file);
	fclose(file);
	return 1;
}

#endif //LC3_READ_IMAGE_H
