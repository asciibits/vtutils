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

// CSI flags
static const unsigned int CSI_BANG    = 0x0001;    // CSI: !
static const unsigned int CSI_CASH    = 0x0002;    // CSI: $
static const unsigned int CSI_WHAT    = 0x0004;    // CSI: ?
static const unsigned int CSI_GT      = 0x0008;    // CSI: >
static const unsigned int CSI_SPACE   = 0x0010;    // CSI:  
static const unsigned int CSI_SQUOTE  = 0x0020;    // CSI: '
static const unsigned int CSI_DQUOTE  = 0x0040;    // CSI: "
static const unsigned int CSI_MULT    = 0x0080;    // CSI: *
static const unsigned int CSI_PLUS    = 0x0100;    // CSI: +
static const unsigned int CSI_POPEN   = 0x0200;    // CSI: (
static const unsigned int CSI_PCLOSE  = 0x0400;    // CSI: )

//// terminal flags
//// DEC cursor key mode
static const unsigned int FLAG_CURSOR_KEY_MODE             = 0x00000001;
//// DEC keypad application mode; TODO: toggle on numlock?
static const unsigned int FLAG_KEYPAD_APPLICATION_MODE     = 0x00000002;
//// DEC line-feed/new-line mode
static const unsigned int FLAG_LINE_FEED_NEW_LINE_MODE     = 0x00000004;
// Disable UTF-8 mode and enable 8bit compatible mode
static const unsigned int FLAG_8BIT_MODE                   = 0x00000008;
// Disable 8bit mode and use 7bit compatible mode
static const unsigned int FLAG_7BIT_MODE                   = 0x00000010;
//// Explicitly use 8bit C1 codes; TODO: implement
static const unsigned int FLAG_USE_C1                      = 0x00000020;
//// Disable keyboard; TODO: implement?
static const unsigned int FLAG_KEYBOARD_ACTION_MODE        = 0x00000040;
//// Enable insert mode
static const unsigned int FLAG_INSERT_REPLACE_MODE         = 0x00000080;
//// Disable local echo
static const unsigned int FLAG_SEND_RECEIVE_MODE           = 0x00000100;
//// Show cursor
static const unsigned int FLAG_TEXT_CURSOR_MODE            = 0x00000200;
//// Inverse colors
static const unsigned int FLAG_INVERSE_SCREEN_MODE         = 0x00000400;
//// Relative origin for cursor
static const unsigned int FLAG_ORIGIN_MODE                 = 0x00000800;
//// Auto line wrap mode
static const unsigned int FLAG_AUTO_WRAP_MODE              = 0x00001000;
//// Auto repeat key press; TODO: implement
static const unsigned int FLAG_AUTO_REPEAT_MODE            = 0x00002000;
//// Send keys from nation charsets; TODO: implement
static const unsigned int FLAG_NATIONAL_CHARSET_MODE       = 0x00004000;
//// Set background color on erase (bce)
static const unsigned int FLAG_BACKGROUND_COLOR_ERASE_MODE = 0x00008000;
//// Prepend escape character to next output
static const unsigned int FLAG_PREPEND_ESCAPE              = 0x00010000;
//// Prevent switching to alternate screen buffer
static const unsigned int FLAG_TITE_INHIBIT_MODE           = 0x00020000;
}

std::istream& operator>>(std::istream &in, Vte &vte) {
  char c;
  while (in.get(c)) {
    vte.input(c);
  }
  return in;
}

