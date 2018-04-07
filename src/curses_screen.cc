#include "curses_screen.h"

#include <iostream>

namespace vtutils {
namespace screen {

void CursesScreen::reset() {
  std::cout << "CursesScreen#reset: " << std::endl;
  _flags = 0;
  curs_set(1);
}
void CursesScreen::hard_reset() {
  std::cout << "CursesScreen#hard_reset: " << std::endl;
}

void CursesScreen::set_flags(unsigned int flags) {
  unsigned int old = _flags;
  _flags |= flags;
  if ((old & SCREEN_ALTERNATE) != (_flags & SCREEN_ALTERNATE)) {
    // TODO... how to handle alternate screen?
    // We can emulate it, or use the existing terminal's ability...
  }
  if ((old & SCREEN_HIDE_CURSOR) != (_flags & SCREEN_HIDE_CURSOR)) {
    // hide the cursor
    curs_set(0); // test changes
  }
  if ((old & SCREEN_INVERSE) != (_flags & SCREEN_INVERSE)) {
    // Invert the entire screen
    // TODO
  }
  
  std::cout << "CursesScreen#set_flags: " << flags << std::endl;
}
void CursesScreen::reset_flags(unsigned int flags) {
  unsigned int old = _flags;
  _flags &= ~flags;
  if ((old & SCREEN_ALTERNATE) != (_flags & SCREEN_ALTERNATE)) {
    def_shell_mode();
    // TODO
  }
  if ((old & SCREEN_HIDE_CURSOR) != (_flags & SCREEN_HIDE_CURSOR)) {
    // un-hide the cursor
    curs_set(1);
  }
  if ((old & SCREEN_INVERSE) != (_flags & SCREEN_INVERSE)) {
    // Un-Invert the entire screen
    // TODO
  }
  
  std::cout << "CursesScreen#set_flags: " << flags << std::endl;
}

void CursesScreen::print(char32_t sym, Attr *attr) {
  std::cout << "CursesScreen#print: " << sym << ", " << *attr << std::endl;
}
void CursesScreen::newline() {
  std::cout << "CursesScreen#newline: " << std::endl;
}
void CursesScreen::insert_lines(unsigned int num) {
  std::cout << "CursesScreen#insert_lines: " << num << std::endl;
}
void CursesScreen::delete_lines(unsigned int num) {
  std::cout << "CursesScreen#delete_lines: " << num << std::endl;
}
void CursesScreen::insert_chars(unsigned int num) {
  std::cout << "CursesScreen#insert_chars: " << num << std::endl;
}
void CursesScreen::delete_chars(unsigned int num) {
  std::cout << "CursesScreen#delete_chars: " << num << std::endl;
}
// system bell
void CursesScreen::alert() {
  std::cout << "CursesScreen#alert: " << std::endl;
}

Attr CursesScreen::default_attr() {
  std::cout << "CursesScreen#default_attr: " << std::endl;
  return Attr{};
}

void CursesScreen::set_def_attr(Attr attr) {
  std::cout << "CursesScreen#set_def_attr: " << attr << std::endl;
}

void CursesScreen::move_left(unsigned int num) {
  std::cout << "CursesScreen#move_left: " << num << std::endl;
}
void CursesScreen::move_right(unsigned int num) {
  std::cout << "CursesScreen#move_right: " << num << std::endl;
}
void CursesScreen::move_to(unsigned int x, unsigned int y) {
  std::cout << "CursesScreen#move_to: " << x << ", " << y << std::endl;
}
void CursesScreen::move_up(unsigned int num, bool scroll) {
  std::cout << "CursesScreen#move_up: " << num << ", " << scroll << std::endl;
}
void CursesScreen::move_down(unsigned int num, bool scroll) {
  std::cout << "CursesScreen#move_down: " << num << ", " << scroll << std::endl;
}
void CursesScreen::move_line_home() {
  std::cout << "CursesScreen#move_line_home: " << std::endl;
}

void CursesScreen::scroll_up(unsigned int num) {
  std::cout << "CursesScreen#scroll_up: " << num << std::endl;
}
void CursesScreen::scroll_down(unsigned int num) {
  std::cout << "CursesScreen#scroll_down: " << num << std::endl;
}

void CursesScreen::set_tabstop() {
  std::cout << "CursesScreen#set_tabstop: " << std::endl;
}
void CursesScreen::reset_tabstop() {
  std::cout << "CursesScreen#reset_tabstop: " << std::endl;
}
void CursesScreen::reset_all_tabstops() {
  std::cout << "CursesScreen#reset_all_tabstops: " << std::endl;
}
void CursesScreen::tab_right(unsigned int num) {
  std::cout << "CursesScreen#tab_right: " << num << std::endl;
}
void CursesScreen::tab_left(unsigned int num) {
  std::cout << "CursesScreen#tab_left: " << num << std::endl;
}

unsigned int CursesScreen::get_cursor_x() {
  std::cout << "CursesScreen#get_cursor_x: " << std::endl;
  
  return 1;
}
unsigned int CursesScreen::get_cursor_y() {
  std::cout << "CursesScreen#get_cursor_y: " << std::endl;
  return 2;
}

void CursesScreen::erase_screen(bool protect) {
  std::cout << "CursesScreen#erase_screen: " << protect << std::endl;
}
void CursesScreen::erase_cursor_to_screen(bool protect) {
  std::cout << "CursesScreen#erase_cursor_to_screen: " << protect << std::endl;
}
void CursesScreen::erase_screen_to_cursor(bool protect) {
  std::cout << "CursesScreen#erase_screen_to_cursor: " << protect << std::endl;
}
void CursesScreen::erase_cursor_to_end(bool protect) {
  std::cout << "CursesScreen#erase_cursor_to_end: " << protect << std::endl;
}
void CursesScreen::erase_home_to_cursor(bool protect) {
  std::cout << "CursesScreen#erase_home_to_cursor: " << protect << std::endl;
}
void CursesScreen::erase_current_line(bool protect) {
  std::cout << "CursesScreen#erase_current_line: " << protect << std::endl;
}
void CursesScreen::erase_chars(unsigned int num) {
  std::cout << "CursesScreen#erase_chars: " << num << std::endl;
}

void CursesScreen::set_margins(unsigned int top, unsigned int bottom) {
  std::cout << "CursesScreen#set_margins: " << top << ", " << bottom << std::endl;
}

void CursesScreen::write(char c) {
  std::cout << "CursesScreen#write: " << c << std::endl;
}

} // namespace screen
} // namespace vtutils
