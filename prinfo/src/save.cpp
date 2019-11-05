#include "save.hpp"
#include "console.hpp"
#include "snippets.hpp"
#include "format.hpp"

#include <fstream>
#include <iostream>

namespace data {

    using namespace console;

    void Save::ToFile(displayFn df) {
        Console::Clear();
        std::wcout << snippets::k_program_head << L"\n\n"
            << snippets::k_separator_thick << L"\n\n Speichern als Text\n"
            << snippets::k_separator_thin << L"\n\n";

        // Ask for filename and comment.        
        std::wstring file_name;
        std::wcout << " Dateiname: ";
        std::wcin.clear();
        std::getline(std::wcin, file_name);

        if (file_name.empty()) {
            file_name = L"prinfo.txt";
        }

        if (!Format::EndsWith(file_name, L".txt")) {
            file_name += L".txt";
        }

        std::wstring comment;
        std::wcout << " Kommentar: ";
        std::wcin.clear();
        std::getline(std::wcin, comment);
        std::wcout << L"\n";

        if (comment.empty()) {
            comment = L"-";
        }

        // Stream data to file.       
        std::wofstream save_file(file_name, std::ios::out);

        if (!save_file.is_open()) {
            std::wcout << L" Fehler: " << Format::ErrorMessage(GetLastError())
                << L"\n\n";
            return;
        }

        save_file << snippets::k_program_head << L"\n\n"
            << snippets::k_separator_thick << L"\n\n"
            << Format::NameValuePair(L"Kommentar", comment) << L"\n\n";
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
        std::wstring file_name;
        std::wcout << " Dateiname: ";
        std::wcin.clear();
        std::getline(std::wcin, file_name);

        if (file_name.empty()) {
            file_name = L"prinfo.csv";
        }

        if (!Format::EndsWith(file_name, L".csv")) {
            file_name += L".csv";
        }

        // Open files
        std::wofstream save_file(file_name, std::ios::out);

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