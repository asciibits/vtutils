#ifndef VTUTILS_VTE_H_
#define VTUTILS_VTE_H_

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <string>

#include "charsets.h"
#include "config.h"
#include "screen.h"
#include "unicode.h"

// A terminal emulation library.
namespace vtutils {
namespace vte {

namespace {
// Input parser states
enum ParserState {
  STATE_NONE,       // placeholder
  STATE_GROUND,     // initial state and ground
  STATE_ESC,        // ESC sequence was started
  STATE_ESC_INT,    // intermediate escape characters
  STATE_CSI_ENTRY,  // starting CSI sequence
  STATE_CSI_PARAM,  // CSI parameters
  STATE_CSI_INT,    // intermediate CSI characters
  STATE_CSI_IGNORE, // CSI error; ignore this CSI sequence
  STATE_DCS_ENTRY,  // starting DCS sequence
  STATE_DCS_PARAM,  // DCS parameters
  STATE_DCS_INT,    // intermediate DCS characters
  STATE_DCS_PASS,   // DCS data passthrough
  STATE_DCS_IGNORE, // DCS error; ignore this DCS sequence
  STATE_OSC_STRING, // parsing OCS sequence
  STATE_ST_IGNORE,  // unimplemented seq; ignore until ST
};

// Input parser actions
enum ParserAction {
  ACTION_NONE,          // placeholder
  ACTION_IGNORE,        // ignore the character entirely
  ACTION_PRINT,         // print the character on the console
  ACTION_EXECUTE,       // execute single control character (C0/C1)
  ACTION_CLEAR,         // clear current parameter state
  ACTION_COLLECT,       // collect intermediate character
  ACTION_PARAM,         // collect parameter character
  ACTION_ESC_DISPATCH, // dispatch escape sequence
  ACTION_CSI_DISPATCH, // dispatch csi sequence
  ACTION_DCS_START,    // start of DCS data
  ACTION_DCS_COLLECT,  // collect DCS data
  ACTION_DCS_END,      // end of DCS data
  ACTION_OSC_START,    // start of OSC data
  ACTION_OSC_COLLECT,  // collect OSC data
  ACTION_OSC_END,      // end of OSC data
};
}

// The Virtual Terminal Emulator. This class was ported from the C code at the libtsm project
// ( https://www.freedesktop.org/wiki/Software/kmscon/libtsm/ )
// This is a state machine that processes terminal escape sequences, and translates them into
// standardized calls to Screen.

class Vte {
 public:
  Vte(screen::Screen &s) : _screen(s) {
    reset();
  }

  void input(char c);
  
  void reset();

 private:
  screen::Screen &_screen;
  unsigned int _flags;
  vtutils::unicode::Utf8To32Converter _utf8_converter;
  ParserState _state;
//  int set_palette(const char *palette);


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
//  unsigned int csi_argc;
//  int csi_argv[CSI_ARG_MAX];
//  unsigned int csi_flags;
//
//  uint8_t (*palette)[3];
  screen::Attr _attr;

  // The two primary character sets
  charsets::charset *_gl;
  charsets::charset *_gr;
  
  // Temp charsets. Used for next character only
  charsets::charset *_glt;
  charsets::charset *_grt;

  // The four available charsets for this terminal
  charsets::charset *_g0;
  charsets::charset *_g1;
  charsets::charset *_g2;
  charsets::charset *_g3;

//  struct vte_saved_state saved_state;
//  unsigned int alt_cursor_x;
//  unsigned int alt_cursor_y;

  void parse_data(char32_t raw);
  void do_trans(char32_t data, ParserState state, ParserAction act);
  void do_action(char32_t data, ParserAction action);
  char32_t map_char(char32_t val);
  void write_console(char32_t sym);
  void reset_state();
};

// Provide input-stream handling
std::istream& operator>>(std::istream &in, Vte &vte);


} // namespace vte
} // namespace vtutils

#endif /* VTUTILS_VTE_H_ */
