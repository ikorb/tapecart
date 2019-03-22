/*
 * dir.c
 *
 *  Created on: 10.01.2009
 *      Author: bader
 *
 * DraCopy (dc*) is a simple copy program.
 * DraBrowse (db*) is a simple file browser.
 *
 * Since both programs make use of kernal routines they shall
 * be able to work with most file oriented IEC devices.
 *
 * Created 2009 by Sascha Bader
 *
 * The code can be used freely as long as you retain
 * a notice describing original source and author.
 *
 * THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL,
 * BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
 *
 * Newer versions might be available here: http://www.sascha-bader.de/html/code.html
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <6502.h>
#include "dir.h"
#include "defines.h"

/*
 * read a devices directory
 */
void readDir(Directory *dir, char *path)
{
	DirElement *previous = NULL;
	DirElement *current = NULL;

    uint8_t i;
    uint8_t mode = 1;
	register uint8_t xpos = 0;

    // initialize directory
    dir->name[0] = 0;
    dir->no_of_elements = 0;
    dir->selected = 0;

	revers(0);
	gotoxy(DIR1X+1, DIR1Y);
	cputs("                         ");
	gotoxy(DIR1X+1, DIR1Y);
	revers(mode);

    tapecart_open_dir(path, dir->name);

    tapecart_sendbyte(CMD_SD_READ_DIR_FAST);
    tapecart_send_u16(MAX_DIR_ELEMENTS);
    SEI();

	while (dir->no_of_elements < MAX_DIR_ELEMENTS) // TODO: Test limit (off by one?)
	{
		DirElement *current = dir->elements + dir->no_of_elements;
		uint8_t *size_ptr = (uint8_t *)&current->size;

        current->type = tapecart_getbyte_fast();

        *size_ptr++ = tapecart_getbyte_fast();
        *size_ptr++ = tapecart_getbyte_fast();
        *size_ptr++ = tapecart_getbyte_fast();

	    for (i = 0; i < FILENAME_LENGTH; ++i)
        {
            current->name[i] = tapecart_getbyte_fast();
        }

	    if (current->type == FILE_NONE)
	    {
    	    // end of dir
	    	break;
		}
	    else
	    {
			dir->no_of_elements++;
		}

    	if (++xpos < MENUX-1)
    	{
    		cputc(' ');
		}
		else
		{
		    mode = !mode;
		    revers(mode);
		    xpos = 0;
		    gotoxy(DIR1X+1, DIR1Y);
		}
	}

    CLI();
	revers(0);
}
