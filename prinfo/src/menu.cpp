#include "menu.hpp"
#include "console.hpp"
#include "format.hpp"
#include "snippets.hpp"
#include "display.hpp"
#include "save.hpp"

#include <conio.h>
#include <cctype>
#include <fstream>
#include <iostream>

using namespace console;
using namespace data;

namespace menu {
    wchar_t Main::Show() {
        if (!m_funcMenu) {
            m_funcMenu = std::make_unique<Functions>();
        }
        wchar_t user_input;

        do {
            // Read user choice
            Console::Clear();
            std::wcout << snippets::k_program_head << L"\n\n"
                << snippets::k_separator_thick << L"\n\n"
                << L" [1] Ausgabe aller gefundenen Informationen\n"
                << L" [2] Funktionen\n"
                << L" [3] Hilfe\n\n";
            user_input = Navigation::Quit();

            // Prepare output for chosen function
            Console::Clear();
            std::wcout << snippets::k_program_head << L"\n\n";

            /**
              Interpret user input.
                * 1: Default functions
                * 2: Functions menu
                * 3: Help
            */
            switch (user_input) {
            case L'1':
                Display::All(std::wcout);
                user_input = Navigation::SaveBackQuit();
                if (user_input == 's') { Save::ToFile(&Display::All); }
                break;
            case L'2':
                user_input = m_funcMenu->Show();
                break;
            case L'3':
                Display::Help(std::wcout);
                user_input = Navigation::BackQuit();
                break;
            default:
                break;
            }
        } while (user_input != L'b');
        return user_input;
    }

    wchar_t Functions::Show() {
        wchar_t user_input;
        bool quit_loop = false;

        do {
            // Read user choice
            Console::Clear();
            std::wcout << snippets::k_program_head << L"\n\n"
                << snippets::k_separator_thick << L"\n\n"
                << L" [1] Win-API Drucker\n"
                << L" [2] Registry Drucker\n"
                << L" [3] Systeminfo\n"
                << L" [4] Analyse\n"
                << L" [5] Speichern aller gefundenen Infos\n\n";
            user_input = Navigation::BackQuit();

            if (user_input == L'z' || user_input == L'b') {
                return user_input;
            }

            // Prepare output for chosen function
            Console::Clear();
            std::wcout << snippets::k_program_head << L"\n\n"
                << snippets::k_separator_thick << L"\n\n";

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
                Display::WinApiPrinters(std::wcout);
                user_input = Navigation::SaveBackQuit();
                if (user_input == 's') { Save::ToFile(&Display::WinApiPrinters); }
                break;
            case L'2':
                Display::RegistryPrinters(std::wcout);
                user_input = Navigation::SaveBackQuit();
                if (user_input == 's') { Save::ToFile(&Display::RegistryPrinters); }
                break;
            case L'3':
                Display::System(std::wcout);
                user_input = Navigation::SaveBackQuit();
                if (user_input == 's') { Save::ToFile(&Display::System); }
                break;
            case L'4':
                Display::Analyze(std::wcout);
                user_input = Navigation::SaveBackQuit();
                if (user_input == 's') { Save::ToFile(&Display::Analyze); }
                break;
            case L'5':
                Save::ToFile(&Display::All);
                break;
            case L'z':
                break;
            }
        } while (user_input != L'b');
        return user_input;
    }

    wchar_t Navigation::Quit() {
        std::wcout << snippets::k_separator_thick << L"\n [B]eenden";
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        return tolower(_getwch());
    }

    wchar_t Navigation::BackQuit() {
        std::wcout << snippets::k_separator_thick << L"\n [Z]urück  [B]eenden";
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        return tolower(_getwch());
    }

    wchar_t Navigation::SaveBackQuit() {
        std::wcout << snippets::k_separator_thick << L"\n [S]peichern  [Z]urück  [B]eenden";
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        return tolower(_getwch());
    }
}