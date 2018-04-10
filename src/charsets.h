#ifndef VTUTILS_CHARSETS_H_
#define VTUTILS_CHARSETS_H_

#include <uchar.h>
/*
 * VTE Character Sets
 * These are predefined charactersets that can be loaded into GL and GR. By
 * default we use unicode_lower and unicode_upper, that is, both sets have the
 * exact unicode mapping. unicode_lower is effectively ASCII and unicode_upper
 * as defined by the unicode standard.
 * Several other character sets are defined here. However, all of them are
 * limited to the 96 character space of GL or GR. Everything beyond GR (which
 * was not supported by the classic VTs by DEC but is available in VT emulators
 * that support unicode/UTF8) is always mapped to unicode and cannot be changed
 * by these character sets. Even mapping GL and GR is only available for
 * backwards compatibility as new applications can use the Unicode functionality
 * of the VTE.
 *
 * Moreover, mapping GR is almost unnecessary to support. In fact, Unicode UTF-8
 * support in VTE works by reading every incoming data as UTF-8 stream. This
 * maps GL/ASCII to ASCII, as UTF-8 is backwards compatible to ASCII, however,
 * everything that has the 8th bit set is a >=2-byte haracter in UTF-8. That is,
 * this is in no way backwards compatible to >=VT220 8bit support. Therefore, if
 * someone maps a character set into GR and wants to use them with this VTE,
 * then they must already send UTF-8 characters to use GR (all GR characters are
 * 8-bits). Hence, they can easily also send the correct UTF-8 character for the
 * unicode mapping.
 * The only advantage is that most characters in many sets are 3-byte UTF-8
 * characters and by mapping the set into GR/GL you can use 2 or 1 byte UTF-8
 * characters which saves bandwidth.
 * Another reason is, if you have older applications that use the VT220 8-bit
 * support and you put a ASCII/8bit-extension to UTF-8 converter in between, you
 * need these mappings to have the application behave correctly if it uses GL/GR
 * mappings extensively.
 *
 * Anyway, we support GL/GR mappings so here are the most commonly used maps as
 * defined by Unicode-standard, DEC-private maps and other famous charmaps.
 *
 * Characters 1-32 are always the control characters (part of CL) and cannot be
 * mapped. Characters 34-127 (94 characters) are part of GL and can be mapped.
 * Characters 33 and 128 are not part of GL and always mapped by VTE but are
 * included here in the maps for alignment reasons but always set to 0.
 */

namespace vtutils {
namespace charsets {

// Charsets are 96 printable characters. Terminals frequently switch between multiple
// charsets to handle various character graphics and other non-ascii characters.
typedef const char32_t charset[96];

/*
 * Lower Unicode character set. This maps the characters to the basic ASCII
 * characters 33-126. These are all graphics characters defined in ASCII. The
 * first an last entry are never used so we can safely set them to anything.
 */
charset unicode_lower = {
    0,  33,  34,  35,  36,  37,  38,  39,
   40,  41,  42,  43,  44,  45,  46,  47,
   48,  49,  50,  51,  52,  53,  54,  55,
   56,  57,  58,  59,  60,  61,  62,  63,
   64,  65,  66,  67,  68,  69,  70,  71,
   72,  73,  74,  75,  76,  77,  78,  79,
   80,  81,  82,  83,  84,  85,  86,  87,
   88,  89,  90,  91,  92,  93,  94,  95,
   96,  97,  98,  99, 100, 101, 102, 103,
  104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119,
  120, 121, 122, 123, 124, 125, 126,   0
};

/*
 * Upper Unicode Table
 * This maps all characters to the upper unicode characters 161-254. These are
 * not compatible to any older 8 bit character sets. See the Unicode standard
 * for the definitions of each symbol. Again, the first an last entry are never
 * used so set them to 0.
 */
charset unicode_upper = {
    0, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183,
  184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 194, 195, 196, 197, 198, 199,
  200, 201, 202, 203, 204, 205, 206, 207,
  208, 209, 210, 211, 212, 213, 214, 215,
  216, 217, 218, 219, 220, 221, 222, 223,
  224, 225, 226, 227, 228, 229, 230, 231,
  232, 233, 234, 235, 236, 237, 238, 239,
  240, 241, 242, 243, 244, 245, 246, 247,
  248, 249, 250, 251, 252, 253, 254,   0
};

/*
 * The DEC supplemental graphics set. For its definition see here:
 *  http://vt100.net/docs/vt220-rm/table2-3b.html
 * Its basically a mixture of common European symbols that are not part of
 * ASCII. Most often, this is mapped into GR to extend the basci ASCII part.
 *
 * This is very similar to unicode_upper, however, few symbols differ so do not
 * mix them up!
 */
charset dec_supplemental_graphics = {
    0, 161, 162, 163,   0, 165,   0, 167,
  164, 169, 170, 171,   0,   0,   0,   0,
  176, 177, 178, 179,   0, 181, 182, 183,
    0, 185, 186, 187, 188, 189,   0, 191,
  192, 193, 194, 195, 196, 197, 198, 199,
  200, 201, 202, 203, 204, 205, 206, 207,
    0, 209, 210, 211, 212, 213, 214, 338,
  216, 217, 218, 219, 220, 376,   0, 223,
  224, 225, 226, 227, 228, 229, 230, 231,
  232, 233, 234, 235, 236, 237, 238, 239,
    0, 241, 242, 243, 244, 245, 246, 339,
  248, 249, 250, 251, 252, 255,   0,   0
};

/*
 * DEC special graphics character set. See here for its definition:
 *  http://vt100.net/docs/vt220-rm/table2-4.html
 * This contains several characters to create ASCII drawings and similar. Its
 * commonly mapped into GR to extend the basic ASCII characters.
 *
 * Lower 62 characters map to ASCII 33-64, everything beyond is special and
 * commonly used for ASCII drawings. It depends on the Unicode Standard 3.2 for
 * the extended horizontal scan-line characters 3, 5, 7, and 9.
 */
charset dec_special_graphics = {
     0,   33,   34,   35,   36,   37,   38,   39,
    40,   41,   42,   43,   44,   45,   46,   47,
    48,   49,   50,   51,   52,   53,   54,   55,
    56,   57,   58,   59,   60,   61,   62,   63,
    64,   65,   66,   67,   68,   69,   70,   71,
    72,   73,   74,   75,   76,   77,   78,   79,
    80,   81,   82,   83,   84,   85,   86,   87,
    88,   89,   90,   91,   92,   93,   94,    0,
  9830, 9618, 9225, 9228, 9229, 9226,  176,  177,
  9252, 9227, 9496, 9488, 9484, 9492, 9532, 9146,
  9147, 9472, 9148, 9149, 9500, 9508, 9524, 9516,
  9474, 8804, 8805,  960, 8800,  163, 8901,    0
};

#endif /* VTUTILS_CHARSETS_H_ */

} // namespace charsets
} // namespace vtutils
