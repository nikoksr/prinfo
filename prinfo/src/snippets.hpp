#pragma once

#include <string>

namespace Helper {
    namespace Snippets {

        /**
        * Default console screen header.
        * Includes Copyright, Edit date, Creator, Contact email.
        */
        constexpr wchar_t program_head[] =
            L"\n Prinfo v0.3        01.04.2019        http://192.168.10.250/admins/prinfo        koeserniko@gmail.com";

        /**
        * Help text.
        */
        constexpr wchar_t help[] =
            L" Prinfo\n\n"
            L"  Dieses Programm wird unter der GNU GPLv3 veröffentlicht.\n"
            L"  Ich entwickle dieses Projekt in meiner Freizeit und garantiere\n"
            L"  daher nicht für seine Fehlerlosigkeit und Stabilität, so wie es\n"
            L"  auch in der Lizens zu lesen ist.\n\n\n"
            L" GitHub\n\n"
            L"  http://192.168.10.250/admins/prinfo\n\n\n"
            L" Lizens\n\n"
            L"  GNU GPLv3\n\n\n"
            L" Kontakt\n\n"
            L"  koeserniko@gmail.com\n\n\n"
            L" Wichtige Hinweise\n\n"
            L"  - Gewisse Funktionen brauchen eventuell Admin-Rechte.\n\n"
            L"  - Bei Formatierungsproblemen der gespeicherten Daten in Notepad:\n"
            L"    Schriftart auf 'Consolas', Schriftschnitt auf 'Standard' und\n"
            L"    Schriftgröße auf '11' umstellen.\n\n";

        /**
        * A thick visual separator line.
        */
        constexpr wchar_t separator_thick[] =
            L"===================================================================="
            L"=================================================";

        /**
        * A thin visual separator line.
        */
        constexpr wchar_t separator_thin[] =
            L"____________________________________________________________________"
            L"_________________________________________________";

    }  // namespace Snippets
}  // namespace Helper