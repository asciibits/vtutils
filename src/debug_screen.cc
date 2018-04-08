#include "debug_screen.h"

#include <iostream>
#include <typeinfo>

namespace vtutils {
namespace screen {

void DebugScreen::reset() {
  _out << class_name() << "#reset: " << std::endl;
}
void DebugScreen::hard_reset() {
  _out << class_name() << "#hard_reset: " << std::endl;
}

void DebugScreen::set_flags(unsigned int flags) {
  _out << class_name() << "#set_flags: " << flags << std::endl;
}
void DebugScreen::reset_flags(unsigned int flags) {
  _out << class_name() << "#reset_flags: " << flags << std::endl;
}

void DebugScreen::print(char32_t sym, Attr *attr) {
  _out << class_name() << "#print: " << sym << ", " << *attr << std::endl;
}
void DebugScreen::newline() {
  _out << class_name() << "#newline: " << std::endl;
}
void DebugScreen::insert_lines(unsigned int num) {
  _out << class_name() << "#insert_lines: " << num << std::endl;
}
void DebugScreen::delete_lines(unsigned int num) {
  _out << class_name() << "#delete_lines: " << num << std::endl;
}
void DebugScreen::insert_chars(unsigned int num) {
  _out << class_name() << "#insert_chars: " << num << std::endl;
}
void DebugScreen::delete_chars(unsigned int num) {
  _out << class_name() << "#delete_chars: " << num << std::endl;
}
// system bell
void DebugScreen::alert() {
  _out << class_name() << "#alert: " << std::endl;
}

Attr DebugScreen::default_attr() {
  _out << class_name() << "#default_attr: " << std::endl;
  return Attr{};
}

void DebugScreen::set_def_attr(Attr attr) {
  _out << class_name() << "#set_def_attr: " << attr << std::endl;
}

void DebugScreen::move_left(unsigned int num) {
  _out << class_name() << "#move_left: " << num << std::endl;
}
void DebugScreen::move_right(unsigned int num) {
  _out << class_name() << "#move_right: " << num << std::endl;
}
void DebugScreen::move_to(unsigned int x, unsigned int y) {
  _out << class_name() << "#move_to: " << x << ", " << y << std::endl;
}
void DebugScreen::move_up(unsigned int num, bool scroll) {
  _out << class_name() << "#move_up: " << num << ", " << scroll << std::endl;
}
void DebugScreen::move_down(unsigned int num, bool scroll) {
  _out << class_name() << "#move_down: " << num << ", " << scroll << std::endl;
}
void DebugScreen::move_line_home() {
  _out << class_name() << "#move_line_home: " << std::endl;
}

void DebugScreen::scroll_up(unsigned int num) {
  _out << class_name() << "#scroll_up: " << num << std::endl;
}
void DebugScreen::scroll_down(unsigned int num) {
  _out << class_name() << "#scroll_down: " << num << std::endl;
}

void DebugScreen::set_tabstop() {
  _out << class_name() << "#set_tabstop: " << std::endl;
}
void DebugScreen::reset_tabstop() {
  _out << class_name() << "#reset_tabstop: " << std::endl;
}
void DebugScreen::reset_all_tabstops() {
  _out << class_name() << "#reset_all_tabstops: " << std::endl;
}
void DebugScreen::tab_right(unsigned int num) {
  _out << class_name() << "#tab_right: " << num << std::endl;
}
void DebugScreen::tab_left(unsigned int num) {
  _out << class_name() << "#tab_left: " << num << std::endl;
}

unsigned int DebugScreen::get_cursor_x() {
  _out << class_name() << "#get_cursor_x: " << std::endl;
  return 1;
}
unsigned int DebugScreen::get_cursor_y() {
  _out << class_name() << "#get_cursor_y: " << std::endl;
  return 2;
}

void DebugScreen::erase_screen(bool protect) {
  _out << class_name() << "#erase_screen: " << protect << std::endl;
}
void DebugScreen::erase_cursor_to_screen(bool protect) {
  _out << class_name() << "#erase_cursor_to_screen: " << protect << std::endl;
}
void DebugScreen::erase_screen_to_cursor(bool protect) {
  _out << class_name() << "#erase_screen_to_cursor: " << protect << std::endl;
}
void DebugScreen::erase_cursor_to_end(bool protect) {
  _out << class_name() << "#erase_cursor_to_end: " << protect << std::endl;
}
void DebugScreen::erase_home_to_cursor(bool protect) {
  _out << class_name() << "#erase_home_to_cursor: " << protect << std::endl;
}
void DebugScreen::erase_current_line(bool protect) {
  _out << class_name() << "#erase_current_line: " << protect << std::endl;
}
void DebugScreen::erase_chars(unsigned int num) {
  _out << class_name() << "#erase_chars: " << num << std::endl;
}

void DebugScreen::set_margins(unsigned int top, unsigned int bottom) {
  _out << class_name() << "#set_margins: " << top << ", " << bottom << std::endl;
}
void DebugScreen::write(char c) {
  _out << class_name() << "#write: " << c << std::endl;
}

} // namespace screen
} // namespace vtutils

