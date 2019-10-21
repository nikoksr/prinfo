#pragma once

#include <string>


namespace snippets {

    /**
    * Default console screen header.
    * Includes Copyright, Edit date, Creator, Contact email.
    */
    constexpr wchar_t k_program_head[] =
        L"\n Prinfo v0.6.1        https://github.com/nikoksr/prinfo        koeserniko@gmail.com";

    /**
    * Help text.
    */
    constexpr wchar_t k_help[] =
        L" Prinfo\n\n"
        L"  Dieses Programm wird unter der GNU GPLv3 veröffentlicht.\n"
        L"  Ich entwickle dieses Projekt in meiner Freizeit und garantiere\n"
        L"  daher nicht für seine Fehlerlosigkeit und Stabilität, so wie es\n"
        L"  auch in der Lizens zu lesen ist.\n\n\n"
        L" GitHub\n\n"
        L"  https://github.com/nikoksr/prinfo\n\n\n"
        L" Lizens\n\n"
        L"  MIT\n\n\n"
        L" Kontakt\n\n"
        L"  koeserniko@gmail.com\n\n\n"
        L" Wichtige Hinweise\n\n"
        L"  - Gewisse Funktionen brauchen eventuell Admin-Rechte.\n\n"
        L"  - Bei Darstellungsproblemen der gespeicherten Daten in Notepad:\n"
        L"    Schriftart auf 'Consolas', Schriftschnitt auf 'Standard' und\n"
        L"    Schriftgröße auf '11' umstellen.\n\n";

    /**
    * A thick visual separator line.
    */
    constexpr wchar_t k_separator_thick[] =
        L"===================================================================="
        L"=================================================";

    /**
    * A thin visual separator line.
    */
    constexpr wchar_t k_separator_thin[] =
        L"____________________________________________________________________"
        L"_________________________________________________";

    constexpr wchar_t k_warn_spooler[] =
        L" Sie sind im Begriff die Druckerwarteschlange neuzustarten.\n\n";

    constexpr wchar_t k_warn_clear_printers_folder[] =
        L" Sie sind im Begriff die Druckerwarteschlange neuzustarten und alle Dateien im PRINTERS Ordner zu löschen.\n\n";

}  // namespace Snippets