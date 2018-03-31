#ifndef SCROLL_H_
#define SCROLL_H_

#include <cstddef>
#include <cstdint>
#include <iostream>

// A terminal "scrolling" library.
namespace scroll {

// A virtual "screen", used by the virtual terminal emulator to issue the required "drawing"
// commands.
class Screen {
 public:
  virtual ~Screen() = default;

  // resize this screen to the given height and width
  virtual void resize() = 0;
};

// The Virtual Terminal Emulator. This class was ported from the C code at the libtsm project
// ( https://www.freedesktop.org/wiki/Software/kmscon/libtsm/ )
// This is a state machine that processes terminal escape sequences, and translates them into
// standardized calls to Screen.
class Vte {
 public:
  Vte(Screen *s, unsigned int f) : screen(s), flags(f) { }
  Vte(Screen *s) : Vte(s, 0) { }

  friend std::istream& operator>>(std::istream &in, Vte &vte);

  int set_palette(const char *palette);
  void reset();
  void hard_reset();

 private:
  Screen *screen;
  unsigned int flags;

//  unsigned long ref;
//  tsm_log_t llog;
//  void *llog_data;
//  struct tsm_screen *con;
//  tsm_vte_write_cb write_cb;
//  void *data;
//  char *palette_name;
//
//  struct tsm_utf8_mach *mach;
//  unsigned long parse_cnt;
//
//  unsigned int state;
//  unsigned int csi_argc;
//  int csi_argv[CSI_ARG_MAX];
//  unsigned int csi_flags;
//
//  uint8_t (*palette)[3];
//  struct tsm_screen_attr def_attr;
//  struct tsm_screen_attr cattr;
//
//  tsm_vte_charset **gl;
//  tsm_vte_charset **gr;
//  tsm_vte_charset **glt;
//  tsm_vte_charset **grt;
//  tsm_vte_charset *g0;
//  tsm_vte_charset *g1;
//  tsm_vte_charset *g2;
//  tsm_vte_charset *g3;
//
//  struct vte_saved_state saved_state;
  unsigned int alt_cursor_x;
  unsigned int alt_cursor_y;

  void parse_data(uint32_t raw);
};

} // namespace scroll

#endif /* SCROLL_H_ */
