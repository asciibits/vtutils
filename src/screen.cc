#include "screen.h"

namespace vtutils {
namespace screen {
  
std::ostream& operator<<(std::ostream &out, const Color &color) {
  if (color.color_code != COLOR_RGB) {
    out << "code:" << int(color.color_code);
  } else {
    out << "rgb[r:" << int(color.r)
        << ",g:" << int(color.g)
        << ",b:" << int(color.b)
        << ']';
  }
  return out;
}
std::ostream& operator<<(std::ostream &out, const Attr &attr) {
  out << "Attr[fg[" << attr.fg << "],bg[" << attr.bg << "]";
  if (attr.blink) {
    out << ",blink";
  }
  if (attr.bold) {
    out << ",bold";
  }
  if (attr.inverse) {
    out << ",inverse";
  }
  if (attr.protect) {
    out << ",protect";
  }
  if (attr.underline) {
    out << ",underline";
  }
  out << ']';
  return out;
}

} // namespace screen
} // namespace vtutils
