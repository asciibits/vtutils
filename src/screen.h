#ifndef VTUTILS_SCREEN_H_
#define VTUTILS_SCREEN_H_

#include <ostream>

// A Screen abstraction for the terminal emulation library.
namespace vtutils {
namespace screen {
  
static const unsigned int SCREEN_INSERT_MODE      = 0x01;
static const unsigned int SCREEN_AUTO_WRAP        = 0x02;
static const unsigned int SCREEN_REL_ORIGIN       = 0x04;
static const unsigned int SCREEN_INVERSE           = 0x08;
static const unsigned int SCREEN_HIDE_CURSOR      = 0x10;
static const unsigned int SCREEN_FIXED_POS         = 0x20;
static const unsigned int SCREEN_ALTERNATE         = 0x40;
static const unsigned int SCREEN_SMOOTH_SCROLLING = 0x80;

enum ColorCode : int8_t {
  COLOR_CODE_RGB = -1, // indicates use RGB value
  COLOR_CODE_BLACK,
  COLOR_CODE_RED,
  COLOR_CODE_GREEN,
  COLOR_CODE_YELLOW,
  COLOR_CODE_BLUE,
  COLOR_CODE_MAGENTA,
  COLOR_CODE_CYAN,
  COLOR_CODE_LIGHT_GREY,
  COLOR_CODE_DARK_GREY,
  COLOR_CODE_LIGHT_RED,
  COLOR_CODE_LIGHT_GREEN,
  COLOR_CODE_LIGHT_YELLOW,
  COLOR_CODE_LIGHT_BLUE,
  COLOR_CODE_LIGHT_MAGENTA,
  COLOR_CODE_LIGHT_CYAN,
  COLOR_CODE_WHITE
};

struct Color {
  ColorCode color_code;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct Attr {
  Color fg;
  Color bg;
  
  bool bold : 1;
  bool underline : 1;
  bool inverse : 1;
  bool protect : 1;
  bool blink : 1;
};

std::ostream& operator<<(std::ostream &out, const Color &color);
std::ostream& operator<<(std::ostream &out, const Attr &attr);

// A virtual "screen", used by the virtual terminal emulator to issue the required "drawing"
// commands.
class Screen {
 public:
  virtual ~Screen() = default;

  virtual void reset() = 0;
  virtual void hard_reset() = 0;
  
  virtual void set_flags(unsigned int flags) = 0;
  virtual void reset_flags(unsigned int flags) = 0;
  
  // print the character to the screen
  virtual void print(char32_t sym, Attr *attr) = 0;
  
  virtual void newline() = 0;
  virtual void insert_lines(unsigned int num) = 0;
  virtual void delete_lines(unsigned int num) = 0;
  virtual void insert_chars(unsigned int num) = 0;
  virtual void delete_chars(unsigned int num) = 0;
  // system bell
  virtual void alert() = 0;
  
  virtual Attr default_attr() = 0;
  virtual void set_def_attr(screen::Attr attr) = 0;
  
  virtual void move_left(unsigned int num) = 0;
  virtual void move_right(unsigned int num) = 0;
  virtual void move_up(unsigned int num, bool scroll) = 0;
  virtual void move_down(unsigned int num, bool scroll) = 0;
  virtual void move_to(unsigned int x, unsigned int y) = 0;
  virtual void move_line_home() = 0;
  
  virtual void scroll_up(unsigned int num) = 0;
  virtual void scroll_down(unsigned int num) = 0;
  
  virtual void set_tabstop() = 0;
  virtual void reset_tabstop() = 0;
  virtual void reset_all_tabstops() = 0;
  virtual void tab_right(unsigned int num) = 0;
  virtual void tab_left(unsigned int num) = 0;
  
  virtual unsigned int get_cursor_x() = 0;
  virtual unsigned int get_cursor_y() = 0;

  virtual void erase_screen(bool protect) = 0;
  virtual void erase_cursor_to_screen(bool protect) = 0;
  virtual void erase_screen_to_cursor(bool protect) = 0;
  virtual void erase_cursor_to_end(bool protect) = 0;
  virtual void erase_home_to_cursor(bool protect) = 0;
  virtual void erase_current_line(bool protect) = 0;
  virtual void erase_chars(unsigned int num) = 0;
  
  virtual void set_margins(unsigned int top, unsigned int bottom) = 0;
  
  // push the character to the sub-processes std-in
  virtual void write(char sym) = 0;
};

} // namespace screen
} // namespace vtutils

#endif /* VTUTILS_SCREEN_H_ */
