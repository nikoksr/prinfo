#pragma once

#include <string>

namespace Helper {
    class Snippets {
    public:
        /**
        * Default console screen header.
        * Includes Copyright, Edit date, Creator, Contact email.
        */
        static constexpr wchar_t program_head[] =
            L"\n Prinfo\u00A9        20.11.2016        Niko Köser        koeserniko@gmail.com";

        /**
        * Help text.
        */
        static constexpr wchar_t help[] =
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

        /**
        * A thick visual separator line.
        */
        static constexpr wchar_t separator_thick[] =
            L"===================================================================="
            L"=================================================";

        /**
        * A thin visual separator line.
        */
        static constexpr wchar_t separator_thin[] =
            L"____________________________________________________________________"
            L"_________________________________________________";

    };  // class Snippets
}  // namespace Helper