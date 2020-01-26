/*
 * Derived from Draco Browser V1.0C 8 Bit (27.12.2009)
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <cbm.h>
#include <errno.h>
#include <joystick.h>
#include "tapecartif.h"
#include "screen.h"
#include "dir.h"
#include "base.h"
#include "defines.h"
#include "loader.h"

/* declarations */
static void about(void);
static void mainLoop(void);
static void updateScreen(void);
static void updateMenu(void);

static void execute(char *prg);
static void changeDir(char *path);
static void clrDir(void);
static void showDir(void);
static void updateDir(uint16_t last_selected);
static void printDir(void);
static void printElement(uint16_t pos);

/* definitions */
static char linebuffer[SCREENW+1];
static Directory *dir = NULL;
static uint8_t textc = COLOR_LIGHTGREEN;
static const char *program = "Tapecart SD Browser v1.2";

#ifdef DIRH
#undef DIRH
#define DIRH 22
#endif

#ifdef MENUX
#undef MENUX
#define MENUX 27
#endif

int main(void)
{
  initScreen(COLOR_BLACK, COLOR_BLACK, textc);
  mainLoop();
  __asm__ ("jmp $fce2");    // Soft reset
  return 0;
}

static void updateScreen(void)
{
    uint8_t menuy=MENUY;
    clrscr();
    textcolor(textc);
    revers(0);
    updateMenu();
    showDir();
}

static void updateMenu(void)
{
    revers(0);
    gotoxy(MENUX+1, MENUY+2);
    cputs(" CR RUN/CD");
    gotoxy(MENUX+1, MENUY+4);
    cputs(" HO ROOT");
    gotoxy(MENUX+1, MENUY+6);
    cputs(" BS DIR UP");
    gotoxy(MENUX+1, MENUY+8);
    cputs(" T  TOP");
    gotoxy(MENUX+1, MENUY+10);
    cputs(" B  BOTTOM");
    gotoxy(MENUX+1, MENUY+12);
    cputs(" .  ABOUT");
    gotoxy(MENUX+1, MENUY+14);
    cputs(" Q  QUIT");

    drawFrame(NULL, MENUX, MENUY, MENUW, MENUH+1);
    revers(1);
    textcolor(COLOR_GREEN);
    gotoxy(0, BOTTOM);
    cprintf("        %s        ", program);
    textcolor(textc);
    revers(0);
}

