#pragma once

#include <string>

namespace Helper {
    class Menu {
    public:
        /**
          Main menu.
        */
        static void main_menu();

        /**
          Menu for listing all available functions.
        */
        static wchar_t functions_menu();

        /**
          Submenus for navigating back, quitting and saving data.
        */
        static wchar_t submenu_quit();
        static wchar_t submenu_back_quit();
        static wchar_t submenu_save_back_quit();

        /**
          Visual separator lines.
          Draws different lines in console to separate content.
        */
        static std::wstring get_separator_thick();
        static std::wstring get_separator_thin();

        /**
          Save functionality.

          @param function_to_save Pointer to function which data is wanted to be
            saved.
          @param user_input Char that user put in console to execute function. Used
            for stability. Check if input was actually a call to save data.
        */
        static void save_request(void(*function_to_save)(std::wostream &stream),
            const wchar_t user_input);

        /**
          TODO: Find out what function does.
        */
        static void undo_back_command(wchar_t *user_input);
    };  // class Menu
}  // namespace Helper
