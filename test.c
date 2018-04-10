#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1

#include <stdio.h>
#include <locale.h>
#include <curses.h>
#include <stdlib.h>

int main(char* argv, int argc) {
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  WINDOW* _w = newwin(0, 0, 0, 0);
  wprintw(_w, "HEllo!!\n\n");
  wprintw(_w, "UTF8: \xc2\xa3\n\n");
  wprintw(_w, "Unicode escape: \u00a3\n\n");
  wprintw(_w, "using addch: ");
//  waddch(_w, 'X');
  cchar_t t;
  setcchar(&t, (wchar_t[]){L'\u00a3', 0 }, 0, 0, 0);
//  cchar_t s[] { t, 0 };
//  wadd_wch(_w, &t);
  wecho_wchar(_w, &t);
  wrefresh(_w);
  wgetch(_w);
  endwin();
  return 0;
}
