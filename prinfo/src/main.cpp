#include "winapi_printer.hpp"
#include "menu.hpp"

#include <clocale>
#include <iostream>

int wmain() {
    using namespace WinApi;

    // Set local to german
    std::setlocale(LC_ALL, "de_DE.UTF-8");

    std::vector<Printer*> printers = Printer::load_printers();

    for (Printer* printer : printers) {
        printer->display(std::wcout);
    }

    std::cin.get();


    // Helper::Menu::main_menu();
}