static void mainLoop(void)
{
    DirElement *current = NULL;
    uint16_t last_selected = 0;
    uint16_t exitflag = 0;
    uint16_t joy_cnt = 0;
    bool joy_down = false;
    uint8_t c, j;

    if (joy_install(&joy_static_stddrv) != JOY_ERR_OK)
    {
        gotoxy(DIR1X, DIR1Y);
        cputs("Error: Failed to install joystick driver.\n");
        gotoxy(DIR1X, DIR1Y+2);
        waitKey();
        return;
    }

    if (!tapecart_cmdmode())
    {
        gotoxy(DIR1X, DIR1Y);
        cputs("Error: Tapecart not detected.\n");
        gotoxy(DIR1X, DIR1Y+2);
        waitKey();
        return;
    }

    dir = (Directory *) malloc(sizeof(Directory));
    memset(dir, 0, sizeof(Directory));

    readDir(dir, ".");
    updateScreen();

    do
    {
        c = kbhit() ? cgetc() : 0;
        if (!c)
        {
            j = joy_read(1);
            c = JOY_UP(j) ? CH_CURS_UP :
                JOY_DOWN(j) ? CH_CURS_DOWN :
                JOY_LEFT(j) ? CH_CURS_LEFT :
                JOY_RIGHT(j) ? CH_CURS_RIGHT :
                JOY_BTN_1(j) ? 13 : 0;

            if (c)
            {
                if (joy_cnt)
                {
                    c = 0;
                }
                else
                {
                    // TODO: Use timer for this
                    joy_cnt = joy_down ? 100 : 1000;
                    joy_down = true;
                }
            }
            else
            {
                joy_down = false;
                joy_cnt = 0;
            }
        }

        if (joy_cnt)
        {
            joy_cnt--;
        }

        switch (c)
          {
            // --- start / enter directory
            case CH_ENTER:
                    if (dir->no_of_elements)
                    {
                        current = dir->elements + dir->selected;
                        if (current->type == FILE_DIR)
                        {
                            changeDir(current->name);
                        }
                        else if (current->type != FILE_UNKNOWN)
                        {
                            execute(current->name);
                        }
                    }
                    break;

            // --- root directory
            case CH_HOME:
                    changeDir("/");
                    break;

            // --- leave directory
            case CH_DEL:
                    changeDir("..");
                    break;

            case 't':
                    if (dir->no_of_elements)
                    {
                        last_selected = dir->selected;
                        dir->selected = 0;
                        updateDir(last_selected);
                    }
                    break;

            case 'b':
                    if (dir->no_of_elements)
                    {
                        last_selected = dir->selected;
                        dir->selected = dir->no_of_elements-1;
                        updateDir(last_selected);
                    }
                    break;

            case CH_CURS_DOWN:
                    if (dir->no_of_elements && (dir->selected+1) < dir->no_of_elements)
                    {
                        last_selected = dir->selected;
                        dir->selected++;
                        updateDir(last_selected);
                    }
                    break;

            case CH_CURS_UP:
                    if (dir->no_of_elements && dir->selected)
                    {
                        last_selected = dir->selected;
                        dir->selected--;
                        updateDir(last_selected);
                    }
                    break;

            case CH_CURS_RIGHT:
                    if (dir->no_of_elements)
                    {
                        last_selected = dir->selected;
                        if (dir->selected + DIRH < dir->no_of_elements)
                        {
                            dir->selected += DIRH;
                        }
                        else
                        {
                            dir->selected = dir->no_of_elements-1;
                        }

                        updateDir(last_selected);
                    }
                    break;

            case CH_CURS_LEFT:
                    if (dir->no_of_elements)
                    {
                        last_selected = dir->selected;
                        if (dir->selected > DIRH)
                        {
                            dir->selected -= DIRH;
                        }
                        else
                        {
                            dir->selected = 0;
                        }

                        updateDir(last_selected);
                    }
                    break;

            case '.':
                    about();
                    break;

            case 'q':
                    exitflag = 1;
                    break;
        }
    }
    while (exitflag == 0);

    free(dir);
    tapecart_streammode();
}

static void changeDir(char *path)
{
    // use buffer as path gets cleared in readDir
    sprintf(linebuffer, "%s", path);
    readDir(dir, linebuffer);

    clrDir();
    dir->selected = 0;
    showDir();
}

static void execute(char *prg)
{
    uint8_t i;

    tapecart_sendbyte(CMD_SD_SELECT_FILE);
    for (i = 0; i < FILENAME_LENGTH; ++i)
    {
        tapecart_sendbyte(prg[i]);
    }

    // install loader
    tapecart_read_loader((uint8_t *)0x0351);

    load_and_run();
    while(true);
}

static void showDir(void)
{
    char *title;
    if (dir->name[0] != NULL)
    {
        title = dir->name;
    }
    else
    {
        title = "  no directory  ";
    }

    sprintf(linebuffer, "> %-16s <", title);

    textcolor(COLOR_WHITE);
    drawFrame(linebuffer, DIR1X, DIR1Y, DIRW+2, DIRH+2);

    textcolor(textc);
    printDir();
}

static void clrDir(void)
{
    clearArea(DIR1X+1, DIR1Y, DIRW, DIRH+1);
}

