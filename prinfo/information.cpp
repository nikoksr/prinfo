#include "information.h"
#include "menu.h"

#include <iostream>

// Programm Info/ Copyright ausgeben
std::wstring get_program_head() {
  return L"\n Prinfo\u00A9        20.11.2016        Niko Köser        "
         L"koeserniko@gmail.com";
}

// Hilfe anzeigen
std::wstring get_help() {
  return Helper::Menu::get_separator_thick() +
         L"\n\n"
         L" Wichtige Hinweise:\n\n"
         L"     * Bei fehlerhaften Programm-/ Funktionsabläufen sollte das "
         L"Programm\n"
         L"        bzw. die CMD-Console als Administrator ausgeführt "
         L"werden.\n\n"
         L"     * Bei Darstellungs-/Formatierungsproblemen der gespeicherten "
         L"Daten in Notepad:\n"
         L"        Schriftart auf 'Consolas', Schriftschnitt auf 'Standard' "
         L"und Schriftgröße auf '11' umstellen.\n\n"
         L"     * Getestete Systeme: Windows 10\n\n";
}