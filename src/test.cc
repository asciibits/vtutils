#include <chrono>
#include <cstdlib>
#include <ostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <bits/stdio2.h>
#include <ncurses.h>

#include "vte.h"
#include "debug_screen.h"
#include "curses_screen.h"

using namespace vtutils::screen;
using namespace vtutils::vte;

static std::ofstream err("/home/lsanderson/err.out");

const std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
typedef std::chrono::duration<double> seconds_t;

void log(
    std::string file,
    int line,
    std::string func,
    std::string format,
    va_list args) {

  seconds_t d = std::chrono::system_clock::now() - start;

  err << std::setw(13) << std::fixed << std::right << d.count() << std::setw(0) << " ";
  err << file << '#' << func << '[' << line << "]: ";
  char buf[256];
  std::vsnprintf(buf, 255, format.c_str(), args);
  err << std::string(buf) << std::endl;
}

WINDOW* do_curses() {
  initscr();
  raw();
  noecho();
  WINDOW *window = newwin(0, 0, 0, 0);
  wprintw(window, "Hello World !!!");
  wrefresh(window);
  return window;
}

int main() {
  err << "In main!" << std::endl;

  WINDOW *window = do_curses();
  CursesScreen test_screen(window, err);

  Vte vte{test_screen, log};
//  istringstream("tester\n") >> vte;
//  istringstream("ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
//  istringstream("\xf6\xa4\x13""asdf\n") >> vte;
////  istringstream(U"ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;

  std::cin >> vte;
  
  return 0;
}
