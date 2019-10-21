#include "display.hpp"
#include "winapi_printer.hpp"
#include "registry_printer.hpp"
#include "system.hpp"
#include "snippets.hpp"
#include "analyze.hpp"
#include "format.hpp"
#include <shlobj_core.h>
#include <filesystem>

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

    std::unique_ptr<winapi::Service> Display::SetupSvc(std::wostream& wos, const std::wstring& service_name) {
        auto service = std::make_unique<winapi::Service>(service_name);

        if (!service) {
            wos << L" Error: Konnte Service " << service_name << " nicht erzeugen.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return nullptr;
        }

        if (!service->connect()) {
            wos << L" Error: Konnte Service " << service_name << " nicht mit WinApi verbinden.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return nullptr;
        }

        return service;
    }

    bool Display::StartSvc(std::wostream& wos, const std::wstring& service_name) {
        auto service = Display::SetupSvc(wos, service_name);
        if (!service) {
            return false;
        }

        wos << L" " << service_name << " wird gestartet...";

        if (!service->start()) {
            wos << L"\n\n Error: Konnte Service " << service_name << " nicht starten.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return false;
        }

        wos << L"Erledigt.\n\n";
        return true;
    }

    bool Display::StopSvc(std::wostream& wos, const std::wstring& service_name) {
        auto service = Display::SetupSvc(wos, service_name);
        if (!service) {
            return false;
        }

        wos << L" " << service_name << " wird beendet...";

        if (!service->stop()) {
            wos << L"\n\n Error: Konnte Service " << service_name << " nicht beenden.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return false;
        }

        wos << L"Erledigt.\n\n";
        return true;
    }

    bool Display::RestartSvc(std::wostream& wos, const std::wstring& service_name) {
        auto service = Display::SetupSvc(wos, service_name);
        if (!service) {
            return false;
        }

        wos << L" " << service_name << " wird neugestartet...";

        if (!service->restart()) {
            wos << L"\n\n Error: Konnte Service " << service_name << " nicht neustarten.\n ";
            wos << Format::ErrorMessage(GetLastError());
            return false;
        }

        wos << L"Erledigt.\n\n";
        return true;
    }

    void Display::PurgeFolder(std::wostream& wos, const std::wstring& folder) {
        const std::wstring svc_name = L"Spooler";

        if (!StopSvc(wos, svc_name)) {
            return;
        }

        uintmax_t num_deleted = 0;

        for (auto& p : std::filesystem::directory_iterator(analyze::PrintersFolder::path)) {
            std::error_code err;
            num_deleted += std::filesystem::remove_all(p.path(), err);

            if (err) {
                wos << L" Fehler beim entfernen von " << p.path() << L". " << err.message().c_str() << L"\n";
            }
        }

        wos << L" " << num_deleted << " Dateien/Verzeichnisse wurden entfernt.\n\n";

        if (!StartSvc(wos, svc_name)) {
            return;
        }
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