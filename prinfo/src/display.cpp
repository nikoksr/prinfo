#include "display.hpp"
#include "winapi_printer.hpp"
#include "registry_printer.hpp"
#include "system.hpp"
#include "snippets.hpp"
#include "analyze.hpp"
#include "service.hpp"
#include "format.hpp"
#include <shlobj_core.h>

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

    void Display::RestartSpooler(std::wostream& wos) {
        auto service = std::make_unique<winapi::Service>(L"Spooler");

        if (!service) {
            wos << L" Error: Konnte Service nicht erzeugen.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return;
        }

        if (!service->connect()) {
            wos << L" Error: Konnte Service nicht mit WinApi verbinden.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return;
        }

        wos << L" Druckerwarteschlange wird neugestartet...";

        if (!service->restart()) {
            wos << L"\n\n Error: Konnte Service nicht neustarten.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return;
        }

        wos << L"Erledigt.\n\n";
    }

    void Display::Warning(std::wostream& wos, const std::wstring& warning) {
        wos << L" Warnung\n\n" << warning << L" Möchten Sie trotzdem fortfahren?\n\n";
    }

    void Display::Overview(std::wostream& wos) {
        std::wostringstream woss;
        System(woss);
        WinApiPrinters(woss);

        if (IsUserAnAdmin()) {
            analyze::PrintersFolder p_folder;
            woss << p_folder;
        }

        wos << woss.str();
    }

    void Display::Help(std::wostream& wos) {
        wos << snippets::k_help;
    }
}