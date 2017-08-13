/* tapecart - a tape port storage pod for the C64

   Copyright (C) 2013-2017  Ingo Korb <ingo@akana.de>
   All rights reserved.
   Idea by enthusi

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   SUCH DAMAGE.


   conutil.c: a few convenient routines for a text mode user interface

*/

#include <conio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conutil.h"

bool input_aborted;

static unsigned char cputsnxy(const unsigned char xpos, const unsigned char ypos,
                              unsigned char len, const char *str) {
  unsigned char printed = 0;

  gotoxy(xpos, ypos);
  while (len-- && *str) {
    cputc(*str++);
    ++printed;
  }

  return printed;
}

bool read_string(char *buffer, unsigned char maxlen,
                 unsigned char xpos, unsigned char ypos,
                 unsigned char displaylen) {
  unsigned char strpos;
  int screenpos;
  unsigned char i;
  char ch;
  char curs_state;

  curs_state = cursor(1);

  /* display tail part of string */
  /* FIXME: copy-and-paste below in KEY_CLR, */
  /* should be merged, but needs access to many local vars */
  strpos    = strlen(buffer);
  screenpos = strpos;
  if (strpos < displaylen) {
    /* entire string fits with one space to spare */
    cputsxy(xpos, ypos, buffer);
  } else {
    /* check if there should be a free space at the end */
    if (strpos == maxlen) {
      /* no space */
      screenpos = displaylen;
      cputsnxy(xpos, ypos, displaylen, buffer + strpos - displaylen);
    } else {
      /* leave one space */
      screenpos = displaylen - 1;
      cputsnxy(xpos, ypos, displaylen - 1, buffer + strpos - displaylen + 1);
      cputc(' ');
      gotoxy(xpos + screenpos, ypos);
    }
  }

  while (1) {
    ch = cgetc();

    if (isprint(ch)) {
      /* add to string if possible */
      if (strpos < maxlen) {
        buffer[strpos] = ch;
        ++strpos;
        ++screenpos;
        if (screenpos < displaylen || strpos == maxlen) {
          cputc(ch);
        } else {
          /* need to scroll the field */
          screenpos--;
          cputsnxy(xpos, ypos, displaylen, buffer + strpos - screenpos);
          cputc(' ');
          gotoxy(xpos + displaylen - 1, ypos);
        }
      }
    } else {
      switch (ch) {
      case KEY_LEFT:
        if (strpos > 0) {
          --strpos;
          --screenpos;
          if (screenpos < 0) {
            /* at left edge of field, scroll */
            screenpos = 0;
            cputsnxy(xpos, ypos, displaylen, buffer + strpos);
          }
          gotoxy(xpos + screenpos, ypos);
        }
        break;

      case KEY_RIGHT:
        if (strpos < maxlen && buffer[strpos] != 0) {
          ++strpos;
          ++screenpos;
          if (screenpos >= displaylen && strpos < maxlen) {
            /* at right edge, scroll */
            screenpos = displaylen - 1;
            if (cputsnxy(xpos, ypos, displaylen, buffer + strpos - screenpos) < displaylen) {
              cputc(' ');
            }
          }
          gotoxy(xpos + screenpos, ypos);
        }
        break;

      case KEY_RETURN:
        cursor(curs_state);
        input_aborted = false;
        return true;

      case KEY_STOP:
        cursor(curs_state);
        input_aborted = true;
        return false;

      case KEY_DEL:
        if (strpos > 0) {
          /* move remainder of string back one char */
          for (i = strpos; i <= maxlen; ++i) {
            buffer[i-1] = buffer[i];
          }
          --strpos;
          --screenpos;
          if (screenpos < 0)
            screenpos = 0;

          /* re-draw current substring */
          i = cputsnxy(xpos, ypos, displaylen, buffer + strpos - screenpos);
          for (; i < displaylen; ++i)
            cputc(' ');

          gotoxy(xpos + screenpos, ypos);
        }
        break;

      case KEY_HOME:
        strpos = 0;
        screenpos = 0;

        /* re-draw start of string */
        i = cputsnxy(xpos, ypos, displaylen, buffer);
        for (; i < displaylen; ++i)
          cputc(' ');

        gotoxy(xpos, ypos);
        break;

      case KEY_CLR: /* move to end of string */
        /* FIXME: copy-and-paste from init of this function */
        strpos    = strlen(buffer);
        screenpos = strpos;
        if (strpos < displaylen) {
          /* entire string fits with one space to spare */
          cputsxy(xpos, ypos, buffer);
        } else {
          /* check if we need a space at the end */
          if (strpos == maxlen) {
            /* no space */
            screenpos = displaylen;
            cputsnxy(xpos, ypos, displaylen, buffer + strpos - displaylen);
          } else {
            /* leave one space */
            screenpos = displaylen - 1;
            cputsnxy(xpos, ypos, displaylen - 1, buffer + strpos - displaylen + 1);
            cputc(' ');
            gotoxy(xpos + screenpos, ypos);
          }
        }
        break;
      }
    }
  }
}

