#include "winapi_printer.hpp"
#include "registry_printer.hpp"
#include "menu.hpp"

#include <clocale>
#include <iostream>
#include <memory>
#include <string>

int wmain() {
    using namespace WinApi;

    // Set local to german
    std::setlocale(LC_ALL, "de_DE.UTF-8");

#if 0
    auto& printers = Printer::load_printers();

    for (auto& printer : printers) {
        std::wcout << *printer;
    }
#endif

    Registry::Printer::show_lm_printers(std::wcout);
    std::wcin.get();

    // Helper::Menu::main_menu();
}