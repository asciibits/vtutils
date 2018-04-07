#include "debug_screen.h"

#include <iostream>

namespace vtutils {
namespace screen {

void DebugScreen::reset() {
  _out << "DebugScreen#reset: " << std::endl;
}
void DebugScreen::hard_reset() {
  _out << "DebugScreen#hard_reset: " << std::endl;
}

void DebugScreen::set_flags(unsigned int flags) {
  _out << "DebugScreen#set_flags: " << flags << std::endl;
}
void DebugScreen::reset_flags(unsigned int flags) {
  _out << "DebugScreen#reset_flags: " << flags << std::endl;
}

void DebugScreen::print(char32_t sym, Attr *attr) {
  _out << "DebugScreen#print: " << sym << ", " << *attr << std::endl;
}
void DebugScreen::newline() {
  _out << "DebugScreen#newline: " << std::endl;
}
void DebugScreen::insert_lines(unsigned int num) {
  _out << "DebugScreen#insert_lines: " << num << std::endl;
}
void DebugScreen::delete_lines(unsigned int num) {
  _out << "DebugScreen#delete_lines: " << num << std::endl;
}
void DebugScreen::insert_chars(unsigned int num) {
  _out << "DebugScreen#insert_chars: " << num << std::endl;
}
void DebugScreen::delete_chars(unsigned int num) {
  _out << "DebugScreen#delete_chars: " << num << std::endl;
}
// system bell
void DebugScreen::alert() {
  _out << "DebugScreen#alert: " << std::endl;
}

Attr DebugScreen::default_attr() {
  _out << "DebugScreen#default_attr: " << std::endl;
  return Attr{};
}

void DebugScreen::set_def_attr(Attr attr) {
  _out << "DebugScreen#set_def_attr: " << attr << std::endl;
}

void DebugScreen::move_left(unsigned int num) {
  _out << "DebugScreen#move_left: " << num << std::endl;
}
void DebugScreen::move_right(unsigned int num) {
  _out << "DebugScreen#move_right: " << num << std::endl;
}
void DebugScreen::move_to(unsigned int x, unsigned int y) {
  _out << "DebugScreen#move_to: " << x << ", " << y << std::endl;
}
void DebugScreen::move_up(unsigned int num, bool scroll) {
  _out << "DebugScreen#move_up: " << num << ", " << scroll << std::endl;
}
void DebugScreen::move_down(unsigned int num, bool scroll) {
  _out << "DebugScreen#move_down: " << num << ", " << scroll << std::endl;
}
void DebugScreen::move_line_home() {
  _out << "DebugScreen#move_line_home: " << std::endl;
}

void DebugScreen::scroll_up(unsigned int num) {
  _out << "DebugScreen#scroll_up: " << num << std::endl;
}
void DebugScreen::scroll_down(unsigned int num) {
  _out << "DebugScreen#scroll_down: " << num << std::endl;
}

void DebugScreen::set_tabstop() {
  _out << "DebugScreen#set_tabstop: " << std::endl;
}
void DebugScreen::reset_tabstop() {
  _out << "DebugScreen#reset_tabstop: " << std::endl;
}
void DebugScreen::reset_all_tabstops() {
  _out << "DebugScreen#reset_all_tabstops: " << std::endl;
}
void DebugScreen::tab_right(unsigned int num) {
  _out << "DebugScreen#tab_right: " << num << std::endl;
}
void DebugScreen::tab_left(unsigned int num) {
  _out << "DebugScreen#tab_left: " << num << std::endl;
}

unsigned int DebugScreen::get_cursor_x() {
  _out << "DebugScreen#get_cursor_x: " << std::endl;
  return 1;
}
unsigned int DebugScreen::get_cursor_y() {
  _out << "DebugScreen#get_cursor_y: " << std::endl;
  return 2;
}

void DebugScreen::erase_screen(bool protect) {
  _out << "DebugScreen#erase_screen: " << protect << std::endl;
}
void DebugScreen::erase_cursor_to_screen(bool protect) {
  _out << "DebugScreen#erase_cursor_to_screen: " << protect << std::endl;
}
void DebugScreen::erase_screen_to_cursor(bool protect) {
  _out << "DebugScreen#erase_screen_to_cursor: " << protect << std::endl;
}
void DebugScreen::erase_cursor_to_end(bool protect) {
  _out << "DebugScreen#erase_cursor_to_end: " << protect << std::endl;
}
void DebugScreen::erase_home_to_cursor(bool protect) {
  _out << "DebugScreen#erase_home_to_cursor: " << protect << std::endl;
}
void DebugScreen::erase_current_line(bool protect) {
  _out << "DebugScreen#erase_current_line: " << protect << std::endl;
}
void DebugScreen::erase_chars(unsigned int num) {
  _out << "DebugScreen#erase_chars: " << num << std::endl;
}

void DebugScreen::set_margins(unsigned int top, unsigned int bottom) {
  _out << "DebugScreen#set_margins: " << top << ", " << bottom << std::endl;
}
void DebugScreen::write(char c) {
  _out << "DebugScreen#write: " << c << std::endl;
}

} // namespace screen
} // namespace vtutils

