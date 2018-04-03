#include "unicode.h"

#include <string>

namespace vtutils {
namespace unicode {
  
namespace {
static const char32_t UTF_REPLACEMENT = 0xFFFD;
}

bool Utf8To32Converter::put(char ch, char32_t &code_point) {
  char32_t c = ch;
  
  switch (_state) {
    case UTF8_START:
      if (c == 0xC0 || c == 0xC1) {
        // overlong encoding for ASCII, reject 
        return reject(code_point);
      } else if ((c & 0x80) == 0) {
        // single byte, accept
        code_point = c;
        return true;
      } else if ((c & 0xC0) == 0x80) {
        // parser out of sync, ignore byte 
      } else if ((c & 0xE0) == 0xC0) {
        // start of two byte sequence 
        _code_point = (c & 0x1F) << 6;
        _state = UTF8_EXPECT1;
      } else if ((c & 0xF0) == 0xE0) {
        // start of three byte sequence 
        _code_point = (c & 0x0F) << 12;
        _state = UTF8_EXPECT2;
      } else if ((c & 0xF8) == 0xF0) {
        // start of four byte sequence 
        _code_point = (c & 0x07) << 18;
        _state = UTF8_EXPECT3;
      } else {
        // overlong encoding, reject 
        return reject(code_point);
      }
      break;
	case UTF8_EXPECT3:
      if ((c & 0xC0) != 0x80) {
        // invalid continuation character
        return reject(code_point);
      }
      _code_point |= (c & 0x3F) << 12;
      _state = UTF8_EXPECT2;
      break;
	case UTF8_EXPECT2:
      if ((c & 0xC0) != 0x80) {
        // invalid continuation character
        return reject(code_point);
      }
      _code_point |= (c & 0x3F) << 6;
      _state = UTF8_EXPECT1;
      break;
	case UTF8_EXPECT1:
      if ((c & 0xC0) != 0x80) {
        // invalid continuation character
        return reject(code_point);
      }
      code_point = _code_point | (c & 0x3F);
      reset();
      return true;
  }

  return false;
}

bool Utf8To32Converter::reject(char32_t &code_point) {
  code_point = UTF_REPLACEMENT;
  reset();
  return true;
}

void Utf8To32Converter::reset() {
  _state = UTF8_START;
  _code_point = 0;
}

} // end namespace vtutils
} // end namespace unicode
