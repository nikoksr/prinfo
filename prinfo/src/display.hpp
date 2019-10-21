#pragma once

#include "service.hpp"

#include <sstream>
#include <memory>
#include <string>

namespace data {
    class Display {
    public:
        static void Overview(std::wostream& wos);
        static void WinApiPrinters(std::wostream& wos);
        static void RegistryPrinters(std::wostream& wos);
        static void System(std::wostream& wos);
        static void Analyze(std::wostream& wos);
        static std::unique_ptr<winapi::Service> SetupSvc(std::wostream& wos, const std::wstring& service_name);
        static bool StartSvc(std::wostream& wos, const std::wstring& service_name);
        static bool StopSvc(std::wostream& wos, const std::wstring& service_name);
        static bool RestartSvc(std::wostream& wos, const std::wstring& service_name);
        static void PurgeFolder(std::wostream& wos, const std::wstring& folder);
        static void Help(std::wostream& wos);
        static void Warning(std::wostream& wos, const std::wstring& warning);
    };
}