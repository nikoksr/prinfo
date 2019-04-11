// Set minimum Windows version(Windows Header Files)
// https://docs.microsoft.com/en-us/windows/desktop/api/winreg/nf-winreg-reggetvaluew
// https://docs.microsoft.com/de-de/windows/desktop/WinProg/using-the-windows-headers
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 
#endif

#include "winapi_printer.hpp"
#include "registry_printer.hpp"
#include "menu.hpp"
#include "snippets.hpp"

#include <clocale>
#include <iostream>
#include <sstream>

int wmain() {
    // Set local to german
    std::setlocale(LC_ALL, "de_DE.UTF-8");
    // std::ios_base::sync_with_stdio(false); can increase cout performance

    auto& printers = WinApi::Printer::load_printers();
    std::wostringstream oss;

    for (auto& printer : printers) {
        oss << *printer;
    }

    std::wcout << oss.str();
    std::wcin.get();

    // Helper::Menu::main_menu();
}