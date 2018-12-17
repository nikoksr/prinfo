#include "winapi_printer.h"

#include <clocale>
#include <iostream>

int wmain() {
  // Set local to german
  std::setlocale(LC_ALL, "de_DE.UTF-8");  

  auto printer_list = WinApi::Printer::get_printer_list();

  for (auto printer : printer_list) {
    std::wcout << L"Name: " << printer->get_name() << L"\n";
    std::wcout << L"Driver: " << printer->get_driver() << L"\n\n";
  }

  _getwch();
}