void Vte::input(std::string s) {
  for (char c : s) {
    input(c);
  }
}
void Vte::input(char c) {
  ++_parse_cnt;
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
  --_parse_cnt;
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

// perform state transition and dispatch related actions
void Vte::do_trans(char32_t data, ParserState state, ParserAction act) {
  if (state != STATE_NONE) {
    // A state transition occurs. Perform exit-action,
    // transition-action and entry-action. Even when performing a
    // transition to the same state as the current state we do this.
    // Use STATE_NONE if this is not the desired behavior.
    do_action(data, exit_action(_state));
    do_action(data, act);
    do_action(data, entry_action(_state));
    _state = state;
  } else {
    do_action(data, act);
  }
}

// perform parser action
void Vte::do_action(char32_t data, ParserAction action) {
  switch (action) {
    case ACTION_NONE:
      // do nothing
      return;
    case ACTION_IGNORE:
      // ignore character
      break;
    case ACTION_PRINT:
      write_console(map_char(data));
      break;
    case ACTION_EXECUTE:
      do_execute(data);
      break;
    case ACTION_CLEAR:
      do_clear();
      break;
    case ACTION_COLLECT:
      do_collect(data);
      break;
    case ACTION_PARAM:
      do_param(data);
      break;
    case ACTION_ESC_DISPATCH:
      do_esc(data);
      break;
    case ACTION_CSI_DISPATCH:
      do_csi(data);
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

void Vte::do_execute(char32_t ctrl) {
  switch (ctrl) {
    case 0x00: // NUL
      // Ignore on input
      break;
    case 0x05: // ENQ
      // Transmit answerback message
      // TODO: is there a better answer than ACK?
      write("\x06");
      break;
    case 0x07: // BEL
      // Sound bell tone
      _screen.alert();
      break;
    case 0x08: // BS
      // Move cursor one position left
      _screen.move_left(1);
      break;
    case 0x09: // HT
      // Move to next tab stop or end of line
      _screen.tab_right(1);
      break;
    case 0x0a: // LF
    case 0x0b: // VT
    case 0x0c: // FF
      // Line feed or newline (CR/NL mode)
      if (_flags & FLAG_LINE_FEED_NEW_LINE_MODE) {
        _screen.newline();
      } else {
        _screen.move_down(1, true);
      }
      break;
    case 0x0d: // CR
      // Move cursor to left margin
      _screen.move_line_home();
      break;
    case 0x0e: // SO
      // Map G1 character set into GL
      _gl = _g1;
      break;
    case 0x0f: // SI
      // Map G0 character set into GL
      _gl = _g0;
      break;
    case 0x11: // XON
      // Resume transmission
      // TODO
      break;
    case 0x13: // XOFF
      // Stop transmission
      // TODO
      break;
    case 0x18: // CAN
      // Cancel escape sequence
      // nothing to do here
      break;
    case 0x1a: // SUB
      // Discard current escape sequence and show err-sym
      write_console(0xbf);
      break;
    case 0x1b: // ESC
      // Invokes an escape sequence
      // nothing to do here
      break;
    case 0x1f: // DEL
      // Ignored
      break;
    case 0x84: // IND
      // Move down one row, perform scroll-up if needed
      _screen.move_down(1, true);
      break;
    case 0x85: // NEL
      // CR/NL with scroll-up if needed
      _screen.newline();
      break;
    case 0x88: // HTS
      // Set tab stop at current position
      _screen.set_tabstop();
      break;
    case 0x8d: // RI
      // Move up one row, perform scroll-down if needed
      _screen.move_up(1, true);
      break;
    case 0x8e: // SS2
      // Temporarily map G2 into GL for next char only
      _glt = _g2;
      break;
    case 0x8f: // SS3
      // Temporarily map G3 into GL for next char only
      _glt = _g3;
      break;
    case 0x9a: // DECID
      // Send device attributes response like ANSI DA
      send_primary_da();
      break;
    case 0x9c: // ST
      // End control string
      // nothing to do here
      break;
//    default:
//      llog_debug(vte, "unhandled control char %u", ctrl);
  }
}

void Vte::do_collect(char32_t data) {
  switch (data) {
    case '!':
      _csi_flags |= CSI_BANG;
      break;
    case '$':
      _csi_flags |= CSI_CASH;
      break;
    case '?':
      _csi_flags |= CSI_WHAT;
      break;
    case '>':
      _csi_flags |= CSI_GT;
      break;
    case ' ':
      _csi_flags |= CSI_SPACE;
      break;
    case '\'':
      _csi_flags |= CSI_SQUOTE;
      break;
    case '"':
      _csi_flags |= CSI_DQUOTE;
      break;
    case '*':
      _csi_flags |= CSI_MULT;
      break;
    case '+':
      _csi_flags |= CSI_PLUS;
      break;
    case '(':
      _csi_flags |= CSI_POPEN;
      break;
    case ')':
      _csi_flags |= CSI_PCLOSE;
      break;
  }
}

void Vte::do_param(char32_t data) {
  int param;

  if (data == ';') {
    if (_csi_argc < CSI_ARG_MAX) {
      _csi_argc++;
    }
    return;
  }

  if (_csi_argc >= CSI_ARG_MAX) {
    return;
  }

  // avoid integer overflows; max allowed value is 16384 anyway
  if (_csi_argv[_csi_argc] > 0xffff) {
    return;
  }

  if (data >= '0' && data <= '9') {
    param = _csi_argv[_csi_argc];
    if (param <= 0) {
      param = data - '0';
    } else {
      param = param * 10 + data - '0';
    }
    _csi_argv[_csi_argc] = param;
  }
}

void Vte::do_esc(char32_t data) {
  switch (data) {
    case 'B': /* map ASCII into G0-G3 */
      if (set_charset(&charsets::unicode_lower))
        return;
      break;
    case '<': /* map DEC supplemental into G0-G3 */
      if (set_charset(&charsets::dec_supplemental_graphics))
        return;
      break;
    case '0': /* map DEC special into G0-G3 */
      if (set_charset(&charsets::dec_special_graphics))
        return;
      break;
    case 'A': /* map British into G0-G3 */
      /* TODO: create British charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case '4': /* map Dutch into G0-G3 */
      /* TODO: create Dutch charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'C':
    case '5': /* map Finnish into G0-G3 */
      /* TODO: create Finnish charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'R': /* map French into G0-G3 */
      /* TODO: create French charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'Q': /* map French-Canadian into G0-G3 */
      /* TODO: create French-Canadian charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'K': /* map German into G0-G3 */
      /* TODO: create German charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'Y': /* map Italian into G0-G3 */
      /* TODO: create Italian charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'E':
    case '6': /* map Norwegian/Danish into G0-G3 */
      /* TODO: create Norwegian/Danish charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'Z': /* map Spanish into G0-G3 */
      /* TODO: create Spanish charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'H':
    case '7': /* map Swedish into G0-G3 */
      /* TODO: create Swedish charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case '=': /* map Swiss into G0-G3 */
      /* TODO: create Swiss charset from DEC */
      if (set_charset(&charsets::unicode_upper))
        return;
      break;
    case 'F':
      if (_csi_flags & CSI_SPACE) {
        /* S7C1T */
        /* Disable 8bit C1 mode */
        _flags &= ~FLAG_USE_C1;
        return;
      }
      break;
    case 'G':
      if (_csi_flags & CSI_SPACE) {
        /* S8C1T */
        /* Enable 8bit C1 mode */
        _flags |= FLAG_USE_C1;
        return;
      }
      break;
  }

  /* everything below is only valid without CSI flags */
  if (_csi_flags) {
//    llog_debug("unhandled escape seq %u", data);
    return;
  }

  switch (data) {
    case 'D': /* IND */
      /* Move down one row, perform scroll-up if needed */
      _screen.move_down(1, true);
      break;
    case 'E': /* NEL */
      /* CR/NL with scroll-up if needed */
      _screen.newline();
      break;
    case 'H': /* HTS */
      /* Set tab stop at current position */
      _screen.set_tabstop();
      break;
    case 'M': /* RI */
      /* Move up one row, perform scroll-down if needed */
      _screen.move_up(1, true);
      break;
    case 'N': /* SS2 */
      /* Temporarily map G2 into GL for next char only */
      _glt = _g2;
      break;
    case 'O': /* SS3 */
      /* Temporarily map G3 into GL for next char only */
      _glt = _g3;
      break;
    case 'Z': /* DECID */
      /* Send device attributes response like ANSI DA */
      send_primary_da();
      break;
    case '\\': /* ST */
      /* End control string */
      /* nothing to do here */
      break;
    case '~': /* LS1R */
      /* Invoke G1 into GR */
      _gr = _g1;
      break;
    case 'n': /* LS2 */
      /* Invoke G2 into GL */
      _gl = _g2;
      break;
    case '}': /* LS2R */
      /* Invoke G2 into GR */
      _gr = _g2;
      break;
    case 'o': /* LS3 */
      /* Invoke G3 into GL */
      _gl = _g3;
      break;
    case '|': /* LS3R */
      /* Invoke G3 into GR */
      _gr = _g3;
      break;
    case '=': /* DECKPAM */
      /* Set application keypad mode */
      _flags |= FLAG_KEYPAD_APPLICATION_MODE;
      break;
    case '>': /* DECKPNM */
      /* Set numeric keypad mode */
      _flags &= ~FLAG_KEYPAD_APPLICATION_MODE;
      break;
    case 'c': /* RIS */
      /* hard reset */
      hard_reset();
      break;
    case '7': /* DECSC */
      /* save console state */
      save_state();
      break;
    case '8': /* DECRC */
      /* restore console state */
      restore_state();
      break;
      //	default:
      //		llog_debug(vte, "unhandled escape seq %u", data);
  }
}

void Vte::do_csi(char32_t data) {
	int num, x, y, upper, lower;
	bool protect;

	if (_csi_argc < CSI_ARG_MAX)
		_csi_argc++;

	switch (data) {
	case 'A': /* CUU */
		/* move cursor up */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.move_up(num, false);
		break;
	case 'B': /* CUD */
		/* move cursor down */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.move_down(num, false);
		break;
	case 'C': /* CUF */
		/* move cursor forward */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.move_right(num);
		break;
	case 'D': /* CUB */
		/* move cursor backward */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.move_left(num);
		break;
	case 'd': /* VPA */
		/* Vertical Line Position Absolute */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		x = _screen.get_cursor_x();
		_screen.move_to(x, num - 1);
		break;
	case 'e': /* VPR */
		/* Vertical Line Position Relative */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		x = _screen.get_cursor_x();
		y = _screen.get_cursor_y();
		_screen.move_to(x, y + num);
		break;
	case 'H': /* CUP */
	case 'f': /* HVP */
		/* position cursor */
		x = _csi_argv[0];
		if (x <= 0)
			x = 1;
		y = _csi_argv[1];
		if (y <= 0)
			y = 1;
		_screen.move_to(y - 1, x - 1);
		break;
	case 'G': /* CHA */
		/* Cursor Character Absolute */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		y = _screen.get_cursor_y();
		_screen.move_to(num - 1, y);
		break;
	case 'J':
		if (_csi_flags & CSI_WHAT)
			protect = true;
		else
			protect = false;

		if (_csi_argv[0] <= 0)
			_screen.erase_cursor_to_screen(protect);
		else if (_csi_argv[0] == 1)
			_screen.erase_screen_to_cursor(protect);
		else if (_csi_argv[0] == 2)
			_screen.erase_screen(protect);
//		else
//			llog_debug(vte, "unknown parameter to CSI-J: %d",
//				   _csi_argv[0]);
		break;
	case 'K':
		if (_csi_flags & CSI_WHAT)
			protect = true;
		else
			protect = false;

		if (_csi_argv[0] <= 0)
			_screen.erase_cursor_to_end(protect);
		else if (_csi_argv[0] == 1)
			_screen.erase_home_to_cursor(protect);
		else if (_csi_argv[0] == 2)
			_screen.erase_current_line(protect);
//		else
//			llog_debug(vte, "unknown parameter to CSI-K: %d",
//				   _csi_argv[0]);
		break;
	case 'X': /* ECH */
		/* erase characters */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.erase_chars(num);
		break;
	case 'm':
		csi_attribute();
		break;
	case 'p':
		if (_csi_flags & CSI_GT) {
			/* xterm: select X11 visual cursor mode */
			csi_soft_reset();
		} else if (_csi_flags & CSI_BANG) {
			/* DECSTR: Soft Reset */
			csi_soft_reset();
		} else if (_csi_flags & CSI_CASH) {
			/* DECRQM: Request DEC Private Mode */
			/* If CSI_WHAT is set, then enable,
			 * otherwise disable */
			csi_soft_reset();
		} else {
			/* DECSCL: Compatibility Level */
			/* Sometimes CSI_DQUOTE is set here, too */
			csi_compat_mode();
		}
		break;
	case 'h': /* SM: Set Mode */
		csi_mode(true);
		break;
	case 'l': /* RM: Reset Mode */
		csi_mode(false);
		break;
	case 'r': /* DECSTBM */
		/* set margin size */
		upper = _csi_argv[0];
		if (upper < 0) {
			upper = 0;
        }
		lower = _csi_argv[1];
		if (lower < 0) {
			lower = 0;
        }
		_screen.set_margins(upper, lower);
		break;
	case 'c': /* DA */
		/* device attributes */
		csi_dev_attr();
		break;
	case 'L': /* IL */
		/* insert lines */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.insert_lines(num);
		break;
	case 'M': /* DL */
		/* delete lines */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.delete_lines(num);
		break;
	case 'g': /* TBC */
		/* tabulation clear */
		num = _csi_argv[0];
		if (num <= 0) {
			_screen.reset_tabstop();
		} else if (num == 3) {
			_screen.reset_all_tabstops();
        }
//		else
//			llog_debug(vte, "invalid parameter %d to TBC CSI", num);
		break;
	case '@': /* ICH */
		/* insert characters */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.insert_chars(num);
		break;
	case 'P': /* DCH */
		/* delete characters */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.delete_chars(num);
		break;
	case 'Z': /* CBT */
		/* cursor horizontal backwards tab */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.tab_left(num);
		break;
	case 'I': /* CHT */
		/* cursor horizontal forward tab */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.tab_right(num);
		break;
	case 'n': /* DSR */
		/* device status reports */
		csi_dsr();
		break;
	case 'S': /* SU */
		/* scroll up */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.scroll_up(num);
		break;
	case 'T': /* SD */
		/* scroll down */
		num = _csi_argv[0];
		if (num <= 0)
			num = 1;
		_screen.scroll_down(num);
		break;
//	default:
//		llog_debug(vte, "unhandled CSI sequence %c", data);
	}
}

void Vte::csi_attribute() {
  static const uint8_t bval[6] = {0x00, 0x5f, 0x87, 0xaf, 0xd7, 0xff};
  unsigned int i, code;

  if (_csi_argc <= 1 && _csi_argv[0] == -1) {
    _csi_argc = 1;
    _csi_argv[0] = 0;
  }

  for (i = 0; i < _csi_argc; ++i) {
    switch (_csi_argv[i]) {
      case -1:
        break;
      case 0:
        _attr = _screen.default_attr();
        _attr.bold = 0;
        _attr.underline = 0;
        _attr.inverse = 0;
        _attr.blink = 0;
        break;
      case 1:
        _attr.bold = 1;
        break;
      case 4:
        _attr.underline = 1;
        break;
      case 5:
        _attr.blink = 1;
        break;
      case 7:
        _attr.inverse = 1;
        break;
      case 22:
        _attr.bold = 0;
        break;
      case 24:
        _attr.underline = 0;
        break;
      case 25:
        _attr.blink = 0;
        break;
      case 27:
        _attr.inverse = 0;
        break;
      case 30:
        _attr.fg.color_code = screen::COLOR_BLACK;
        break;
      case 31:
        _attr.fg.color_code = screen::COLOR_RED;
        break;
      case 32:
        _attr.fg.color_code = screen::COLOR_GREEN;
        break;
      case 33:
        _attr.fg.color_code = screen::COLOR_YELLOW;
        break;
      case 34:
        _attr.fg.color_code = screen::COLOR_BLUE;
        break;
      case 35:
        _attr.fg.color_code = screen::COLOR_MAGENTA;
        break;
      case 36:
        _attr.fg.color_code = screen::COLOR_CYAN;
        break;
      case 37:
        _attr.fg.color_code = screen::COLOR_LIGHT_GREY;
        break;
      case 39:
        _attr.fg = _screen.default_attr().fg;
        break;
      case 40:
        _attr.bg.color_code = screen::COLOR_BLACK;
        break;
      case 41:
        _attr.bg.color_code = screen::COLOR_RED;
        break;
      case 42:
        _attr.bg.color_code = screen::COLOR_GREEN;
        break;
      case 43:
        _attr.bg.color_code = screen::COLOR_YELLOW;
        break;
      case 44:
        _attr.bg.color_code = screen::COLOR_BLUE;
        break;
      case 45:
        _attr.bg.color_code = screen::COLOR_MAGENTA;
        break;
      case 46:
        _attr.bg.color_code = screen::COLOR_CYAN;
        break;
      case 47:
        _attr.bg.color_code = screen::COLOR_LIGHT_GREY;
        break;
      case 49:
        _attr.bg = _screen.default_attr().bg;
        break;
      case 90:
        _attr.fg.color_code = screen::COLOR_DARK_GREY;
        break;
      case 91:
        _attr.fg.color_code = screen::COLOR_LIGHT_RED;
        break;
      case 92:
        _attr.fg.color_code = screen::COLOR_LIGHT_GREEN;
        break;
      case 93:
        _attr.fg.color_code = screen::COLOR_LIGHT_YELLOW;
        break;
      case 94:
        _attr.fg.color_code = screen::COLOR_LIGHT_BLUE;
        break;
      case 95:
        _attr.fg.color_code = screen::COLOR_LIGHT_MAGENTA;
        break;
      case 96:
        _attr.fg.color_code = screen::COLOR_LIGHT_CYAN;
        break;
      case 97:
        _attr.fg.color_code = screen::COLOR_WHITE;
        break;
      case 100:
        _attr.bg.color_code = screen::COLOR_DARK_GREY;
        break;
      case 101:
        _attr.bg.color_code = screen::COLOR_LIGHT_RED;
        break;
      case 102:
        _attr.bg.color_code = screen::COLOR_LIGHT_GREEN;
        break;
      case 103:
        _attr.bg.color_code = screen::COLOR_LIGHT_YELLOW;
        break;
      case 104:
        _attr.bg.color_code = screen::COLOR_LIGHT_BLUE;
        break;
      case 105:
        _attr.bg.color_code = screen::COLOR_LIGHT_MAGENTA;
        break;
      case 106:
        _attr.bg.color_code = screen::COLOR_LIGHT_CYAN;
        break;
      case 107:
        _attr.bg.color_code = screen::COLOR_WHITE;
        break;
      case 38:
        // fallthrough
      case 48:
        if (i + 2 >= _csi_argc ||
            _csi_argv[i + 1] != 5 ||
            _csi_argv[i + 2] < 0) {
//          llog_debug(vte, "invalid 256color SGR");
          break;
        }

        code = _csi_argv[i + 2];
        if (_csi_argv[i] == 38) {
          if (code < 16) {
            _attr.fg.color_code = (screen::ColorCode) code;
          } else if (code < 232) {
            _attr.fg.color_code = screen::COLOR_RGB;
            code -= 16;
            _attr.fg.b = bval[code % 6];
            code /= 6;
            _attr.fg.g = bval[code % 6];
            code /= 6;
            _attr.fg.r = bval[code % 6];
          } else {
            _attr.fg.color_code = screen::COLOR_RGB;
            code = (code - 232) * 10 + 8;
            _attr.fg.r = code;
            _attr.fg.g = code;
            _attr.fg.b = code;
          }
        } else {
          if (code < 16) {
            _attr.bg.color_code = (screen::ColorCode) code;
          } else if (code < 232) {
            _attr.bg.color_code = screen::COLOR_RGB;
            code -= 16;
            _attr.bg.b = bval[code % 6];
            code /= 6;
            _attr.bg.g = bval[code % 6];
            code /= 6;
            _attr.bg.r = bval[code % 6];
          } else {
            _attr.bg.color_code = screen::COLOR_RGB;
            code = (code - 232) * 10 + 8;
            _attr.bg.r = code;
            _attr.bg.g = code;
            _attr.bg.b = code;
          }
        }

        i += 2;
        break;
//      default:
//        llog_debug(vte, "unhandled SGR attr %i",
//            _csi_argv[i]);
    }
  }

  if ((_flags & FLAG_BACKGROUND_COLOR_ERASE_MODE) != 0) {
    _screen.set_def_attr(_attr);
  }
}

void Vte::csi_soft_reset() {
  reset();
}

void Vte::csi_compat_mode() {
  // always perform soft reset
  csi_soft_reset();

  if (_csi_argv[0] == 61) {
    // Switching to VT100 compatibility mode. We do
    // not support this mode, so ignore it. In fact,
    // we are almost compatible to it, anyway, so
    // there is no need to explicitly select it.
    // However, we enable 7bit mode to avoid
    // character-table problems
    _flags |= FLAG_7BIT_MODE;
    _g0 = &charsets::unicode_lower;
    _g1 = &charsets::dec_supplemental_graphics;
  } else if (_csi_argv[0] == 62 ||
      _csi_argv[0] == 63 ||
      _csi_argv[0] == 64) {
    // Switching to VT2/3/4 compatibility mode. We
    // are always compatible with this so ignore it.
    // We always send 7bit controls so we also do
    // not care for the parameter value here that
    // select the control-mode.
    // VT220 defines argument 2 as 7bit mode but
    // VT3xx up to VT5xx use it as 8bit mode. We
    // choose to conform with the latter here.
    // We also enable 8bit mode when VT220
    // compatibility is requested explicitly.
    if (_csi_argv[1] == 1 || _csi_argv[1] == 2) {
      _flags |= FLAG_USE_C1;
    }

    _flags |= FLAG_8BIT_MODE;
    _g0 = &charsets::unicode_lower;
    _g1 = &charsets::dec_supplemental_graphics;
  } else {
//    llog_debug(vte, "unhandled DECSCL 'p' CSI %i, switching to utf-8 mode again",
//        vte->csi_argv[0]);
  }
}

void Vte::csi_mode(bool set) {
  unsigned int i;

  for (i = 0; i < _csi_argc; ++i) {
    if ((_csi_flags & CSI_WHAT) == 0) {
      switch (_csi_argv[i]) {
        case -1:
          continue;
        case 2: // KAM
          set_reset_flag(set, FLAG_KEYBOARD_ACTION_MODE);
          continue;
        case 4: // IRM
          set_reset_flag(set, FLAG_INSERT_REPLACE_MODE);
          if (set) {
            _screen.set_flags(screen::SCREEN_INSERT_MODE);
          } else {
            _screen.reset_flags(screen::SCREEN_INSERT_MODE);
          }
          continue;
        case 12: // SRM
          set_reset_flag(set, FLAG_SEND_RECEIVE_MODE);
          continue;
        case 20: // LNM
          set_reset_flag(set, FLAG_LINE_FEED_NEW_LINE_MODE);
          continue;
        default:
//          llog_debug(vte, "unknown non-DEC (Re)Set-Mode %d",
//              _csi_argv[i]);
          continue;
      }
    }

    switch (_csi_argv[i]) {
      case -1:
        continue;
      case 1: // DECCKM
        set_reset_flag(set, FLAG_CURSOR_KEY_MODE);
        continue;
      case 2: // DECANM
        // Select VT52 mode
        // We do not support VT52 mode. Is there any reason why
        // we should support it? We ignore it here and do not
        // mark it as to-do item unless someone has strong
        // arguments to support it.
        continue;
      case 3: // DECCOLM
        // If set, select 132 column mode, otherwise use 80
        // column mode. If neither is selected explicitly, we
        // use dynamic mode, that is, we send SIGWCH when the
        // size changes and we allow arbitrary buffer
        // dimensions. On soft-reset, we automatically fall back
        // to the default, that is, dynamic mode.
        // Dynamic-mode can be forced to a static mode in the
        // config. That is, every time dynamic-mode becomes
        // active, the terminal will be set to the dimensions
        // that were selected in the config. This allows setting
        // a fixed size for the terminal regardless of the
        // display size.
        // TODO: Implement this
        continue;
      case 4: // DECSCLM
        // Select smooth scrolling. We do not support the
        // classic smooth scrolling because we have a scrollback
        // buffer. There is no need to implement smooth
        // scrolling so ignore this here.
        if (set) {
          _screen.set_flags(screen::SCREEN_SMOOTH_SCROLLING);
        } else {
          _screen.reset_flags(screen::SCREEN_SMOOTH_SCROLLING);
        }
        continue;
      case 5: // DECSCNM
        set_reset_flag(set, FLAG_INVERSE_SCREEN_MODE);
        if (set) {
          _screen.set_flags(screen::SCREEN_INVERSE);
        } else {
          _screen.reset_flags(screen::SCREEN_INVERSE);
        }
        continue;
      case 6: // DECOM
        set_reset_flag(set, FLAG_ORIGIN_MODE);
        if (set) {
          _screen.set_flags(screen::SCREEN_REL_ORIGIN);
        } else {
          _screen.reset_flags(screen::SCREEN_REL_ORIGIN);
        }
        continue;
      case 7: // DECAWN
        set_reset_flag(set, FLAG_AUTO_WRAP_MODE);
        if (set) {
          _screen.set_flags(screen::SCREEN_AUTO_WRAP);
        } else {
          _screen.reset_flags(screen::SCREEN_AUTO_WRAP);
        }
        continue;
      case 8: // DECARM
        set_reset_flag(set, FLAG_AUTO_REPEAT_MODE);
        continue;
      case 12: // blinking cursor
        // TODO: implement
        continue;
      case 18: // DECPFF
        // If set, a form feed (FF) is sent to the printer after
        // every screen that is printed. We don't have printers
        // these days directly attached to terminals so we
        // ignore this here.
        continue;
      case 19: // DECPEX
        // If set, the full screen is printed instead of
        // scrolling region only. We have no printer so ignore
        // this mode.
        continue;
      case 25: // DECTCEM
        set_reset_flag(set, FLAG_TEXT_CURSOR_MODE);
        if (set) {
          _screen.reset_flags(screen::SCREEN_HIDE_CURSOR);
        } else {
          _screen.set_flags(screen::SCREEN_HIDE_CURSOR);
        }
        continue;
      case 42: // DECNRCM
        set_reset_flag(set, FLAG_NATIONAL_CHARSET_MODE);
        continue;
      case 47: // Alternate screen buffer
        if ((_flags & FLAG_TITE_INHIBIT_MODE) != 0) {
          continue;
        }

        if (set) {
          _screen.set_flags(screen::SCREEN_ALTERNATE);
        } else {
          _screen.reset_flags(screen::SCREEN_ALTERNATE);
        }
        continue;
      case 1047: // Alternate screen buffer with post-erase
        if ((_flags & FLAG_TITE_INHIBIT_MODE) != 0) {
          continue;
        }

        if (set) {
          _screen.set_flags(screen::SCREEN_ALTERNATE);
        } else {
          _screen.erase_screen(false);
          _screen.reset_flags(screen::SCREEN_ALTERNATE);
        }
        continue;
      case 1048: // Set/Reset alternate-screen buffer cursor
        if ((_flags & FLAG_TITE_INHIBIT_MODE) != 0) {
          continue;
        }

        if (set) {
          _alt_cursor_x = _screen.get_cursor_x();
          _alt_cursor_y = _screen.get_cursor_y();
        } else {
          _screen.move_to( _alt_cursor_x, _alt_cursor_y);
        }
        continue;
      case 1049: // Alternate screen buffer with pre-erase+cursor
        if ((_flags & FLAG_TITE_INHIBIT_MODE) != 0) {
          continue;
        }

        if (set) {
          _alt_cursor_x = _screen.get_cursor_x();
          _alt_cursor_y = _screen.get_cursor_y();
          _screen.set_flags(screen::SCREEN_ALTERNATE);
          _screen.erase_screen( false);
        } else {
          _screen.reset_flags(screen::SCREEN_ALTERNATE);
          _screen.move_to(_alt_cursor_x, _alt_cursor_y);
        }
        continue;
      default:
//        llog_debug(vte, "unknown DEC %set-Mode %d",
//            set ? "S" : "Res", _csi_argv[i]);
        continue;
    }
  }
}

void Vte::csi_dev_attr() {
  if (_csi_argc <= 1 && _csi_argv[0] <= 0) {
    if (_csi_flags == 0) {
      send_primary_da();
      return;
    } else if ((_csi_flags & CSI_GT) != 0) {
      write("\033[>1;1;0c");
      return;
    }
  }
//
//  llog_debug(vte, "unhandled DA: %x %d %d %d...", _csi_flags,
//      _csi_argv[0], _csi_argv[1], _csi_argv[2]);
}

void Vte::csi_dsr() {
  char buf[64];
  unsigned int x, y, len;

  if (_csi_argv[0] == 5) {
    write("\033[0n");
  } else if (_csi_argv[0] == 6) {
    x = _screen.get_cursor_x();
    y = _screen.get_cursor_y();
    len = snprintf(buf, sizeof (buf), "\033[%u;%uR", x, y);
    if (len >= sizeof (buf)) {
      write("\033[0;0R");
    } else {
      write(buf);
    }
  }
}

void Vte::send_primary_da() {
  write("\033[?60;1;6;9;15c");
}

void Vte::write(const std::string u8) {
//#ifdef BUILD_ENABLE_DEBUG
//	/* in debug mode we check that escape sequences are always <0x7f so they
//	 * are correctly parsed by non-unicode and non-8bit-mode clients. */
//	size_t i;
//
//	if (!raw) {
//		for (i = 0; i < len; ++i) {
//			if (u8[i] & 0x80)
//				llog_warning(vte, "sending 8bit character inline to client in %s:%d",
//					     file, line);
//		}
//	}
//#endif

  /* in local echo mode, directly parse the data again */
  if (_parse_cnt == 0 && (_flags & FLAG_SEND_RECEIVE_MODE) == 0) {
    if ((_flags & FLAG_PREPEND_ESCAPE) != 0) {
      input('\033');
    }
    input(u8);
  }

  _flags &= ~FLAG_PREPEND_ESCAPE;
}

void Vte::do_clear() {
  int i;

  _csi_argc = 0;
  for (i = 0; i < CSI_ARG_MAX; ++i) {
    _csi_argv[i] = -1;
  }
  _csi_flags = 0;
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
  _screen.set_def_attr(_attr);

  // reset the saved state
  reset_state();
}

void Vte::hard_reset() {
  reset();
  _screen.erase_screen(false);
  _screen.move_to(0, 0);
  _screen.hard_reset();
}

bool Vte::set_charset(charsets::charset *set) {
  if (_csi_flags & CSI_POPEN) {
    _g0 = set;
  } else if (_csi_flags & CSI_PCLOSE) {
    _g1 = set;
  } else if (_csi_flags & CSI_MULT) {
    _g2 = set;
  } else if (_csi_flags & CSI_PLUS) {
    _g3 = set;
  } else {
    return false;
  }

  return true;
}

void Vte::reset_state() {
	_saved_state.cursor_x = 0;
	_saved_state.cursor_y = 0;
	_saved_state.origin_mode = false;
	_saved_state.wrap_mode = true;
	_saved_state.gl = _g0;
	_saved_state.gr = _g1;
	_saved_state.attr = _screen.default_attr();
	_saved_state.attr.blink = false;
	_saved_state.attr.bold = false;
	_saved_state.attr.inverse = false;
	_saved_state.attr.protect = false;
	_saved_state.attr.underline = false;
}

void Vte::save_state() {
	_saved_state.cursor_x = _screen.get_cursor_x();
	_saved_state.cursor_y = _screen.get_cursor_y();
	_saved_state.attr = _attr;
	_saved_state.gl = _gl;
	_saved_state.gr = _gr;
	_saved_state.wrap_mode = _flags & FLAG_AUTO_WRAP_MODE;
	_saved_state.origin_mode = _flags & FLAG_ORIGIN_MODE;
}

void Vte::restore_state() {
  _screen.move_to(_saved_state.cursor_x, _saved_state.cursor_y);
  _attr = _saved_state.attr;
  if (_flags & FLAG_BACKGROUND_COLOR_ERASE_MODE) {
    _screen.set_def_attr(_attr);
  }
  _gl = _saved_state.gl;
  _gr = _saved_state.gr;

  if (_saved_state.wrap_mode) {
    _flags |= FLAG_AUTO_WRAP_MODE;
    _screen.set_flags(screen::SCREEN_AUTO_WRAP);
  } else {
    _flags &= ~FLAG_AUTO_WRAP_MODE;
    _screen.reset_flags(screen::SCREEN_AUTO_WRAP);
  }

  if (_saved_state.origin_mode) {
    _flags |= FLAG_ORIGIN_MODE;
    _screen.set_flags(screen::SCREEN_REL_ORIGIN);
  } else {
    _flags &= ~FLAG_ORIGIN_MODE;
    _screen.reset_flags(screen::SCREEN_REL_ORIGIN);
  }
}

} // namespace vte
} // namespace vtutils