static void about(void)
{
    clrscr();
    textcolor(COLOR_YELLOW);
    gotoxy(14, 0);
    cputs("Tapecart SD");
    textcolor(COLOR_ORANGE);
    gotoxy(14, 1);
    cputs("\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3\xa3");
    gotoxy(4, 2);
    cputs("github.com/KimJorgensen/tapecart");
    textcolor(COLOR_LIGHTGREEN);
    gotoxy(13, 4);
    cputs("derived from");

    gotoxy(2, 6);
    cputs("tapecart (C) 2013-2017 by Ingo Korb");
    gotoxy(7, 7);
    cputs("github.com/ikorb/tapecart");

    gotoxy(18, 9);
    cputs("and");

    gotoxy(0, 11);
    cputs("Draco Browser V1.0C 8 Bit (27.12.2009)");
    gotoxy(8, 12);
    cputs("Copyright 2009 by Draco");
    textcolor(COLOR_GREEN);
    gotoxy(0, 15);
    cputs("THIS PROGRAM IS DISTRIBUTED IN THE HOPE");
    gotoxy(0, 16);
    cputs("THAT IT WILL BE USEFUL.");
    gotoxy(0, 18);
    cputs("IT IS PROVIDED WITH NO WARRANTY OF ANY ");
    gotoxy(0, 19);
    cputs("KIND.");
    gotoxy(0, 21);
    textcolor(COLOR_LIGHTRED);
    cputs("USE IT AT YOUR OWN RISK!");
    gotoxy(0, BOTTOM);
    waitKey();
    updateScreen();
}

static void printNameSize(DirElement *element)
{
    if (element->type == FILE_DIR)
    {
      cprintf(" %-16s    DIR ", element->name);
    }
    else if (element->type == FILE_UNKNOWN)
    {
        cprintf(" %-16s     -  ", element->name);
    }
    else
    {
        uint32_t size = element->size;
        if (size >= 1024)
        {
            size >>= 10;
            if (size >= 1024)
            {
                size >>= 10;
                cprintf(" %-16s  %4luM ", element->name, size);
            }
            else
            {
                cprintf(" %-16s  %4luk ", element->name, size);
            }
        }
        else
        {
            cprintf(" %-16s  %4lub ", element->name, element->size);
        }
    }
}

static void updateDir(uint16_t last_selected)
{
    uint8_t lastpage;
    uint8_t nextpage;

    lastpage = last_selected/DIRH;
    nextpage = dir->selected/DIRH;

    if (lastpage != nextpage)
    {
        printDir();
    }
    else
    {
        printElement(last_selected);
        printElement(dir->selected);
    }
}

static void printDir(void)
{
    DirElement *current;
    uint8_t page;
    uint16_t element, idx;

    if (dir->no_of_elements == 0)
    {
        return;
    }
    else
    {
        page = dir->selected/DIRH;
        element = page*DIRH;
        idx = 0;

        while (element < dir->no_of_elements && idx < DIRH)
        {
            current = dir->elements + element;
            gotoxy(DIR1X+1, DIR1Y+1+idx);
            if (element == dir->selected)
            {
                revers(1);
            }

            printNameSize(current);
            revers(0);
            textcolor(textc);

          ++idx;
          ++element;
        }

        // clear empty lines
        for (;idx < DIRH; idx++)
        {
            gotoxy(DIR1X+1, DIR1Y+1+idx);
            cputs("                         ");
        }
    }
}

static void printElement(uint16_t pos)
{
    DirElement *current;
    uint8_t page = 0;
    uint16_t idx = 0;
    uint16_t element = 0;
    uint16_t yoff = 0;

    if (dir->no_of_elements == 0)
    {
        return;
    }
    else
    {
        yoff = pos % DIRH;
        gotoxy(DIR1X+1, DIR1Y+1+yoff);
        if (pos == dir->selected)
        {
            revers(1);
        }
        else
        {
            revers(0);
        }

        current = dir->elements + pos;
        printNameSize(current);
        revers(0);
        textcolor(textc);
    }
}
