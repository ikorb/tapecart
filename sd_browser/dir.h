/*
 * dir.h
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
#ifndef DIR_H_
#define DIR_H_

#include <stdint.h>
#include "tapecartif.h"

typedef enum {
  FILE_NONE = 0x00,
  FILE_DIR,

  FILE_PRG,
  FILE_TCRT,

  FILE_TAP,
  FILE_D64,

  FILE_UNKNOWN = 0xFF
} file_t;

#define MAX_DIR_ELEMENTS 1000

typedef struct direlement {
    char name[FILENAME_LENGTH+1];
    uint32_t size;
    uint8_t type;
} DirElement;

typedef struct {
  char name[FILENAME_LENGTH+1];
  DirElement elements[MAX_DIR_ELEMENTS];
  uint16_t no_of_elements;
  uint16_t selected;
} Directory;

void readDir(Directory *dir, char *path);

#endif /* DIR_H_ */
