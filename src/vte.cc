#include "vte.h"

#include <iostream>

/*
 * Comments from original libtsm/tsm_vte.cc:
 * 
 * Virtual Terminal Emulator
 * This is the VT implementation. It is written from scratch. It uses the
 * screen state-machine as output and is tightly bound to it. It supports
 * functionality from vt100 up to vt500 series. It doesn't implement an
 * explicitly selected terminal but tries to support the most important commands
 * to be compatible with existing implementations. However, full vt102
 * compatibility is the least that is provided.
 *
 * The main parser in this file controls the parser-state and dispatches the
 * actions to the related handlers. The parser is based on the state-diagram
 * from Paul Williams: http://vt100.net/emu/
 * It is written from scratch, though.
 * This parser is fully compatible up to the vt500 series. It requires UTF-8 and
 * does not support any other input encoding. The G0 and G1 sets are therefore
 * defined as subsets of UTF-8. You may still map G0-G3 into GL, though.
 *
 * However, the CSI/DCS/etc handlers are not designed after a specific VT
 * series. We try to support all vt102 commands but implement several other
 * often used sequences, too. Feel free to add further.
 */

namespace vtutils {
namespace vte {

namespace {
// entry actions to be performed when entering the selected state
static ParserAction entry_action(ParserState parser_state) {
  switch (parser_state) {
	case STATE_CSI_ENTRY: return ACTION_CLEAR;
	case STATE_DCS_ENTRY: return ACTION_CLEAR;
	case STATE_DCS_PASS: return ACTION_DCS_START;
	case STATE_ESC: return ACTION_CLEAR;
	case STATE_OSC_STRING: return ACTION_OSC_START;
	default: return ACTION_NONE;
  }
}

// exit actions to be performed when leaving the selected state
static ParserAction exit_action(ParserState parser_state) {
  switch (parser_state) {
	case STATE_DCS_PASS: return ACTION_DCS_END;
	case STATE_OSC_STRING: return ACTION_OSC_END;
	default: return ACTION_NONE;
  }
}

///* CSI flags */
//static const unsigned int CSI_BANG    = 0x0001;    // CSI: !
//static const unsigned int CSI_CASH    = 0x0002;    // CSI: $
//static const unsigned int CSI_WHAT    = 0x0004;    // CSI: ?
//static const unsigned int CSI_GT      = 0x0008;    // CSI: >
//static const unsigned int CSI_SPACE   = 0x0010;    // CSI:  
//static const unsigned int CSI_SQUOTE  = 0x0020;    // CSI: '
//static const unsigned int CSI_DQUOTE  = 0x0040;    // CSI: "
//static const unsigned int CSI_MULT    = 0x0080;    // CSI: *
//static const unsigned int CSI_PLUS    = 0x0100;    // CSI: +
//static const unsigned int CSI_POPEN   = 0x0200;    // CSI: (
//static const unsigned int CSI_PCLOSE  = 0x0400;    // CSI: )
//
//
///* max CSI arguments */
// static const unsigned int CSI_ARG_MAX = 16;
//
///* terminal flags */
//// DEC cursor key mode
//static const unsigned int FLAG_CURSOR_KEY_MODE             = 0x00000001;
//// DEC keypad application mode; TODO: toggle on numlock?
//static const unsigned int FLAG_KEYPAD_APPLICATION_MODE     = 0x00000002;
//// DEC line-feed/new-line mode
//static const unsigned int FLAG_LINE_FEED_NEW_LINE_MODE     = 0x00000004;
// Disable UTF-8 mode and enable 8bit compatible mode
static const unsigned int FLAG_8BIT_MODE                   = 0x00000008;
// Disable 8bit mode and use 7bit compatible mode
static const unsigned int FLAG_7BIT_MODE                   = 0x00000010;
//// Explicitly use 8bit C1 codes; TODO: implement
//static const unsigned int FLAG_USE_C1                      = 0x00000020;
//// Disable keyboard; TODO: implement?
//static const unsigned int FLAG_KEYBOARD_ACTION_MODE        = 0x00000040;
//// Enable insert mode
//static const unsigned int FLAG_INSERT_REPLACE_MODE         = 0x00000080;
//// Disable local echo
static const unsigned int FLAG_SEND_RECEIVE_MODE           = 0x00000100;
//// Show cursor
static const unsigned int FLAG_TEXT_CURSOR_MODE            = 0x00000200;
//// Inverse colors
//static const unsigned int FLAG_INVERSE_SCREEN_MODE         = 0x00000400;
//// Relative origin for cursor
//static const unsigned int FLAG_ORIGIN_MODE                 = 0x00000800;
//// Auto line wrap mode
static const unsigned int FLAG_AUTO_WRAP_MODE              = 0x00001000;
//// Auto repeat key press; TODO: implement
static const unsigned int FLAG_AUTO_REPEAT_MODE            = 0x00002000;
//// Send keys from nation charsets; TODO: implement
//static const unsigned int FLAG_NATIONAL_CHARSET_MODE       = 0x00004000;
//// Set background color on erase (bce)
static const unsigned int FLAG_BACKGROUND_COLOR_ERASE_MODE = 0x00008000;
//// Prepend escape character to next output
//static const unsigned int FLAG_PREPEND_ESCAPE              = 0x00010000;
//// Prevent switching to alternate screen buffer
//static const unsigned int FLAG_TITE_INHIBIT_MODE           = 0x00020000;
}

std::istream& operator>>(std::istream &in, Vte &vte) {
  char c;
  while (in.get(c)) {
    vte.input(c);
  }
  return in;
}

void Vte::input(char c) {
  if (_flags & FLAG_7BIT_MODE) {
    // if (u8 & 0x80)
    //llog_debug(vte, "receiving 8bit character U+%d from pty while in 7bit mode",
    //    (int)u8[i]);
    parse_data(c & 0x7f);
  } else if (_flags & FLAG_8BIT_MODE) {
    parse_data(c);
  } else {
    char32_t raw;
    if (_utf8_converter.put(c, raw)) {
      parse_data(raw);
    }
  }
}

void Vte::parse_data(char32_t raw) {
  // events that may occur in any state.
  // Processes everything in the ranges: 0x18,0x1a-0x1b,0x80-0x9f
  switch (raw) {
    case 0x18: // skip to 1a
    case 0x1a: // skip to 80
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8a:
    case 0x8b:
    case 0x8c:
    case 0x8d:
    case 0x8e:
    case 0x8f: // skip to 91
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97: // skip to 99
    case 0x99: 
    case 0x9a: // skip to 9b
    case 0x9c:
      do_trans(raw, STATE_GROUND, ACTION_EXECUTE);
      return;
    case 0x1b:
      do_trans(raw, STATE_ESC, ACTION_NONE);
      return;
    case 0x98:
    case 0x9e:
    case 0x9f:
      do_trans(raw, STATE_ST_IGNORE, ACTION_NONE);
      return;
    case 0x90:
      do_trans(raw, STATE_DCS_ENTRY, ACTION_NONE);
      return;
    case 0x9d:
      do_trans(raw, STATE_OSC_STRING, ACTION_NONE);
      return;
    case 0x9b:
      do_trans(raw, STATE_CSI_ENTRY, ACTION_NONE);
      return;
  }

  // events that depend on the current state
  switch (_state) {
    case STATE_GROUND:
      if (raw < 0x20) {
        // control characters not processed above
        do_trans(raw, STATE_NONE, ACTION_EXECUTE);
      } else {
        do_trans(raw, STATE_NONE, ACTION_PRINT);
      }
      return;
    case STATE_ESC:
      // switch handles ranges: 0x50,0x58,0x5b,0x5d-0x5f,0x7f
      switch (raw) {
        case 0x50:
          do_trans(raw, STATE_DCS_ENTRY, ACTION_NONE);
          return;
        case 0x5b:
          do_trans(raw, STATE_CSI_ENTRY, ACTION_NONE);
          return;
        case 0x5d:
          do_trans(raw, STATE_OSC_STRING, ACTION_NONE);
          return;
        case 0x58:
        case 0x5e:
        case 0x5f:
          do_trans(raw, STATE_ST_IGNORE, ACTION_NONE);
          return;
        case 0x7f:
          do_trans(raw, STATE_NONE, ACTION_IGNORE);
          return;
      }
      if (raw < 0x20) {
        // control characters not processed above
        do_trans(raw, STATE_NONE, ACTION_EXECUTE);
      } else if (raw >= 0x30 && raw < 0x80) {
        // 0x30-0x4f,0x51-0x57,0x59-0x5a,0x60-0x7e
        do_trans(raw, STATE_GROUND, ACTION_ESC_DISPATCH);
      } else {
        // 0x20-0x2f,0xa0-
        do_trans(raw, STATE_ESC_INT, ACTION_COLLECT);
      }
      return;
    case STATE_ESC_INT:
      if (raw < 0x20) {
        do_trans(raw, STATE_NONE, ACTION_EXECUTE);
      } else if (raw >= 0x30 && raw < 0x7f) {
        do_trans(raw, STATE_GROUND, ACTION_ESC_DISPATCH);
      } else if (raw == 0x7f) {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      } else {
        do_trans(raw, STATE_NONE, ACTION_COLLECT);
      }
      return;
    case STATE_CSI_ENTRY:
      switch (raw) {
        case 0x3a:
          do_trans(raw, STATE_CSI_IGNORE, ACTION_NONE);
          return;
        case 0x7f:
          do_trans(raw, STATE_NONE, ACTION_IGNORE);
          return;
      }
      if (raw < 0x20) {
        do_trans(raw, STATE_NONE, ACTION_EXECUTE);
      } else if (raw < 0x30) {
        do_trans(raw, STATE_CSI_INT, ACTION_COLLECT);
      } else if (raw < 0x3c) {
        do_trans(raw, STATE_CSI_PARAM, ACTION_PARAM);
      } else if (raw < 0x40) {
        do_trans(raw, STATE_CSI_PARAM, ACTION_COLLECT);
      } else if (raw < 0x80) {
        do_trans(raw, STATE_GROUND, ACTION_CSI_DISPATCH);
      } else {
        do_trans(raw, STATE_CSI_IGNORE, ACTION_NONE);
      }
      return;
    case STATE_CSI_PARAM:
      switch (raw) {
        case 0x3a:
          do_trans(raw, STATE_CSI_IGNORE, ACTION_NONE);
          return;
        case 0x7f:
          do_trans(raw, STATE_NONE, ACTION_IGNORE);
          return;
      }
      if (raw < 0x20) {
        do_trans(raw, STATE_NONE, ACTION_EXECUTE);
      } else if (raw < 0x30) {
        do_trans(raw, STATE_CSI_INT, ACTION_COLLECT);
      } else if (raw < 0x3c) {
        do_trans(raw, STATE_NONE, ACTION_PARAM);
      } else if (raw >= 0x40 && raw < 0x80) {
        do_trans(raw, STATE_GROUND, ACTION_CSI_DISPATCH);
      } else {
        do_trans(raw, STATE_CSI_IGNORE, ACTION_NONE);
      }
      return;
    case STATE_CSI_INT:
      if (raw < 0x20) {
        do_trans(raw, STATE_NONE, ACTION_EXECUTE);
      } else if (raw < 0x30) {
        do_trans(raw, STATE_NONE, ACTION_COLLECT);
      } else if (raw >= 0x40 && raw < 0x7f) {
        do_trans(raw, STATE_GROUND, ACTION_CSI_DISPATCH);
      } else if (raw == 0x7f) {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      } else {
        do_trans(raw, STATE_CSI_IGNORE, ACTION_NONE);
      }
      return;
    case STATE_CSI_IGNORE:
      if (raw < 0x20) {
        do_trans(raw, STATE_NONE, ACTION_EXECUTE);
      } else if (raw >= 0x40 && raw < 0x7f) {
        do_trans(raw, STATE_GROUND, ACTION_NONE);
      } else {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      }
      return;
    case STATE_DCS_ENTRY:
      switch (raw) {
        case 0x3a:
          do_trans(raw, STATE_DCS_IGNORE, ACTION_NONE);
          return;
        case 0x7f:
          do_trans(raw, STATE_NONE, ACTION_IGNORE);
          return;
      }
      if (raw < 0x20) {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      } else if (raw < 0x30) {
        do_trans(raw, STATE_DCS_INT, ACTION_COLLECT);
      } else if (raw < 0x3c) {
        do_trans(raw, STATE_DCS_PARAM, ACTION_PARAM);
      } else if (raw < 0x40) {
        do_trans(raw, STATE_DCS_PARAM, ACTION_COLLECT);
      } else {
        do_trans(raw, STATE_DCS_PASS, ACTION_NONE);
      }
      return;
    case STATE_DCS_PARAM:
      if (raw < 0x20 || raw == 0x7f) {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      } else if (raw < 0x30) {
        do_trans(raw, STATE_DCS_INT, ACTION_COLLECT);
      } else if (raw < 0x3a || raw == 0x3b) {
        do_trans(raw, STATE_NONE, ACTION_PARAM);
      } else if (raw < 0x40) {
        do_trans(raw, STATE_DCS_IGNORE, ACTION_NONE);
      } else {
        do_trans(raw, STATE_DCS_PASS, ACTION_NONE);
      }
      return;
    case STATE_DCS_INT:
      if (raw < 0x20 || raw == 0x7f) {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      } else if (raw < 0x30) {
        do_trans(raw, STATE_NONE, ACTION_COLLECT);
      } else if (raw < 0x40) {
        do_trans(raw, STATE_DCS_IGNORE, ACTION_NONE);
      } else {
        do_trans(raw, STATE_DCS_PASS, ACTION_NONE);
      }
      return;
    case STATE_DCS_PASS:
      if (raw == 0x7f) {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      } else {
        do_trans(raw, STATE_NONE, ACTION_DCS_COLLECT);
      }
      return;
    case STATE_OSC_STRING:
      if (raw < 0x20) {
        do_trans(raw, STATE_NONE, ACTION_IGNORE);
      } else {
        do_trans(raw, STATE_NONE, ACTION_OSC_COLLECT);
      }
      return;
    case STATE_DCS_IGNORE:
    case STATE_ST_IGNORE:
      do_trans(raw, STATE_NONE, ACTION_IGNORE);
      return;
    case STATE_NONE:
      // do nothing
      ;
  }

//  llog_warn(vte, "unhandled input %u in state %d", raw, _state);
}

/* perform state transition and dispatch related actions */
void Vte::do_trans(char32_t data, ParserState state, ParserAction act) {
  if (state != STATE_NONE) {
    /* A state transition occurs. Perform exit-action,
     * transition-action and entry-action. Even when performing a
     * transition to the same state as the current state we do this.
     * Use STATE_NONE if this is not the desired behavior.
     */
    do_action(data, exit_action(_state));
    do_action(data, act);
    do_action(data, entry_action(_state));
    _state = state;
  } else {
    do_action(data, act);
  }
}

/* perform parser action */
void Vte::do_action(char32_t data, ParserAction action) {
//  tsm_symbol_t sym;

  switch (action) {
    case ACTION_NONE:
      /* do nothing */
      return;
    case ACTION_IGNORE:
      /* ignore character */
      break;
    case ACTION_PRINT:
      write_console(map_char(data));
      break;
    case ACTION_EXECUTE:
//      do_execute(vte, data);
      break;
    case ACTION_CLEAR:
//      do_clear(vte);
      break;
    case ACTION_COLLECT:
//      do_collect(vte, data);
      break;
    case ACTION_PARAM:
//      do_param(vte, data);
      break;
    case ACTION_ESC_DISPATCH:
//      do_esc(vte, data);
      break;
    case ACTION_CSI_DISPATCH:
//      do_csi(vte, data);
      break;
    case ACTION_DCS_START:
      break;
    case ACTION_DCS_COLLECT:
      break;
    case ACTION_DCS_END:
      break;
    case ACTION_OSC_START:
      break;
    case ACTION_OSC_COLLECT:
      break;
    case ACTION_OSC_END:
      break;
//    default:
//      llog_warn(vte, "invalid action %d", action);
  }
}

// map a character according to current GL and GR maps
char32_t Vte::map_char(char32_t val) {
  // characters 33..126 use the gl map, 161..254 the gr map,
  // all others unity
  if (val >= 33 && val <= 126) {
    if (_glt) {
      val = (*_glt)[val - 32];
      _glt = nullptr;
      return val;
    } else {
      return (*_gl)[val - 32];
    }
  } else if (val >= 161 && val <= 254) {
    if (_grt) {
      val = (*_grt)[val - 160];
      _grt = nullptr;
      return val;
    } else {
      return (*_gr)[val - 160];
    }
  } else {
    return val;
  }
}

// write to console
void Vte::write_console(char32_t sym) {
	_screen.write(sym, &_attr);
}

void Vte::reset() {
  // reset flags
  _flags = FLAG_TEXT_CURSOR_MODE
      | FLAG_AUTO_REPEAT_MODE
      | FLAG_SEND_RECEIVE_MODE
      | FLAG_AUTO_WRAP_MODE
      | FLAG_BACKGROUND_COLOR_ERASE_MODE;
  
  // reset screen
  _screen.reset();
  _screen.set_flags(screen::SCREEN_AUTO_WRAP);

  // reset the utf8 state machine
  _utf8_converter.reset();
  
  // reset this state machine
  _state = STATE_GROUND;
  
  // reset the charsets
  _gl = &charsets::unicode_lower;
  _gr = &charsets::unicode_upper;
  _glt = nullptr;
  _grt = nullptr;
  _g0 = &charsets::unicode_lower;
  _g1 = &charsets::unicode_upper;
  _g2 = &charsets::unicode_lower;
  _g3 = &charsets::unicode_upper;
  
  // initialize with the default attributes
  _attr = _screen.default_attr();

  // reset the saved state
  reset_state();
}

void Vte::reset_state() {
//	vte->saved_state.cursor_x = 0;
//	vte->saved_state.cursor_y = 0;
//	vte->saved_state.origin_mode = false;
//	vte->saved_state.wrap_mode = true;
//	vte->saved_state.gl = &vte->g0;
//	vte->saved_state.gr = &vte->g1;
//
//	copy_fcolor(&vte->saved_state.cattr, &vte->def_attr);
//	copy_bcolor(&vte->saved_state.cattr, &vte->def_attr);
//	vte->saved_state.cattr.bold = 0;
//	vte->saved_state.cattr.underline = 0;
//	vte->saved_state.cattr.inverse = 0;
//	vte->saved_state.cattr.protect = 0;
//	vte->saved_state.cattr.blink = 0;
}

} // namespace vte
} // namespace vtutils
