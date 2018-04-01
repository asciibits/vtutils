#include "vte.h"

#include <iostream>

namespace scroll {

namespace {
/* Input parser states */
enum parser_state {
    STATE_NONE,       /* placeholder */
    STATE_GROUND,     /* initial state and ground */
    STATE_ESC,        /* ESC sequence was started */
    STATE_ESC_INT,    /* intermediate escape characters */
    STATE_CSI_ENTRY,  /* starting CSI sequence */
    STATE_CSI_PARAM,  /* CSI parameters */
    STATE_CSI_INT,    /* intermediate CSI characters */
    STATE_CSI_IGNORE, /* CSI error; ignore this CSI sequence */
    STATE_DCS_ENTRY,  /* starting DCS sequence */
    STATE_DCS_PARAM,  /* DCS parameters */
    STATE_DCS_INT,    /* intermediate DCS characters */
    STATE_DCS_PASS,   /* DCS data passthrough */
    STATE_DCS_IGNORE, /* DCS error; ignore this DCS sequence */
    STATE_OSC_STRING, /* parsing OCS sequence */
    STATE_ST_IGNORE,  /* unimplemented seq; ignore until ST */
    STATE_NUM
};

/* Input parser actions */
enum parser_action {
  ACTION_NONE,          /* placeholder */
  ACTION_IGNORE,        /* ignore the character entirely */
  ACTION_PRINT,         /* print the character on the console */
  ACTION_EXECUTE,       /* execute single control character (C0/C1) */
  ACTION_CLEAR,         /* clear current parameter state */
  ACTION_COLLECT,       /* collect intermediate character */
  ACTION_PARAM,         /* collect parameter character */
  ACTION_ESC_DISPATCH,  /* dispatch escape sequence */
  ACTION_CSI_DISPATCH,  /* dispatch csi sequence */
  ACTION_DCS_START,     /* start of DCS data */
  ACTION_DCS_COLLECT,   /* collect DCS data */
  ACTION_DCS_END,       /* end of DCS data */
  ACTION_OSC_START,     /* start of OSC data */
  ACTION_OSC_COLLECT,   /* collect OSC data */
  ACTION_OSC_END,       /* end of OSC data */
  ACTION_NUM
};
}

/* CSI flags */
static const unsigned int CSI_BANG    = 0x0001;    /* CSI: ! */
static const unsigned int CSI_CASH    = 0x0002;    /* CSI: $ */
static const unsigned int CSI_WHAT    = 0x0004;    /* CSI: ? */
static const unsigned int CSI_GT      = 0x0008;    /* CSI: > */
static const unsigned int CSI_SPACE   = 0x0010;    /* CSI:   */
static const unsigned int CSI_SQUOTE  = 0x0020;    /* CSI: ' */
static const unsigned int CSI_DQUOTE  = 0x0040;    /* CSI: " */
static const unsigned int CSI_MULT    = 0x0080;    /* CSI: * */
static const unsigned int CSI_PLUS    = 0x0100;    /* CSI: + */
static const unsigned int CSI_POPEN   = 0x0200;    /* CSI: ( */
static const unsigned int CSI_PCLOSE  = 0x0400;    /* CSI: ) */

/* max CSI arguments */
static const unsigned int CSI_ARG_MAX = 16;

/* terminal flags */

/* DEC cursor key mode */
static const unsigned int FLAG_CURSOR_KEY_MODE             = 0x00000001;
/* DEC keypad application mode; TODO: toggle on numlock? */
static const unsigned int FLAG_KEYPAD_APPLICATION_MODE     = 0x00000002;
/* DEC line-feed/new-line mode */
static const unsigned int FLAG_LINE_FEED_NEW_LINE_MODE     = 0x00000004;
/* Disable UTF-8 mode and enable 8bit compatible mode */
static const unsigned int FLAG_8BIT_MODE                   = 0x00000008;
/* Disable 8bit mode and use 7bit compatible mode */
static const unsigned int FLAG_7BIT_MODE                   = 0x00000010;
/* Explicitly use 8bit C1 codes; TODO: implement */
static const unsigned int FLAG_USE_C1                      = 0x00000020;
/* Disable keyboard; TODO: implement? */
static const unsigned int FLAG_KEYBOARD_ACTION_MODE        = 0x00000040;
/* Enable insert mode */
static const unsigned int FLAG_INSERT_REPLACE_MODE         = 0x00000080;
/* Disable local echo */
static const unsigned int FLAG_SEND_RECEIVE_MODE           = 0x00000100;
/* Show cursor */
static const unsigned int FLAG_TEXT_CURSOR_MODE            = 0x00000200;
/* Inverse colors */
static const unsigned int FLAG_INVERSE_SCREEN_MODE         = 0x00000400;
/* Relative origin for cursor */
static const unsigned int FLAG_ORIGIN_MODE                 = 0x00000800;
/* Auto line wrap mode */
static const unsigned int FLAG_AUTO_WRAP_MODE              = 0x00001000;
/* Auto repeat key press; TODO: implement */
static const unsigned int FLAG_AUTO_REPEAT_MODE            = 0x00002000;
/* Send keys from nation charsets; TODO: implement */
static const unsigned int FLAG_NATIONAL_CHARSET_MODE       = 0x00004000;
/* Set background color on erase (bce) */
static const unsigned int FLAG_BACKGROUND_COLOR_ERASE_MODE = 0x00008000;
/* Prepend escape character to next output */
static const unsigned int FLAG_PREPEND_ESCAPE              = 0x00010000;
/* Prevent switching to alternate screen buffer */
static const unsigned int FLAG_TITE_INHIBIT_MODE           = 0x00020000;


std::istream& operator>>(std::istream& in, Vte &vte) {
  char c;
  while (in.get(c)) {
    if (vte.flags & FLAG_7BIT_MODE) {
      // if (u8 & 0x80)
      //llog_debug(vte, "receiving 8bit character U+%d from pty while in 7bit mode",
      //    (int)u8[i]);
      vte.parse_data(c & 0x7f);
    } else if (vte.flags & FLAG_8BIT_MODE) {
      vte.parse_data(c);
    } else {
      vte.parse_data(c);
      //state = mach_feed(vte->mach, u8[i]);
      //if (state == TSM_UTF8_ACCEPT ||
      //    state == TSM_UTF8_REJECT) {
      //  ucs4 = tsm_utf8_mach_get(vte->mach);
      //  parse_data(vte, ucs4);
      //}
    }
  }
  return in;
}

void Vte::parse_data(uint32_t raw) {
  std::cout << "parsing: 0x" << std::hex << raw;
  if (raw >= 32 && raw < 127) {
    std::cout << " (" << (char) raw << ')';
  }
  std::cout << '\n';
}

int Vte::set_palette(const char *palette) {
  throw "not implemented";
}

void Vte::reset() {
  throw "not implemented";
}

void Vte::hard_reset() {
  throw "not implemented";
}


} // namespace scroll
