#include <cstdlib>
#include <iostream>
#include <sstream>
#include "scroll.h"

using namespace std;

class TestScreen : public scroll::Screen {
 public:
  void resize() override {
    cout << "TestScreen#resize\n";
  }
};

int main(int argc, char** argv) {
  cout << "In main!\n";
  TestScreen test_screen;
  scroll::Vte vte(&test_screen);
  test_screen.resize();
  istringstream("test\n") >> vte;

  vte.reset();
  return 0;
}
