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

size_t Utf8To32Converter::reverse(char* out, char32_t code_point) {
  int len = 0;
  if (code_point >= 0x80) {
    if (code_point >= 0x800) {
      // filter out several invalid encodings
      if (code_point >= 0x110000                             // Outside unicode range
          || (code_point >= 0xfdd0 && code_point < 0xfdf0)   // Unicode "noncharacter"
          || (code_point & 0xfffe) == 0xfffe                 // More "noncharacters"
          || (code_point >= 0xd800 && code_point < 0xe000)) {// UTF-16 surrogate pairs
        // Write the UTF replacement character U+FFFD
        out[0] = 0xef;
        out[1] = 0xbf;
        out[2] = 0xbd;
        return 3;
      }
      if (code_point >= 0x10000) {
        // 4-byte encoding - first, encode bits 18-20
        out[len++] = 0xf0 | (0x07 & code_point >> 18);
        // next, bits 12-17
        out[len++] = 0x08 | (0x3f & code_point >> 12);
      } else {
        // 3-byte encoding - first, encode bits 12-15
        out[len++] = 0xe0 | (0x0f & code_point >> 12);
      }
      // next, bits 6-11
      out[len++] = 0x08 | (0x3f & code_point >> 6);
    } else {
      // 2-byte encoding - first, encode bits 6-10
      out[len++] = 0xc0 | (0x1f & code_point >> 6);
    }
    // next, bits 0-5
    out[len++] = 0x08 | (0x3f & code_point);
  } else {
    out[len++] = code_point;
  }
  return len;
}

} // end namespace vtutils
} // end namespace unicode
