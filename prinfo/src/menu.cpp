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

#include <shlobj_core.h>

using namespace console;
using namespace data;

namespace menu {
    Main::Main() : m_is_admin(IsUserAnAdmin()) {}

    const bool Main::isAdmin() const {
        return m_is_admin;
    }

    wchar_t Main::Show() {
        if (!m_func_menu) {
            m_func_menu = std::make_unique<Functions>();
        }
        wchar_t user_input;

        do {
            // Read user choice
            Console::Clear();
            std::wcout << snippets::k_program_head << L"\n\n"
                << snippets::k_separator_thick << L"\n\n"
                << L" [1] Überblick\n"
                << L" [2] Weitere Funktionen\n"
                << L" [3] Hilfe\n\n";
            user_input = Navigation::Quit();

            // Prepare output for chosen function
            Console::Clear();
            std::wcout << snippets::k_program_head << L"\n\n";

            /**
              Interpret user input.
                * 1: Overview
                * 2: Functions menu
                * 3: Help
            */
            switch (user_input) {
            case L'1':
                Display::Overview(std::wcout);
                user_input = Navigation::SaveBackQuit();
                if (user_input == 's') { Save::ToFile(&Display::Overview); }
                break;
            case L'2':
                user_input = m_func_menu->Show();
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
            Console::Clear();

            // User functions
            std::wcout << snippets::k_program_head << L"\n\n"
                << snippets::k_separator_thick << L"\n\n"
                << L" [1] Win-API Drucker\n"
                << L" [2] Registry Drucker\n"
                << L" [3] Systeminfo\n"
                << L" [4] Überblick speichern\n";

            // Admin functions
            if (isAdmin()) {
                std::wcout << L" [5] Analyse\n" << L" [6] Druckerwarteschlange neustarten\n";
            }

            // Final linebreak            
            std::wcout << L"\n";
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
                * 4: Save overview
                ----
                * 5: Analyze
                * 6: Restart printerspooler
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
                Save::ToFile(&Display::Overview);
                break;
            case L'5':
                if (!isAdmin()) { break; }
                Display::Analyze(std::wcout);
                user_input = Navigation::SaveBackQuit();
                if (user_input == 's') { Save::ToFile(&Display::Analyze); }
                break;
            case L'6':
                if (!isAdmin()) { break; }
                Display::Warning(std::wcout, snippets::k_warn_spooler);
                user_input = Navigation::YesNo();
                if (user_input == 'j') {
                    Console::Clear();
                    std::wcout << snippets::k_program_head << L"\n\n"
                        << snippets::k_separator_thick << L"\n\n";
                    Display::RestartSpooler(std::wcout);
                    user_input = Navigation::BackQuit();
                }
                break;
            case L'z':
                break;
            }
        } while (user_input != L'b');
        return user_input;
    }

    wchar_t Navigation::Quit() {
        std::wcout << snippets::k_separator_thick << L"\n [B]eenden\n";
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        return tolower(_getwch());
    }

    wchar_t Navigation::BackQuit() {
        std::wcout << snippets::k_separator_thick << L"\n [Z]urück  [B]eenden\n";
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        return tolower(_getwch());
    }

    wchar_t Navigation::SaveBackQuit() {
        std::wcout << snippets::k_separator_thick << L"\n [S]peichern  [Z]urück  [B]eenden\n";
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        return tolower(_getwch());
    }

    wchar_t Navigation::YesNo() {
        std::wcout << snippets::k_separator_thick << L"\n [J]a  [N]ein\n";
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        return tolower(_getwch());
    }
}