#ifndef VTUTILS_UNICODE_H_
#define VTUTILS_UNICODE_H_

#include <memory>

namespace vtutils {
namespace unicode {

enum utf8_parse_state {
	UTF8_START,
	UTF8_EXPECT1,
	UTF8_EXPECT2,
	UTF8_EXPECT3,
};

/*
 * UTF8 State Machine
 * This state machine parses UTF8 and converts it into a stream of Unicode
 * characters (UTF32 values).
 *
 * Internally, we use UTF8_START whenever the state-machine is reset. This
 * can be used to ignore the last read input or to simply reset the machine.
 * UTF8_EXPECT* is used to remember how many bytes are still to be read to
 * get a full UTF8 sequence.
 * If an error occurs during reading, the user will receive the replacement 
 * character (\ufffd), and the stream is reset. If further errors occur, we go to
 * state TSM_UTF8_START to avoid printing multiple replacement characters for a
 * single misinterpreted UTF8 sequence. However, under some circumstances it may
 * happen that we stay in TSM_UTF8_REJECT and a next replacement character is
 * returned.
 * It is difficult to decide how to interpret wrong input but this machine seems
 * to be quite good at deciding what to do. Generally, we prefer discarding or
 * replacing input instead of trying to decipher ASCII values from the invalid
 * data. This guarantees that we do not send wrong values to the terminal
 * emulator. Some might argue that an ASCII fallback would be better. However,
 * this means that we might send very weird escape-sequences to the VTE layer.
 * Especially with C1 codes applications can really break many terminal features
 * so we avoid any non-ASCII+non-UTF8 input to prevent this.
 */
class Utf8To32Converter {
public:
  // Push a utf-8 char. If the char completes a Unicode code point, then the
  // code point is copied to code_point, and the method returns true. If the
  // char makes an invalid UTF-8 stream, then the replacement character (\ufffd)
  // is written to code_point, the method returns true, and the state machine
  // is reset. If the char starts or continues a valid UTF-8 sequence, but does
  // not complete it, then nothing is written to code_point, and false is
  // returned.
  bool put(char c, char32_t &code_point);
  
  // Reset the state of this converter
  void reset();

private:
  utf8_parse_state _state = UTF8_START;
  char32_t _code_point = 0;

  bool reject(char32_t &code_point);
};

} // end namespace vtutils
} // end namespace unicode

#endif /* VTUTILS_UNICODE_H_ */

