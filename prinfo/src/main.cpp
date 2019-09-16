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

#include <iostream>
#include <sstream>
#include <io.h>
#include <fcntl.h>

int wmain() {
    // Set translation mode
    _setmode(_fileno(stdout), 0x00020000);
    _setmode(_fileno(stdin), 0x00020000);

    // Stop io sync
    std::ios_base::sync_with_stdio(false);

    auto menu = std::make_unique<menu::Main>();
    menu->Show();
}