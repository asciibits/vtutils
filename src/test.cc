#include <cstdlib>
#include <iostream>
#include <sstream>
#include "vte.h"
#include "debug_screen.h"

using namespace std;
using namespace vtutils::screen;
using namespace vtutils::vte;

int main() {
  cout << "In main!\n";
  DebugScreen test_screen;
  
  Vte vte{test_screen};
  istringstream("tester\n") >> vte;
  istringstream("ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
  istringstream("\xf6\xa4\x13""asdf\n") >> vte;
//  istringstream(U"ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
  
  cout << sizeof(Attr) << '\n';

  return 0;
}
