#pragma once

#include "display.hpp"
#include "winapi_printer.hpp"
#include "winapi_printjob.hpp"

#include <sstream>

namespace data {
    struct File {
        std::wstring type;
        std::wstring name;
        std::wstring comment;
    };

    class Save {
    private:
        typedef  void(*displayFn)(std::wostream& wos);
    public:
        static void AskForFileInfo(File& file, const bool ask_for_comment);
        static void ToFile(displayFn df);
        static void PrintersToCSV();
        static void PrintersToJSON();
    };
}