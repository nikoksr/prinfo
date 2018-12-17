#include "menu.h"
#include "console.h"
#include "format.h"
#include "information.h"

#include <conio.h>
#include <cctype>
#include <fstream>
#include <iostream>

// Hauptmenu
void main_menu() {
  // main-loop
  wchar_t user_input;

  do {
    // Titel
    clear_console_screen();

    // User input
    std::wcout << get_program_head() << L"\n\n"
               << get_separator_thick() << L"\n\n"
               << L" [1] Ausgabe aller gefundenen Informationen\n"
               << L" [2] Funktionen\n"
               << L" [3] Hilfe\n\n";
    user_input = submenu_quit();

    // Console leeren und Titel neu ausgeben
    clear_console_screen();
    std::wcout << get_program_head() << L"\n\n";

    // Auswertung und Ausführung von user-input
    switch (user_input) {
      // Standard Funktionen ausführen
      case L'1':
        // run_default_functions();
        user_input = submenu_save_back_quit();
        // save_request(&run_default_functions, user_input);
        break;
      // Funktionsmenü
      case L'2':
        user_input = functions_menu();
        break;
      // Hilfe
      case L'3':
        // run_help();
        user_input = submenu_back_quit();
        break;
      // Ungültige Eingabe
      default:
        break;
    }
  } while (user_input != L'b');
}

// Functions menu
wchar_t functions_menu() {
  wchar_t user_input;

  do {
    // Titel
    clear_console_screen();

    std::wcout << get_program_head() << L"\n\n"
               << get_separator_thick() << L"\n\n"
               << L" [1] Win-API Drucker\n"
               << L" [2] Registry Drucker\n"
               << L" [3] Systeminfo\n"
               << L" [4] Speichern aller gefundenen Infos\n"
               << L" [5] Löschen aller Druckaufträge (Admin-Rechte)\n\n";
    user_input = submenu_back_quit();

    clear_console_screen();
    std::wcout << get_program_head() << L"\n\n";

    switch (user_input) {
      // Win-Api
      case L'1':
        // run_winapi_printer();
        user_input = submenu_save_back_quit();
        // save_request(&run_winapi_printer, user_input);
        undo_back_command(&user_input);
        break;
      // Registry
      case L'2':
        // run_registry_printer();
        user_input = submenu_save_back_quit();
        // save_request(&run_registry_printer, user_input);
        undo_back_command(&user_input);
        break;
      // Systeminfo
      case L'3':
        // run_system_info();
        user_input = submenu_save_back_quit();
        // save_request(&run_system_info, user_input);
        undo_back_command(&user_input);
        break;
      // Speichern aller Infos
      case L'4':
        // save_request(&run_default_functions, L's');
        user_input = submenu_back_quit();
        undo_back_command(&user_input);
        break;
      // Löschen aller Druckaufträge
      case L'5':
        // delete_print_jobs();
        user_input = submenu_back_quit();
        undo_back_command(&user_input);
        break;
      // Ungültige Eingabe
      default:
        break;
    }
  } while (user_input != L'z' && user_input != L'b');

  return user_input;
}

wchar_t submenu_quit() {
  std::wcout << get_separator_thick() << L"\n [B]eenden";

  wchar_t temp_input;
  temp_input = _getwch();
  temp_input = tolower(temp_input);

  return temp_input;
}

wchar_t submenu_back_quit() {
  std::wcout << get_separator_thick() << L"\n [Z]urück  [B]eenden";

  wchar_t temp_input;
  temp_input = _getwch();
  temp_input = tolower(temp_input);

  return temp_input;
}

wchar_t submenu_save_back_quit() {
  std::wcout << get_separator_thick() << L"\n [S]peichern  [Z]urück  [B]eenden";

  wchar_t temp_input;
  temp_input = _getwch();
  temp_input = tolower(temp_input);

  return temp_input;
}

std::wstring get_separator_thick() {
  return L"===================================================================="
         L"=================================================";
}

std::wstring get_separator_thin() {
  return L"____________________________________________________________________"
         L"_________________________________________________";
}

void save_request(void (*function_to_save)(std::wostream &stream),
                  const wchar_t user_input) {
  if (user_input != L's') {
    return;
  }

  // Titel
  clear_console_screen();

  // User input
  std::wcout << get_program_head() << L"\n\n"
             << get_separator_thick() << L"\n\n Speichern\n"
             << get_separator_thin() << L"\n\n";

  // Dateiname
  std::wstring file_name;
  std::wcout << " Dateiname: ";
  std::getline(std::wcin, file_name);

  if (file_name.empty()) {
    file_name = L"prinfo.txt";
  }

  // Kommentar
  std::wstring comment;
  std::wcout << " Kommentar: ";
  std::getline(std::wcin, comment);
  std::wcout << L"\n";

  if (comment.empty()) {
    comment = L"-";
  }

  // Stream erstellen
  std::wofstream save_file(file_name, std::ios::out);

  if (!save_file.is_open()) {
    std::wcout << L" Fehler: " << general_error_message(GetLastError())
               << L"\n\n";
  }

  // Header
  save_file << get_program_head() << L"\n\n";

  // Kommentar in Datei schreiben
  save_file << get_separator_thick() << L"\n\n"
            << format_name_and_value(L"Kommentar", comment) << L"\n\n";

  // Daten der Funktion in Datei schreiben
  function_to_save(save_file);

  // Stream schließen
  if (save_file.is_open()) {
    save_file.close();
  }
}

void undo_back_command(wchar_t *user_input) {
  if (*user_input == L'z') {
    *user_input = L' ';
  }
}