static char hexbuffer[12];

uint32_t read_uint(uint32_t preset, bool hex, unsigned char width,
                   unsigned char xpos, unsigned char ypos) {
  unsigned char pos;
  char ch;
  char curs_state;

  curs_state = cursor(1);
  if (hex) {
    sprintf(hexbuffer, "$%0*lx", width - 1, preset);
  } else {
    sprintf(hexbuffer, "%0*ld", width, preset);
  }
  cputsxy(xpos, ypos, hexbuffer);
  pos = 1;
  gotoxy(xpos + pos, ypos);

  while (1) {
    ch = cgetc();
    switch (ch) {
    case KEY_DEL:
    case KEY_LEFT:
      if (pos > 0) {
        --pos;
        gotoxy(xpos + pos, ypos);
      }
      break;

    case KEY_RIGHT:
      if (pos < width && hexbuffer[pos] != 0) {
        ++pos;
        gotoxy(xpos + pos, ypos);
      }
      break;

    case KEY_RETURN:
      cursor(curs_state);
      input_aborted = false;
      if (hexbuffer[0] == '$') {
        return strtol(hexbuffer+1, NULL, 16);
      } else {
        return strtol(hexbuffer, NULL, 10);
      }

    case KEY_STOP:
      cursor(curs_state);
      input_aborted = true;
      return 0;

    default:
      if (pos == 0) {
        if (ch == '$' || isdigit(ch)) {
          hexbuffer[0] = ch;
          ++pos;
          cputc(ch);
        }
      } else {
        if ((hexbuffer[0] == '$' && isxdigit(ch)) ||
            (hexbuffer[0] != '$' && isdigit(ch))  ||
            ch == ' ') {
          hexbuffer[pos] = ch;
          ++pos;
          cputc(ch);
        }
      }
      break;

    }
  }
}


unsigned char show_menu(unsigned char count, const char **items, unsigned char sel,
                        unsigned char xpos, unsigned char ypos) {
  unsigned char i;
  char ch;

  while (1) {
    /* print menu */
    for (i = 0; i < count; ++i) {
      revers(i == sel);
      cputsxy(xpos, ypos + i, items[i]);
      revers(0);
    }

    ch = cgetc();

    /* numeric key: direct selection */
    if (ch >= '1' && ch <= '9') {
      i = ch - '0';
      if (i <= count)
        return i-1;
      else continue;
    }

    switch (ch) {
    case KEY_RETURN:
      return sel;

    case KEY_UP:
      if (sel == 0)
        sel = count - 1;
      else
        sel = sel - 1;
      break;

    case KEY_DOWN:
      if (sel >= count - 1)
        sel = 0;
      else
        sel = sel + 1;
      break;
    }
  }
}

void wait_key(void) {
  (void)cgetc();
}

void clear_mainarea(void) {
  unsigned int i;

  for (i = 2; i < STATUS_START - 2; ++i)
    cclearxy(0, i, 40);
}

void clear_mainarea_full(void) {
  unsigned int i;

  for (i = 2; i < STATUS_START; ++i)
    cclearxy(0, i, 40);
}

