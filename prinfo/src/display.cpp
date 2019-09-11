#include "display.hpp"
#include "winapi_printer.hpp"
#include "registry_printer.hpp"
#include "system.hpp"
#include "snippets.hpp"

namespace data {
    void Display::WinApiPrinters(std::wostream& wos) {
        auto& printers = winapi::Printer::LoadPrinters();

        for (auto& printer : printers) {
            wos << *printer;
        }
    }

    void Display::RegistryPrinters(std::wostream& wos) {
        registry::Printer::DisplayLMPrinters(wos);
        registry::Printer::DisplayCUPrinters(wos);
    }

    void Display::System(std::wostream& wos) {
        winapi::System system(L"local system");
        wos << system;
    }

    void Display::All(std::wostream& wos) {
        std::wostringstream woss;

        System(woss);
        WinApiPrinters(woss);
        RegistryPrinters(woss);

        wos << woss.str();
    }

    void Display::Help(std::wostream& wos) {
        wos << snippets::k_help;
    }
}