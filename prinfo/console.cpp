#include "console.h"

#include <Windows.h>
#include <iostream>

#pragma comment(lib, "Kernel32.lib")

// Konsolen Screen löschen
void clear_console_screen() {
  const auto console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

  if (console_handle == INVALID_HANDLE_VALUE) {
    return;
  }

  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if (!GetConsoleScreenBufferInfo(console_handle, &csbi)) {
    return;
  }

  // Screengröße berechnen
  const DWORD screen_size = csbi.dwSize.X * csbi.dwSize.Y;

  // Screen mit Leerzeichen f�llen
  constexpr COORD home_coords = {0, 0};
  DWORD num_chars;

  if (!FillConsoleOutputCharacter(console_handle, L' ', screen_size,
                                  home_coords, &num_chars))
    return;

  SetConsoleCursorPosition(console_handle, home_coords);
}