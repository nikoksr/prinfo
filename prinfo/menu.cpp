#include "menu.h"
#include "console.h"
#include "format.h"
#include "information.h"

#include <conio.h>
#include <cctype>
#include <fstream>
#include <iostream>

namespace Helper {
void Menu::main_menu() {
  wchar_t user_input;

  /**
    Main loop.
  */
  do {
    Console::clear_screen();
    std::wcout << Snippets::program_head() << L"\n\n"
               << get_separator_thick() << L"\n\n"
               << L" [1] Ausgabe aller gefundenen Informationen\n"
               << L" [2] Funktionen\n"
               << L" [3] Hilfe\n\n";
    user_input = submenu_quit();
    Console::clear_screen();
    std::wcout << Snippets::program_head() << L"\n\n";

    /**
      Interpret user input.
        * 1: Default functions
        * 2: Functions menu
        * 3: Help
    */
    switch (user_input) {
      case L'1':
        // run_default_functions();
        user_input = submenu_save_back_quit();
        // save_request(&run_default_functions, user_input);
        break;
      case L'2':
        user_input = functions_menu();
        break;
      case L'3':
        // run_help();
        user_input = submenu_back_quit();
        break;
      default:
        break;
    }
  } while (user_input != L'b');
}

wchar_t Menu::functions_menu() {
  wchar_t user_input;

  /**
    Second main loop.
  */
  do {
    Console::clear_screen();
    std::wcout << Snippets::program_head() << L"\n\n"
               << get_separator_thick() << L"\n\n"
               << L" [1] Win-API Drucker\n"
               << L" [2] Registry Drucker\n"
               << L" [3] Systeminfo\n"
               << L" [4] Speichern aller gefundenen Infos\n"
               << L" [5] Löschen aller Druckaufträge (Admin-Rechte)\n\n";
    user_input = submenu_back_quit();
    Console::clear_screen();
    std::wcout << Snippets::program_head() << L"\n\n";

    /**
      Interpret user input.
        * 1: WinApi data
        * 2: Registry data
        * 3: System data
        * 4: Save all data
        * 5: Delete all printjobs
    */
    switch (user_input) {
      case L'1':
        // run_winapi_printer();
        user_input = submenu_save_back_quit();
        // save_request(&run_winapi_printer, user_input);
        undo_back_command(&user_input);
        break;
      case L'2':
        // run_registry_printer();
        user_input = submenu_save_back_quit();
        // save_request(&run_registry_printer, user_input);
        undo_back_command(&user_input);
        break;
      case L'3':
        // run_system_info();
        user_input = submenu_save_back_quit();
        // save_request(&run_system_info, user_input);
        undo_back_command(&user_input);
        break;
      case L'4':
        // save_request(&run_default_functions, L's');
        user_input = submenu_back_quit();
        undo_back_command(&user_input);
        break;
      case L'5':
        // delete_print_jobs();
        user_input = submenu_back_quit();
        undo_back_command(&user_input);
        break;
      default:
        break;
    }
  } while (user_input != L'z' && user_input != L'b');

  return user_input;
}

wchar_t Menu::submenu_quit() {
  std::wcout << get_separator_thick() << L"\n [B]eenden";

  wchar_t temp_input;
  temp_input = _getwch();
  temp_input = tolower(temp_input);

  return temp_input;
}

wchar_t Menu::submenu_back_quit() {
  std::wcout << get_separator_thick() << L"\n [Z]urück  [B]eenden";

  wchar_t temp_input;
  temp_input = _getwch();
  temp_input = tolower(temp_input);

  return temp_input;
}

wchar_t Menu::submenu_save_back_quit() {
  std::wcout << get_separator_thick() << L"\n [S]peichern  [Z]urück  [B]eenden";

  wchar_t temp_input;
  temp_input = _getwch();
  temp_input = tolower(temp_input);

  return temp_input;
}

std::wstring Menu::get_separator_thick() {
  return L"===================================================================="
         L"=================================================";
}

std::wstring Menu::get_separator_thin() {
  return L"____________________________________________________________________"
         L"_________________________________________________";
}

void Menu::save_request(void (*function_to_save)(std::wostream &stream),
                        const wchar_t user_input) {
  if (user_input != L's') {
    return;
  }

  Console::clear_screen();
  std::wcout << Snippets::program_head() << L"\n\n"
             << get_separator_thick() << L"\n\n Speichern\n"
             << get_separator_thin() << L"\n\n";

  /**
    Ask for filename and comment.
  */
  std::wstring file_name;
  std::wcout << " Dateiname: ";
  std::getline(std::wcin, file_name);

  if (file_name.empty()) {
    file_name = L"prinfo.txt";
  }

  std::wstring comment;
  std::wcout << " Kommentar: ";
  std::getline(std::wcin, comment);
  std::wcout << L"\n";

  if (comment.empty()) {
    comment = L"-";
  }

  /**
    Stream data to file.
  */
  std::wofstream save_file(file_name, std::ios::out);

  if (!save_file.is_open()) {
    std::wcout << L" Fehler: " << Format::error_message(GetLastError())
               << L"\n\n";
  }

  save_file << Snippets::program_head() << L"\n\n";
  save_file << get_separator_thick() << L"\n\n"
            << Format::name_and_value(L"Kommentar", comment) << L"\n\n";
  function_to_save(save_file);

  if (save_file.is_open()) {
    save_file.close();
  }
}

void Menu::undo_back_command(wchar_t *user_input) {
  if (*user_input == L'z') {
    *user_input = L' ';
  }
}
}  // namespace Helper