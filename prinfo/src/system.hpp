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
        void print_system_info(std::wostream &stream = std::wcout);

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
        std::wstring get_user_name;
        std::wstring get_workstation_name;
        std::wstring get_offline_files;
        std::wstring get_default_printer;
        std::wstring get_operating_system;
        std::wstring get_processor;
        std::wstring get_memory;

    private:
        /**
         Initialize COM library and configure security
        */
        HRESULT prepare_wmi();

        std::wstring user_name;
        std::wstring machine_name;
        std::wstring domain;
        std::wstring offline_files;
        std::wstring default_printer;
        std::wstring os_name;
        std::wstring os_version;
        std::wstring os_architecture;
        std::wstring processor;
        std::wstring memory_in_use;
        std::wstring memory_total;
    };  // class System
}  // namespace WinApi