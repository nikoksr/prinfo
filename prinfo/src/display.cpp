#include "display.hpp"
#include "winapi_printer.hpp"
#include "registry_printer.hpp"
#include "system.hpp"
#include "snippets.hpp"
#include "analyze.hpp"

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

    void Display::Analyze(std::wostream& wos) {
        analyze::PrintersFolder p_folder;
        wos << p_folder;
    }

    void Display::Overview(std::wostream& wos) {
        std::wostringstream woss;
        System(woss);
        WinApiPrinters(woss);
        wos << woss.str();
    }

    void Display::Help(std::wostream& wos) {
        wos << snippets::k_help;
    }
}