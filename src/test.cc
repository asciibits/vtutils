#include <cstdlib>
#include <ostream>
#include <sstream>
#include <bits/stdio2.h>
#include "vte.h"
#include "debug_screen.h"

using namespace std;
using namespace vtutils::screen;
using namespace vtutils::vte;

void log(
    string file,
    int line,
    string func,
    string format,
    va_list args) {
  
  cerr << file << '#' << func << '[' << line << "]: " << std::flush;
  std::vfprintf(stderr, format.c_str(), args);
  fflush(stderr);
  cerr << '\n';
}
int main() {
  cout << "In main!\n";
  DebugScreen test_screen;
  
  Vte vte{test_screen, log};
  istringstream("tester\n") >> vte;
  istringstream("ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
  istringstream("\xf6\xa4\x13""asdf\n") >> vte;
//  istringstream(U"ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
  
  cout << sizeof(Attr) << '\n';

  return 0;
}
