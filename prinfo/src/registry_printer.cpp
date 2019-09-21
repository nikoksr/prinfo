#include "registry_printer.hpp"
#include "format.hpp"
#include "snippets.hpp"

namespace registry {

    /**
      Define static members
    */
    const std::array<std::wstring, 4> Printer::k_dsdriver_keys = {
        L"printLanguage", L"printBinNames", L"printMediaReady",
        L"printOrientationsSupported" };

    const std::array<std::wstring, 9> Printer::k_dsspooler_keys = {
        L"printerName", L"printShareName",  L"portName", L"driverName",
        L"serverName",  L"shortServerName", L"uNCName",  L"url",
        L"description" };

    const std::array<std::wstring, 2> Printer::k_pnpdata_keys = { L"Manufacturer",
                                                          L"HardwareID" };

    const std::array<std::wstring, 3> Printer::k_printerdriverdata_keys = {
        L"Model", L"TrayFormQueueProp", L"TrayFormTable" };

    constexpr DWORD Printer::k_max_key_length;
    constexpr DWORD Printer::k_max_value_name;
    constexpr wchar_t Printer::k_localmachine_path[];
    constexpr wchar_t Printer::k_currentuser_path[];

    /**
      Functions
    */
    template<std::size_t SIZE>
    static std::wstring Printer::readKey(const HKEY& hkey_origin, const std::wstring& printer_name,
        const std::wstring& subkey, const std::array<std::wstring, SIZE>& value_name_list) {
        /**
          Check if key is openable and open it if so.
        */
        HKEY hkey_copy = hkey_origin;
        const std::wstring subkeypfad = printer_name + L"\\" + subkey;
        const LSTATUS result_reg_open_key = RegOpenKeyExW(hkey_copy, subkeypfad.c_str(), NULL, KEY_READ, &hkey_copy);
        std::wstring full_key_output = L"";

        if (result_reg_open_key != ERROR_SUCCESS) {
            full_key_output += L"\\" + subkey + L" - " +
                Format::ErrorMessage(result_reg_open_key) +
                L"\n";
            return full_key_output;
        }
        else {
            full_key_output += L"\\" + subkey + L"\n";
        }

        /**
          Read all data from subkey.
        */
        for (std::size_t i = 0; i < SIZE; ++i) {
            DWORD type = 0;
            DWORD size = k_max_value_name;

            std::wstring value_name = value_name_list.at(i);
            auto data = std::make_unique<TCHAR[]>(k_max_value_name);

            const LSTATUS result_get_value =
                RegGetValueW(hkey_origin, subkeypfad.c_str(), value_name.c_str(),
                    RRF_RT_ANY, &type, data.get(), &size);

            if (result_get_value != ERROR_SUCCESS) {
                full_key_output += Format::NameValuePair(
                    value_name, Format::ErrorMessage(result_get_value));
                continue;
            }

            std::wstring value_data = L"";

            // REG_SZ
            if (type != REG_MULTI_SZ) {
                value_data = std::wstring(data.get());
            }
            // REG_MULTI_SZ
            else {
                value_data = Format::MultiSZKey(data.get());
            }

            if (value_data.length() < 1) {
                value_data = L"-";
            }

            full_key_output += Format::NameValuePair(value_name, value_data) + L"\n";
        }

        return full_key_output;
    }

    void Printer::DisplayLMPrinters(std::wostream& stream) {
        /**
          Check if key is openable.
        */
        HKEY hkey;
        LSTATUS result_reg_open_key = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE, k_localmachine_path, NULL, KEY_READ, &hkey);

        if (result_reg_open_key != ERROR_SUCCESS) {
            stream << Format::ErrorMessage(result_reg_open_key) << "\n";
            return;
        }

        /**
          Print header.
        */
        stream << snippets::k_separator_thick << L"\n\n"
            << L" REGISTRY\n" << L" HKEY_LOCAL_MACHINE\\" << k_localmachine_path << L"\n"
            << snippets::k_separator_thin << L"\n\n";

