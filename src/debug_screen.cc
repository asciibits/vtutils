#include "debug_screen.h"

#include <iostream>

namespace vtutils {
namespace screen {

void DebugScreen::reset() {
  std::cout << "DebugScreen#reset: \n";
}
void DebugScreen::hard_reset() {
  std::cout << "DebugScreen#hard_reset: \n";
}

void DebugScreen::set_flags(unsigned int flags) {
  std::cout << "DebugScreen#set_flags: " << flags << '\n';
}
void DebugScreen::reset_flags(unsigned int flags) {
  std::cout << "DebugScreen#reset_flags: " << flags << '\n';
}

void DebugScreen::write(char32_t sym, Attr *attr) {
  std::cout << "DebugScreen#write: " << sym << ", " << *attr << '\n';
}
void DebugScreen::newline() {
  std::cout << "DebugScreen#newline: \n";
}
void DebugScreen::insert_lines(unsigned int num) {
  std::cout << "DebugScreen#insert_lines: " << num << '\n';
}
void DebugScreen::delete_lines(unsigned int num) {
  std::cout << "DebugScreen#delete_lines: " << num << '\n';
}
void DebugScreen::insert_chars(unsigned int num) {
  std::cout << "DebugScreen#insert_chars: " << num << '\n';
}
void DebugScreen::delete_chars(unsigned int num) {
  std::cout << "DebugScreen#delete_chars: " << num << '\n';
}
// system bell
void DebugScreen::alert() {
  std::cout << "DebugScreen#alert: \n";
}

Attr DebugScreen::default_attr() {
  std::cout << "DebugScreen#default_attr: \n";
  return Attr{};
}

void DebugScreen::set_def_attr(screen::Attr attr) {
  std::cout << "DebugScreen#set_def_attr: " << attr << '\n';
}

void DebugScreen::move_left(unsigned int num) {
  std::cout << "DebugScreen#move_left: " << num << '\n';
}
void DebugScreen::move_right(unsigned int num) {
  std::cout << "DebugScreen#move_right: " << num << '\n';
}
void DebugScreen::move_to(unsigned int x, unsigned int y) {
  std::cout << "DebugScreen#move_to: " << x << ", " << y << '\n';
}
void DebugScreen::move_up(unsigned int num, bool scroll) {
  std::cout << "DebugScreen#move_up: " << num << ", " << scroll << '\n';
}
void DebugScreen::move_down(unsigned int num, bool scroll) {
  std::cout << "DebugScreen#move_down: " << num << ", " << scroll << '\n';
}
void DebugScreen::move_line_home() {
  std::cout << "DebugScreen#move_line_home: \n";
}

void DebugScreen::scroll_up(unsigned int num) {
  std::cout << "DebugScreen#scroll_up: " << num << '\n';
}
void DebugScreen::scroll_down(unsigned int num) {
  std::cout << "DebugScreen#scroll_down: " << num << '\n';
}

void DebugScreen::set_tabstop() {
  std::cout << "DebugScreen#set_tabstop: \n";
}
void DebugScreen::reset_tabstop() {
  std::cout << "DebugScreen#reset_tabstop: \n";
}
void DebugScreen::reset_all_tabstops() {
  std::cout << "DebugScreen#reset_all_tabstops: \n";
}
void DebugScreen::tab_right(unsigned int num) {
  std::cout << "DebugScreen#tab_right: " << num << '\n';
}
void DebugScreen::tab_left(unsigned int num) {
  std::cout << "DebugScreen#tab_left: " << num << '\n';
}

unsigned int DebugScreen::get_cursor_x() {
  std::cout << "DebugScreen#get_cursor_x: \n";
  return 1;
}
unsigned int DebugScreen::get_cursor_y() {
  std::cout << "DebugScreen#get_cursor_y: \n";
  return 2;
}

void DebugScreen::erase_screen(bool protect) {
  std::cout << "DebugScreen#erase_screen: " << protect << '\n';
}
void DebugScreen::erase_cursor_to_screen(bool protect) {
  std::cout << "DebugScreen#erase_cursor_to_screen: " << protect << '\n';
}
void DebugScreen::erase_screen_to_cursor(bool protect) {
  std::cout << "DebugScreen#erase_screen_to_cursor: " << protect << '\n';
}
void DebugScreen::erase_cursor_to_end(bool protect) {
  std::cout << "DebugScreen#erase_cursor_to_end: " << protect << '\n';
}
void DebugScreen::erase_home_to_cursor(bool protect) {
  std::cout << "DebugScreen#erase_home_to_cursor: " << protect << '\n';
}
void DebugScreen::erase_current_line(bool protect) {
  std::cout << "DebugScreen#erase_current_line: " << protect << '\n';
}
void DebugScreen::erase_chars(unsigned int num) {
  std::cout << "DebugScreen#erase_chars: " << num << '\n';
}

void DebugScreen::set_margins(unsigned int top, unsigned int bottom) {
  std::cout << "DebugScreen#set_margins: " << top << ", " << bottom << '\n';
}

} // namespace screen
} // namespace vtutils

