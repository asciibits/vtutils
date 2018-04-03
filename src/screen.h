#ifndef VTUTILS_SCREEN_H_
#define VTUTILS_SCREEN_H_

// A Screen abstraction for the terminal emulation library.
namespace vtutils {
namespace screen {
  
static const unsigned int SCREEN_INSERT_MODE = 0x01;
static const unsigned int SCREEN_AUTO_WRAP   = 0x02;
static const unsigned int SCREEN_REL_ORIGIN  = 0x04;
static const unsigned int SCREEN_INVERSE     = 0x08;
static const unsigned int SCREEN_HIDE_CURSOR = 0x10;
static const unsigned int SCREEN_FIXED_POS   = 0x20;
static const unsigned int SCREEN_ALTERNATE   = 0x40;

enum ColorCode : uint8_t {
  COLOR_RGB, // indicates use RGB value
  COLOR_BLACK,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_YELLOW,
  COLOR_BLUE,
  COLOR_MAGENTA,
  COLOR_CYAN,
  COLOR_LIGHT_GREY,
  COLOR_DARK_GREY,
  COLOR_LIGHT_RED,
  COLOR_LIGHT_GREEN,
  COLOR_LIGHT_YELLOW,
  COLOR_LIGHT_BLUE,
  COLOR_LIGHT_MAGENTA,
  COLOR_LIGHT_CYAN,
  COLOR_WHITE
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

// A virtual "screen", used by the virtual terminal emulator to issue the required "drawing"
// commands.
class Screen {
 public:
  virtual ~Screen() = default;

  virtual void reset() = 0;
  virtual void set_flags(unsigned int flags) = 0;
  virtual void write(char32_t sym, Attr *attr) = 0;
  virtual Attr default_attr() = 0;

  // resize this screen to the given height and width
  virtual void resize() = 0;
};

} // namespace screen
} // namespace vtutils

#endif /* VTUTILS_SCREEN_H_ */

