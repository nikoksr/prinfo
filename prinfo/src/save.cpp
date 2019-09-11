#include "save.hpp"
#include "console.hpp"
#include "snippets.hpp"
#include "format.hpp"

#include <fstream>
#include <iostream>

namespace data {

    using namespace console;

    void Save::ToFile(displayFn df) {
        Console::clear();
        std::wcout << snippets::k_program_head << L"\n\n"
            << snippets::k_separator_thick << L"\n\n Speichern\n"
            << snippets::k_separator_thin << L"\n\n";

        /**
          Ask for filename and comment.
        */
        std::wstring file_name;
        std::wcout << " Dateiname: ";
        std::getline(std::wcin, file_name);

        if (file_name.empty()) {
            file_name = L"prinfo.txt";
        }

        if (!Format::EndsWith(file_name, L".txt")) {
            file_name += L".txt";
        }

        std::wstring comment;
        std::wcout << " Kommentar: ";
        std::getline(std::wcin, comment);
        std::wcout << L"\n";

        if (comment.empty()) {
            comment = L"-";
        }

        /**
          Stream data to file.
        */
        std::wofstream save_file(file_name, std::ios::out);

        if (!save_file.is_open()) {
            std::wcout << L" Fehler: " << Format::ErrorMessage(GetLastError())
                << L"\n\n";
        }

        save_file << snippets::k_program_head << L"\n\n"
            << snippets::k_separator_thick << L"\n\n"
            << Format::NameValuePair(L"Kommentar", comment) << L"\n\n";
        std::invoke(df, save_file);

        if (save_file.is_open()) {
            save_file.close();
        }
    }
}