#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

namespace winapi {
    class System {
    public:
        System();
        System(const std::wstring& system_name);

        void Refresh();

        const std::wstring& SystemName() const;
        const std::wstring& UserName() const;
        const std::wstring& WorkstationName() const;
        const std::wstring& OfflineFiles() const;
        const std::wstring& DefaultPrinter() const;
        const std::wstring& OperatingSystem() const;
        const std::wstring& Processor() const;
        const std::wstring& Memory() const;

        friend std::wostream& operator<<(std::wostream& stream, const System& system);

    private:

        static HRESULT prepareWMI();
        static bool isWMIPrepared;

        std::wstring m_system_name;
        std::wstring m_user_name;
        std::wstring m_machine_name;
        std::wstring m_domain;
        std::wstring m_offline_files;
        std::wstring m_default_printer;
        std::wstring m_os_name;
        std::wstring m_os_version;
        std::wstring m_os_architecture;
        std::wstring m_processor;
        std::wstring m_memory_in_use;
        std::wstring m_memory_total;

        void init();

        /**
          Sets member user_name. Retrieves data from
          Windows GetUserNameW funcion.
        */
        void setUsername();

        /**
          Sets member machine_name and domain. Retrieves data from
          NetWkstaGetInfo function.
        */
        void setWorkstation();

        /**
          Sets member default_printer. Retrieves data from GetDefaultPrinterW
          function.
        */
        void setDefaultPrinter();

        /**
         Sets member offline_files. Retrieves data from OfflineFilesQueryStatus
         function.
        */
        void setOfflineFiles();

        /**
         Sets member os_name, os_version and os_architecture. Retrieves data through
         DCOM from a WMI namespace.
        */
        void setOperatingSystem();

        /**
         Sets member processor. Retrieves data through DCOM from a WMI namespace.
        */
        void setProcessor();

        /**
         Sets member memory_in_use and memory_total. Retrieves data
         GlobalMemoryStatusEx function.
        */
        void setMemory();
    };  // class System
}  // namespace WinApi