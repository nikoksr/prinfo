#include "save.hpp"
#include "console.hpp"
#include "snippets.hpp"
#include "format.hpp"

#include <fstream>
#include <iostream>

namespace data {

    using namespace console;

    void Save::AskForFileInfo(File& file, const bool ask_for_comment) {
        // Ask for filename and comment.                
        std::wcout << " Dateiname: ";
        std::wcin.clear();
        std::getline(std::wcin, file.name);

        if (file.name.empty()) {
            file.name = L"prinfo" + file.type;
        }

        if (!Format::EndsWith(file.name, file.type)) {
            file.name += file.type;
        }

        if (ask_for_comment) {
            std::wcout << " Kommentar: ";
            std::wcin.clear();
            std::getline(std::wcin, file.comment);
            std::wcout << L"\n";

            if (file.comment.empty()) {
                file.comment = L"-";
            }
        }
    }

    void Save::ToFile(displayFn df) {
        Console::Clear();
        std::wcout << snippets::k_program_head << L"\n\n"
            << snippets::k_separator_thick << L"\n\n Speichern als Text\n"
            << snippets::k_separator_thin << L"\n\n";

        // Ask for filename and comment.        
        File f;
        f.type = L".txt";
        AskForFileInfo(f, true);

        // Stream data to file.       
        std::wofstream save_file(f.name, std::ios::out);

        if (!save_file.is_open()) {
            std::wcout << L" Fehler: " << Format::ErrorMessage(GetLastError())
                << L"\n\n";
            return;
        }

        save_file << snippets::k_program_head << L"\n\n"
            << snippets::k_separator_thick << L"\n\n"
            << Format::NameValuePair(L"Kommentar", f.comment) << L"\n\n";
        std::invoke(df, save_file);

        if (save_file.is_open()) {
            save_file.close();
        }
    }

    void Save::PrintersToCSV() {
        // Read optional file name
        Console::Clear();
        std::wcout << snippets::k_program_head << L"\n\n"
            << snippets::k_separator_thick << L"\n\n Exportieren als CSV\n"
            << snippets::k_separator_thin << L"\n\n";

        // Ask for filename
        File f;
        f.type = L".csv";
        AskForFileInfo(f, false);

        // Open files
        std::wofstream save_file(f.name, std::ios::out);

        if (!save_file.is_open()) {
            std::wcout << L" Fehler: " << Format::ErrorMessage(GetLastError()) << L"\n\n";
            return;
        }

        // Insert CSV header
        save_file << L"Name;Typ;Port;Freigabe;Sharename;Servername;Über Terminalserver verbunden;Treiber;Printprocessor;Datentyp;Duplex;Status;Anzahl Druckaufträge\n";

        // Load printers
        auto& printers = winapi::Printer::LoadPrinters();

        // Insert data
        for (auto& printer : printers) {
            save_file
                << printer->Name() << L";"
                << printer->Type() << L";"
                << printer->Port() << L";"
                << printer->IsShared() << L";"
                << printer->ShareName() << L";"
                << printer->ServerName() << L";"
                << printer->TerminalServer() << L";"
                << printer->Driver() << L";"
                << printer->PrintProcessor() << L";"
                << printer->DataType() << L";"
                << printer->Duplex() << L";"
                << printer->Status() << L";"
                << printer->NumOfPrintjobs() << L"\n";
        }

        if (save_file.is_open()) {
            save_file.close();
        }
    }
}