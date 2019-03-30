#include "winapi_printer.hpp"
#include "menu.hpp"

#include <clocale>
#include <iostream>
#include <memory>
#include <string>

int wmain() {
    using namespace WinApi;

    // Set local to german
    std::setlocale(LC_ALL, "de_DE.UTF-8");

    auto& printers = Printer::load_printers();

    for (auto& printer : printers) {
        std::wcout << *printer;
    }

    std::cin.get();

    // Helper::Menu::main_menu();
}