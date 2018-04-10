#ifndef VTUTILS_VTE_H_
#define VTUTILS_VTE_H_

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdarg.h>
#include <stdexcept>
#include <string>

#include "charsets.h"
#include "config.h"
#include "screen.h"
#include "unicode.h"

// A terminal emulation library.
namespace vtutils {
namespace vte {

enum LogLevel {
  LOG_TRACE = 1,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR
};

/**
 * Logging Callback
 *
 * @file: Source code file where the log message originated or NULL
 * @line: Line number in source code or 0
 * @func: C function name or NULL
 * @sev: Kernel-style severity between 0=FATAL and 7=DEBUG
 * @format: printf-formatted message
 * @args: arguments for printf-style @format
 *
 * This is the type of a logging callback function. You can always pass NULL
 * instead of such a function to disable logging.
 */
typedef void (*log_cb) (
    std::string file,
    int line,
    std::string func,
    LogLevel level,
    std::string format,
    va_list args);

static inline
void log_format(log_cb logger,
        std::string file,
        int line,
        std::string func,
        LogLevel level,
        std::string format,
        ...) {
  va_list list;
  if (logger) {
    va_start(list, format);
    logger(file, line, func, level, format, list);
    va_end(list);
  }
}

#define LOG_DEFAULT __FILE__, __LINE__, __func__

#define log_trace(obj, format, ...) \
    log_printf((obj), (LOG_TRACE), (format), ##__VA_ARGS__)

#define log_info(obj, format, ...) \
    log_printf((obj), (LOG_INFO), (format), ##__VA_ARGS__)

#define log_warn(obj, format, ...) \
    log_printf((obj), (LOG_WARN), (format), ##__VA_ARGS__)

#define log_printf(obj, level, format, ...) \
    log_format((obj)->_logger, \
            LOG_DEFAULT, \
            (level), \
            (format), \
            ##__VA_ARGS__)



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
  ACTION_NONE,         // placeholder
  ACTION_IGNORE,       // ignore the character entirely
  ACTION_PRINT,        // print the character on the console
  ACTION_EXECUTE,      // execute single control character (C0/C1)
  ACTION_CLEAR,        // clear current parameter state
  ACTION_COLLECT,      // collect intermediate character
  ACTION_PARAM,        // collect parameter character
  ACTION_ESC_DISPATCH, // dispatch escape sequence
  ACTION_CSI_DISPATCH, // dispatch csi sequence
  ACTION_DCS_START,    // start of DCS data
  ACTION_DCS_COLLECT,  // collect DCS data
  ACTION_DCS_END,      // end of DCS data
  ACTION_OSC_START,    // start of OSC data
  ACTION_OSC_COLLECT,  // collect OSC data
  ACTION_OSC_END,      // end of OSC data
};

// max CSI arguments
const int CSI_ARG_MAX = 16;

// Saved state
struct saved_state {
  unsigned int cursor_x;
  unsigned int cursor_y;
  screen::Attr attr;
  charsets::charset *gl;
  charsets::charset *gr;
  bool wrap_mode;
  bool origin_mode;
};

// The Virtual Terminal Emulator. This class was ported from the C code at the libtsm project
// ( https://www.freedesktop.org/wiki/Software/kmscon/libtsm/ )
// This is a state machine that processes terminal escape sequences, and translates them into
// standardized calls to Screen.
class Vte {
 public:
  Vte(screen::Screen &s, log_cb l) : _screen(s), _logger(l) {
    reset();
  }
  Vte(screen::Screen &s): Vte(s, nullptr) { };

  // Handle a single character. Handles unicode and bit-size enforcement. Logic
  // pushed to parse_data
  void input(char c);
  // convenience wrapper around input above
  void input(std::string s);

  // reset this terminal
  void reset();
  // hard reset. Similar to reset, but goes even farther to ensure the full
  // state is clear
  void hard_reset();

 private:
  // initialized state
  screen::Screen &_screen;
  log_cb _logger;
  //tsm_vte_write_cb write_cb;
  
  // state machine state
  unsigned int _flags;
  ParserState _state;
  unsigned int _csi_argc;
  int _csi_argv[CSI_ARG_MAX];
  unsigned int _csi_flags;
  unsigned int _parse_cnt = 0;

  // UTF-8 state machine
  vtutils::unicode::Utf8To32Converter _utf8_converter;

  //
  // Fields below are used with direct interactions with _screen
  //

  screen::Attr _attr;
  // The two active character sets
  charsets::charset *_gl;
  charsets::charset *_gr;
  // Temp charsets. Used for next character only
  charsets::charset *_glt;
  charsets::charset *_grt;
  // The four loaded charsets for this terminal
  charsets::charset *_g0;
  charsets::charset *_g1;
  charsets::charset *_g2;
  charsets::charset *_g3;
  // screen state
  struct saved_state _saved_state;
  unsigned int _alt_cursor_x;
  unsigned int _alt_cursor_y;

  // Entry for all parsing
  void parse_data(char32_t raw);

  // private implementation details
  void do_trans(char32_t data, ParserState state, ParserAction act);
  void do_action(char32_t data, ParserAction action);
  void do_execute(char32_t ctrl);
  void do_clear();
  void do_collect(char32_t data);
  void do_param(char32_t data);
  void do_esc(char32_t data);
  void do_csi(char32_t data);

  // Redirection Interactions (Terminal invoking commands on terminal)
  void write_console(char32_t sym);
  void write(const std::string u8);
  void send_primary_da();
  bool set_charset(charsets::charset *set);
  char32_t map_char(char32_t val);
  void set_reset_flag(bool set, unsigned int flag);

  // CSI implementation
  void csi_attribute();
  void csi_soft_reset();
  void csi_compat_mode();
  void csi_mode(bool set);
  void csi_dev_attr();
  void csi_dsr();

  // screen state handling
  void reset_state();
  void save_state();
  void restore_state();
};

// Provide input-stream handling
std::istream& operator>>(std::istream &in, Vte &vte);


} // namespace vte
} // namespace vtutils

#endif /* VTUTILS_VTE_H_ */
