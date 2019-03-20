#include "console.hpp"

#include <Windows.h>
#include <iostream>

#pragma comment(lib, "Kernel32.lib")

namespace Helper {
    void Console::clear_screen() {
        const HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

        if (console_handle == INVALID_HANDLE_VALUE) {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;

        if (!GetConsoleScreenBufferInfo(console_handle, &csbi)) {
            return;
        }

        /**
          Calculate console-screen size and fill it with blanks.
        */
        const DWORD screen_size = csbi.dwSize.X * csbi.dwSize.Y;
        constexpr COORD home_coords = { 0, 0 };
        DWORD num_chars;

        if (!FillConsoleOutputCharacter(console_handle, L' ', screen_size,
            home_coords, &num_chars)) {
            return;
        }

        /**
          Reset cursor position.
        */
        SetConsoleCursorPosition(console_handle, home_coords);
    }
}  // namespace Helper