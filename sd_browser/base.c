/*
 * base.c
 *
 *  Created on: 3.01.2009
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

#include <stdint.h>
#include <stdio.h>
#include <conio.h>
#include "base.h"

uint8_t waitKey(void)
{
	revers(1);
	textcolor(COLOR_VIOLET);
	cputs("PRESS A KEY");
	revers(0);
	return cgetc();
}
