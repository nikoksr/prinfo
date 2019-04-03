#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

namespace WinApi {
    class System {
    public:
        /**
         Formatted output of all data to specified stream.

         @param stream Stream that receives the data.
        */
        void print_system_info(std::wostream &stream);

        /**
          Sets member user_name. Retrieves data from
          Windows GetUserNameW funcion.
        */
        void set_username();

        /**
          Sets member machine_name and domain. Retrieves data from
          NetWkstaGetInfo function.
        */
        void set_workstation();

        /**
          Sets member default_printer. Retrieves data from GetDefaultPrinterW
          function.
        */
        void set_default_printer();

        /**
         Sets member offline_files. Retrieves data from OfflineFilesQueryStatus
         function.
        */
        void set_offline_files();

        /**
         Sets member os_name, os_version and os_architecture. Retrieves data through
         DCOM from a WMI namespace.
        */
        void set_operating_system();

        /**
         Sets member processor. Retrieves data through DCOM from a WMI namespace.
        */
        void set_processor();

        /**
         Sets member memory_in_use and memory_total. Retrieves data
         GlobalMemoryStatusEx function.
        */
        void set_memory();

        /**
          Getter functions
        */
        const std::wstring& get_user_name() const;
        const std::wstring& get_workstation_name() const;
        const std::wstring& get_offline_files() const;
        const std::wstring& get_default_printer() const;
        const std::wstring& get_operating_system() const;
        const std::wstring& get_processor() const;
        const std::wstring& get_memory() const;

    private:
        /**
         Initialize COM library and configure security
        */
        HRESULT prepare_wmi();

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
    };  // class System
}  // namespace WinApi