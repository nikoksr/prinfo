#pragma once

#include <Windows.h>
#include <iostream>
#include <array>

namespace registry {
    class Printer {
    public:
        /**
         Streams data found under LocalMachine to specified output.

         @param &stream Reference of stream which should be used for data output.
       */
        static void DisplayLMPrinters(std::wostream &stream);

        /**
          Streams data found under CurrentUser to specified output.

          @param &stream Reference of stream which should be used for data output.
        */
        static void DisplayCUPrinters(std::wostream &stream);

    private:
        /**
          Defines max length of registry key.
        */
        static constexpr DWORD k_max_key_length = 255;

        /**
          Defines max length of registry-entry name.
        */
        static constexpr DWORD k_max_value_name = 16383;

        /**
          Defines values-names to search for in specific registry-entry.
        */
        static const std::array<std::wstring, 4> k_dsdriver_keys;
        static const std::array<std::wstring, 9> k_dsspooler_keys;
        static const std::array<std::wstring, 2> k_pnpdata_keys;
        static const std::array<std::wstring, 3> k_printerdriverdata_keys;

        static constexpr wchar_t k_localmachine_path[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Print\\Printers";
        static constexpr wchar_t k_currentuser_path[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\PrinterPorts";

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
        static std::wstring readKey(const HKEY& hkey_origin, const std::wstring& printer_name,
            const std::wstring& subkey, const std::array<std::wstring, SIZE>& value_name_list);

    };  // class Printer
}  // namespace Registry