        // Number of entries/printers
        DWORD num_printers;
        RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, &num_printers, nullptr,
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

        /**
          Read value_data from all subkeys.
        */
        for (DWORD idx = 0; idx < num_printers; ++idx) {
            TCHAR name[k_max_key_length];
            DWORD size = k_max_key_length;

            LSTATUS result_reg_enum_key = RegEnumKeyExW(hkey, idx, name, &size, nullptr,
                nullptr, nullptr, nullptr);

            if (result_reg_enum_key != ERROR_SUCCESS) {
                stream << Format::ErrorMessage(result_reg_enum_key)
                    << snippets::k_separator_thick << "\n";
                continue;
            }

            /**
              Store value_data.
            */
            std::wstring dsdriver = readKey(hkey, name, L"DsDriver", k_dsdriver_keys);
            std::wstring dsspooler = readKey(hkey, name, L"DsSpooler", k_dsspooler_keys);
            std::wstring pnpdata = readKey(hkey, name, L"PnPData", k_pnpdata_keys);
            std::wstring printerdriverdata = readKey(hkey, name, L"PrinterDriverData", k_printerdriverdata_keys);

            /**
              Print body.
            */
            stream << L" Drucker: " << name << L"\n"
                << snippets::k_separator_thin << L"\n\n"
                << dsdriver << L"\n"
                << dsspooler << L"\n"
                << pnpdata << L"\n"
                << printerdriverdata << L"\n"
                << snippets::k_separator_thin << L"\n\n";
        }

        RegCloseKey(hkey);
    }

    void Printer::DisplayCUPrinters(std::wostream& stream) {
        /**
          Check if key is openable.
        */
        HKEY hkey;
        LSTATUS result_reg_open_key = RegOpenKeyExW(
            HKEY_CURRENT_USER, k_currentuser_path, NULL, KEY_READ, &hkey);

        if (result_reg_open_key != ERROR_SUCCESS) {
            stream << Format::ErrorMessage(result_reg_open_key) << L"\n";
            return;
        }

        // Number of Entries/Printers
        DWORD num_values;
        RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
            &num_values, nullptr, nullptr, nullptr, nullptr);

        /**
          Print header.
        */
        stream << snippets::k_separator_thick << L"\n\n"
            << L" REGISTRY\n" << L" HKEY_CURRENT_USER\\" << k_currentuser_path << L"\n"
            << snippets::k_separator_thin << L"\n\n";

        /**
          Read value_data from subkey.
        */

        for (DWORD idx = 0; idx < num_values; ++idx) {
            // Get name and data size
            DWORD buf_value_name_needed = k_max_value_name;
            DWORD buf_data_needed = 0;
            auto value_name = std::make_unique<TCHAR[]>(k_max_value_name);

            const LSTATUS result_get_size = RegEnumValueW(
                hkey, idx, value_name.get(), &buf_value_name_needed, nullptr,
                nullptr, nullptr, &buf_data_needed);

            if (result_get_size != ERROR_SUCCESS && result_get_size != ERROR_MORE_DATA) {
                stream << Format::ErrorMessage(result_get_size) << L"\n";
                continue;
            }

            // Needs to be increased by 1 to fit null-terminator
            buf_value_name_needed += 1;

            // Get data
            auto data = std::make_unique<TCHAR[]>(buf_data_needed);

            const LSTATUS result_get_data = RegEnumValueW(
                hkey, idx, value_name.get(), &buf_value_name_needed, nullptr,
                nullptr, reinterpret_cast<LPBYTE>(data.get()), &buf_data_needed);

            if (result_get_data != ERROR_SUCCESS) {
                stream << Format::ErrorMessage(result_get_data) << L"\n";
                continue;
            }

            stream << Format::NameValuePair(value_name.get(), data.get()) << L"\n";
        }

        stream << L"\n";
        RegCloseKey(hkey);
    }
}  // namespace Registry