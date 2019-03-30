#pragma once

// Set minimum Windows version(Windows Header Files)
// https://docs.microsoft.com/en-us/windows/desktop/api/winreg/nf-winreg-reggetvaluew
// https://docs.microsoft.com/de-de/windows/desktop/WinProg/using-the-windows-headers
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 
#endif

#include <Windows.h>
#include <iostream>
#include <array>

namespace Registry {
    class Printer {
    public:
        /**
         Streams data found under LocalMachine to specified output.

         @param &stream Reference of stream which should be used for data output.
       */
        static void show_lm_printers(std::wostream &stream);

        /**
          Streams data found under CurrentUser to specified output.

          @param &stream Reference of stream which should be used for data output.
        */
        static void show_cu_printers(std::wostream &stream);

    private:
        /**
          Defines max length of registry key.
        */
        constexpr static DWORD m_max_key_length = 255;

        /**
          Defines max length of registry-entry name.
        */
        constexpr static DWORD m_max_value_name = 16383;

        /**
          Defines values-names to search for in specific registry-entry.
        */
        static const std::array<std::wstring, 4> m_dsdriver_values_names;
        static const std::array<std::wstring, 9> m_dsspooler_values_names;
        static const std::array<std::wstring, 2> m_pnpdata_values_names;
        static const std::array<std::wstring, 3> m_printerdriverdata_values_names;

        static constexpr wchar_t localmachine_reg_path[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Print\\Printers\\";
        static constexpr wchar_t currentuser_reg_path[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\PrinterPorts";

        /**
          Reads specified registry key and filters out the results for
          the specified subkey_values_names (e.g pnpdata_values_names).

          @param hkey Handle for registry-key.
          @param printer_name Name of the printer to look for.
          @param subkey Path of the subkey in which to look for printer.
          @param subkey_list List of values to look for in subkey.
          @pararm list_size Size of the subkey_values_names.
        */
        template<std::size_t SIZE>
        static std::wstring read_key(HKEY hkey, const std::wstring& printer_name,
            const std::wstring& subkey, const std::array<std::wstring, SIZE>& subkey_list);
    };  // class Printer
}  // namespace Registry