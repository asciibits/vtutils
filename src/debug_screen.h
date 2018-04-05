#ifndef VTUTILS_DEBUG_SCREEN_H_
#define VTUTILS_DEBUG_SCREEN_H_

#include "config.h"
#include "screen.h"

namespace vtutils {
namespace screen {

class DebugScreen : public Screen {
public:
  void reset() override;
  void hard_reset() override;

  void set_flags(unsigned int flags) override;
  void reset_flags(unsigned int flags) override;

  void write(char32_t sym, Attr *attr) override;
  void newline() override;
  void insert_lines(unsigned int num) override;
  void delete_lines(unsigned int num) override;
  void insert_chars(unsigned int num) override;
  void delete_chars(unsigned int num) override;
  void alert() override;

  Attr default_attr() override;
  void set_def_attr(screen::Attr attr) override;

  void move_left(unsigned int num) override;
  void move_right(unsigned int num) override;
  void move_up(unsigned int num, bool scroll) override;
  void move_down(unsigned int num, bool scroll) override;
  void move_to(unsigned int x, unsigned int y) override;
  void move_line_home() override;

  void scroll_up(unsigned int num) override;
  void scroll_down(unsigned int num) override;

  void set_tabstop() override;
  void reset_tabstop() override;
  void reset_all_tabstops() override;
  void tab_right(unsigned int num) override;
  void tab_left(unsigned int num) override;

  unsigned int get_cursor_x() override;
  unsigned int get_cursor_y() override;

  void erase_screen(bool protect) override;
  void erase_cursor_to_screen(bool protect) override;
  void erase_screen_to_cursor(bool protect) override;
  void erase_cursor_to_end(bool protect) override;
  void erase_home_to_cursor(bool protect) override;
  void erase_current_line(bool protect) override;
  void erase_chars(unsigned int num) override;
  
  void set_margins(unsigned int top, unsigned int bottom) override;
};

} // namespace screen
} // namespace vtutils

#endif /* VTUTILS_DEBUG_SCREEN_H_ */
