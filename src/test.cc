#include <chrono>
#include <cstdlib>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <bits/stdio2.h>

#include "vte.h"
#include "debug_screen.h"

using namespace vtutils::screen;
using namespace vtutils::vte;

const std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
typedef std::chrono::duration<double> seconds_t;

std::string black = "\033[38;5;15;48;5;0m";
std::string red = "\033[38;5;0;48;5;9m";
std::string blue = "\033[38;5;15;48;5;4m";
std::string yellow = "\033[38;5;0;48;5;11m";
std::string green = "\033[38;5;15;48;5;2m";

void log(
    std::string file,
    int line,
    std::string func,
    LogLevel level,
    std::string format,
    va_list args) {

  seconds_t d = std::chrono::system_clock::now() - start;

  switch (level) {
    case LOG_TRACE:
      std::cerr << green;
      break;
    case LOG_INFO:
      std::cerr << blue;
      break;
    case LOG_WARN:
      std::cerr << yellow;
      break;
    case LOG_ERROR:
      std::cerr << red;
      break;
  }
  
  std::cerr << std::setw(13) << std::fixed << std::right << d.count() << std::setw(0) << black << " ";
  std::cerr << file << '#' << func << '[' << line << "]: " << std::flush;
  std::vfprintf(stderr, format.c_str(), args);
  fflush(stderr);
  std::cerr << std::endl;
}
int main() {
  std::cout << "In main!" << std::endl;
  DebugScreen test_screen;

  Vte vte{test_screen, log};
//  istringstream("tester\n") >> vte;
//  istringstream("ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
//  istringstream("\xf6\xa4\x13""asdf\n") >> vte;
////  istringstream(U"ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;

  std::cin >> vte;

  return 0;
}
