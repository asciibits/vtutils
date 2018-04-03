#ifndef VTUTILS_DEBUG_SCREEN_H_
#define VTUTILS_DEBUG_SCREEN_H_

#include "screen.h"

namespace vtutils {
namespace screen {

class DebugScreen : public Screen {
public:
  void reset();
  void hard_reset();
  
  void set_flags(unsigned int flags);
  void reset_flags(unsigned int flags);
  
  void write(char32_t sym, Attr *attr);
  void newline();
  void insert_lines(unsigned int num);
  void delete_lines(unsigned int num);
  void insert_chars(unsigned int num);
  void delete_chars(unsigned int num);
  // system bell
  void alert();
  
  Attr default_attr();
  void set_def_attr(screen::Attr attr);
  
  void move_left(unsigned int num);
  void move_right(unsigned int num);
  void move_to(unsigned int x, unsigned int y);
  void move_up(unsigned int num, bool scroll);
  void move_down(unsigned int num, bool scroll);
  void move_line_home();
  
  void scroll_up(unsigned int num);
  void scroll_down(unsigned int num);
  
  void set_tabstop();
  void reset_tabstop();
  void reset_all_tabstops();
  void tab_right(unsigned int num);
  void tab_left(unsigned int num);
  
  unsigned int get_cursor_x();
  unsigned int get_cursor_y();

  void erase_screen(bool protect);
  void erase_cursor_to_screen(bool protect);
  void erase_screen_to_cursor(bool protect);
  void erase_cursor_to_end(bool protect);
  void erase_home_to_cursor(bool protect);
  void erase_current_line(bool protect);
  void erase_chars(unsigned int num);
  
  void set_margins(unsigned int top, unsigned int bottom);
};

} // namespace screen
} // namespace vtutils

#endif /* VTUTILS_DEBUG_SCREEN_H_ */
