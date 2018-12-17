#ifndef MENU_H
#define MENU_H

#include <string>

// Menus
void main_menu();
wchar_t functions_menu();
wchar_t submenu_quit();
wchar_t submenu_back_quit();
wchar_t submenu_save_back_quit();

// Separators
std::wstring get_separator_thick();
std::wstring get_separator_thin();

// Help funcs
void save_request(void (*function_to_save)(std::wostream &stream),
                  const wchar_t user_input);
void undo_back_command(wchar_t *user_input);

#endif  // MENU_H