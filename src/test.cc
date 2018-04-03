#include <cstdlib>
#include <iostream>
#include <sstream>
#include "vte.h"

using namespace std;
using namespace vtutils::screen;
using namespace vtutils::vte;

std::ostream& operator<<(std::ostream &in, const Color &color) {
  if (color.color_code != COLOR_RGB) {
    in << "code:" << int(color.color_code);
  } else {
    in << "rgb[r:" << int(color.r)
        << ",g:" << int(color.g)
        << ",b:" << int(color.b)
        << ']';
  }
  return in;
}
std::ostream& operator<<(std::ostream &in, const Attr &attr) {
  in << "Attr[fg[" << attr.fg << "],bg[" << attr.bg << "]";
  if (attr.blink) {
    in << ",blink";
  }
  if (attr.bold) {
    in << ",bold";
  }
  if (attr.inverse) {
    in << ",inverse";
  }
  if (attr.protect) {
    in << ",protect";
  }
  if (attr.underline) {
    in << ",underline";
  }
  in << ']';
  return in;
}

class TestScreen : public Screen {
 public:
  void resize() override {
    cout << "TestScreen#resize\n";
  }
  void reset() override {
    cout << "TestScreen#reset\n";
  }
  void set_flags(unsigned int flags) override {
    cout << "TestScreen#set_flags: " << std::hex << flags << '\n';
  }
  void write(char32_t sym, Attr *attr) override {
    cout << "TestScreen#write: " << std::hex << sym << std::dec << ", " << *attr << '\n';
  }
  Attr default_attr() {
    cout << "TestScreen#default_attr\n";
    return Attr{Color{COLOR_BLUE},Color{COLOR_RED}};
  }
};

int main() {
  cout << "In main!\n";
  TestScreen test_screen;
  
  Vte vte{test_screen};
  test_screen.resize();
  istringstream("tester\n") >> vte;
  istringstream("ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
  istringstream("\xf6\xa4\x13""asdf\n") >> vte;
//  istringstream(U"ᚳ᛫ᛗᛁᚳᛚ\n") >> vte;
  
  cout << sizeof(Attr) << '\n';

  
  return 0;
}
