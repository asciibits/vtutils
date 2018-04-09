#ifndef VTUTILS_DEBUG_SCREEN_H_
#define VTUTILS_DEBUG_SCREEN_H_

#include <iostream>
#include <ostream>
#include <typeinfo>
#ifdef __GNUG__
#include <cxxabi.h>
#endif
#include "screen.h"

namespace vtutils {
namespace screen {

class DebugScreen : public Screen {
public:
  DebugScreen(std::ostream &out) : _out(out) { };
  DebugScreen() : DebugScreen(std::cerr) { }
  virtual ~DebugScreen() = default;
  
  virtual void reset() override;
  virtual void hard_reset() override;

  virtual void set_flags(unsigned int flags) override;
  virtual void reset_flags(unsigned int flags) override;

  virtual void print(char32_t sym, Attr *attr) override;
  virtual void newline() override;
  virtual void insert_lines(unsigned int num) override;
  virtual void delete_lines(unsigned int num) override;
  virtual void insert_chars(unsigned int num) override;
  virtual void delete_chars(unsigned int num) override;
  virtual void alert() override;

  virtual Attr default_attr() override;
  virtual void set_def_attr(screen::Attr attr) override;

  virtual void move_left(unsigned int num) override;
  virtual void move_right(unsigned int num) override;
  virtual void move_up(unsigned int num, bool scroll) override;
  virtual void move_down(unsigned int num, bool scroll) override;
  virtual void move_to(unsigned int x, unsigned int y) override;
  virtual void move_line_home() override;

  virtual void scroll_up(unsigned int num) override;
  virtual void scroll_down(unsigned int num) override;

  virtual void set_tabstop() override;
  virtual void reset_tabstop() override;
  virtual void reset_all_tabstops() override;
  virtual void tab_right(unsigned int num) override;
  virtual void tab_left(unsigned int num) override;

  virtual unsigned int get_cursor_x() override;
  virtual unsigned int get_cursor_y() override;

  virtual void erase_screen(bool protect) override;
  virtual void erase_cursor_to_screen(bool protect) override;
  virtual void erase_screen_to_cursor(bool protect) override;
  virtual void erase_cursor_to_end(bool protect) override;
  virtual void erase_home_to_cursor(bool protect) override;
  virtual void erase_current_line(bool protect) override;
  virtual void erase_chars(unsigned int num) override;
  
  virtual void set_margins(unsigned int top, unsigned int bottom) override;

  virtual void write(char sym) override;
  
  std::string class_name() {
    if (_class_name.empty()) {
      const char *type_name = typeid(*this).name();
#ifdef __GNUG__
      int status;
      char *demangled = abi::__cxa_demangle(type_name, NULL, NULL, &status);
      if (status == 0) {
        _class_name = std::string(demangled);
        std::free(demangled);
      } else
#endif
      _class_name = std::string(type_name);
    }
    return _class_name;
  }
protected:
  std::ostream &_out;
private:
  std::string _class_name;
};

} // namespace screen
} // namespace vtutils

#endif /* VTUTILS_DEBUG_SCREEN_H_ */
