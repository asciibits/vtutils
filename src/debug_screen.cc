#include "debug_screen.h"

#include <iostream>

namespace vtutils {
namespace screen {

void DebugScreen::reset() {
  std::cout << "DebugScreen#reset: " << std::endl;
}
void DebugScreen::hard_reset() {
  std::cout << "DebugScreen#hard_reset: " << std::endl;
}

void DebugScreen::set_flags(unsigned int flags) {
  std::cout << "DebugScreen#set_flags: " << flags << std::endl;
}
void DebugScreen::reset_flags(unsigned int flags) {
  std::cout << "DebugScreen#reset_flags: " << flags << std::endl;
}

void DebugScreen::write(char32_t sym, Attr *attr) {
  std::cout << "DebugScreen#write: " << sym << ", " << *attr << std::endl;
}
void DebugScreen::newline() {
  std::cout << "DebugScreen#newline: " << std::endl;
}
void DebugScreen::insert_lines(unsigned int num) {
  std::cout << "DebugScreen#insert_lines: " << num << std::endl;
}
void DebugScreen::delete_lines(unsigned int num) {
  std::cout << "DebugScreen#delete_lines: " << num << std::endl;
}
void DebugScreen::insert_chars(unsigned int num) {
  std::cout << "DebugScreen#insert_chars: " << num << std::endl;
}
void DebugScreen::delete_chars(unsigned int num) {
  std::cout << "DebugScreen#delete_chars: " << num << std::endl;
}
// system bell
void DebugScreen::alert() {
  std::cout << "DebugScreen#alert: " << std::endl;
}

Attr DebugScreen::default_attr() {
  std::cout << "DebugScreen#default_attr: " << std::endl;
  return Attr{};
}

void DebugScreen::set_def_attr(Attr attr) {
  std::cout << "DebugScreen#set_def_attr: " << attr << std::endl;
}

void DebugScreen::move_left(unsigned int num) {
  std::cout << "DebugScreen#move_left: " << num << std::endl;
}
void DebugScreen::move_right(unsigned int num) {
  std::cout << "DebugScreen#move_right: " << num << std::endl;
}
void DebugScreen::move_to(unsigned int x, unsigned int y) {
  std::cout << "DebugScreen#move_to: " << x << ", " << y << std::endl;
}
void DebugScreen::move_up(unsigned int num, bool scroll) {
  std::cout << "DebugScreen#move_up: " << num << ", " << scroll << std::endl;
}
void DebugScreen::move_down(unsigned int num, bool scroll) {
  std::cout << "DebugScreen#move_down: " << num << ", " << scroll << std::endl;
}
void DebugScreen::move_line_home() {
  std::cout << "DebugScreen#move_line_home: " << std::endl;
}

void DebugScreen::scroll_up(unsigned int num) {
  std::cout << "DebugScreen#scroll_up: " << num << std::endl;
}
void DebugScreen::scroll_down(unsigned int num) {
  std::cout << "DebugScreen#scroll_down: " << num << std::endl;
}

void DebugScreen::set_tabstop() {
  std::cout << "DebugScreen#set_tabstop: " << std::endl;
}
void DebugScreen::reset_tabstop() {
  std::cout << "DebugScreen#reset_tabstop: " << std::endl;
}
void DebugScreen::reset_all_tabstops() {
  std::cout << "DebugScreen#reset_all_tabstops: " << std::endl;
}
void DebugScreen::tab_right(unsigned int num) {
  std::cout << "DebugScreen#tab_right: " << num << std::endl;
}
void DebugScreen::tab_left(unsigned int num) {
  std::cout << "DebugScreen#tab_left: " << num << std::endl;
}

unsigned int DebugScreen::get_cursor_x() {
  std::cout << "DebugScreen#get_cursor_x: " << std::endl;
  return 1;
}
unsigned int DebugScreen::get_cursor_y() {
  std::cout << "DebugScreen#get_cursor_y: " << std::endl;
  return 2;
}

void DebugScreen::erase_screen(bool protect) {
  std::cout << "DebugScreen#erase_screen: " << protect << std::endl;
}
void DebugScreen::erase_cursor_to_screen(bool protect) {
  std::cout << "DebugScreen#erase_cursor_to_screen: " << protect << std::endl;
}
void DebugScreen::erase_screen_to_cursor(bool protect) {
  std::cout << "DebugScreen#erase_screen_to_cursor: " << protect << std::endl;
}
void DebugScreen::erase_cursor_to_end(bool protect) {
  std::cout << "DebugScreen#erase_cursor_to_end: " << protect << std::endl;
}
void DebugScreen::erase_home_to_cursor(bool protect) {
  std::cout << "DebugScreen#erase_home_to_cursor: " << protect << std::endl;
}
void DebugScreen::erase_current_line(bool protect) {
  std::cout << "DebugScreen#erase_current_line: " << protect << std::endl;
}
void DebugScreen::erase_chars(unsigned int num) {
  std::cout << "DebugScreen#erase_chars: " << num << std::endl;
}

void DebugScreen::set_margins(unsigned int top, unsigned int bottom) {
  std::cout << "DebugScreen#set_margins: " << top << ", " << bottom << std::endl;
}

} // namespace screen
} // namespace vtutils

