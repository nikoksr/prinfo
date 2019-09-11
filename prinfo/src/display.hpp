#pragma once

#include <sstream>

namespace data {
    class Display {
    public:
        static void All(std::wostream& wos);
        static void WinApiPrinters(std::wostream& wos);
        static void RegistryPrinters(std::wostream& wos);
        static void System(std::wostream& wos);
        static void Help(std::wostream& wos);
    };
}