# vtutils - Virtual Terminal Utilities

VT Utils is a suite of C++ APIs and tools to assist with working with
terminals. It uses a virtual-terminal parser copied from the libtsm
project (ported from C to C++), and rather than drawing to a hard-coded
screen buffer, it sends all drawing primitives to a Screen interface. This
allows any use of Screen to be abstracted away from the terminal escape
parsing.

Specific screen impleentations have been created that are easily composable.
They include:

* box
  creates a virtal terminal of any size hosted within a parent terminal.
  If the sub-terminal does not fit, scrolling is made available via terminal
  escape sequences.

* scroll
  Creates a virtual terminal that tracks the scrollbuffer of an existing
  terminal, and provides scrollback and search.

