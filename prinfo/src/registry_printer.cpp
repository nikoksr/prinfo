#include "registry_printer.hpp"
#include "format.hpp"
#include "snippets.hpp"

namespace Registry {

    /**
      Define static members
    */
    const std::array<std::wstring, 4> Printer::m_dsdriver_values_names = {
        L"printLanguage", L"printBinNames", L"printMediaReady",
        L"printOrientationsSupported" };

    const std::array<std::wstring, 9> Printer::m_dsspooler_values_names = {
        L"printerName", L"printShareName",  L"portName", L"driverName",
        L"serverName",  L"shortServerName", L"uNCName",  L"url",
        L"description" };

    const std::array<std::wstring, 2> Printer::m_pnpdata_values_names = { L"Manufacturer",
                                                          L"HardwareID" };

    const std::array<std::wstring, 3> Printer::m_printerdriverdata_values_names = {
        L"Model", L"TrayFormQueueProp", L"TrayFormTable" };

    constexpr DWORD Printer::m_max_key_length;
    constexpr DWORD Printer::m_max_value_name;
    constexpr wchar_t Printer::localmachine_reg_path[];
    constexpr wchar_t Printer::currentuser_reg_path[];

    /**
      Functions
    */
    template<std::size_t SIZE>
    static std::wstring Printer::read_key(const HKEY& hkey_origin, const std::wstring& printer_name,
        const std::wstring& subkey, const std::array<std::wstring, SIZE>& value_name_list) {
        /**
          Check if key is openable and open it if so.
        */
        HKEY hkey_copy = hkey_origin;
        const std::wstring subkeypfad = printer_name + L"\\" + subkey;
        const LSTATUS result_reg_open_key =
            RegOpenKeyExW(hkey_copy, subkeypfad.c_str(), NULL, KEY_READ, &hkey_copy);
        std::wstring full_key_output = L"";

        if (result_reg_open_key != ERROR_SUCCESS) {
            full_key_output += L"\\" + subkey + L" - " +
                Helper::Format::error_message(result_reg_open_key) +
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
            DWORD size = m_max_value_name;
            TCHAR data[m_max_value_name] = L"";
            std::wstring value_name = value_name_list.at(i);

            const LSTATUS result_get_value =
                RegGetValueW(hkey_origin, subkeypfad.c_str(), value_name.c_str(),
                    RRF_RT_ANY, &type, data, &size);

            if (result_get_value != ERROR_SUCCESS) {
                full_key_output += Helper::Format::name_and_value(
                    value_name, Helper::Format::error_message(result_get_value));
                continue;
            }

            std::wstring value_data = L"";

            // REG_SZ
            if (type != REG_MULTI_SZ) {
                value_data = std::wstring(data);
            }
            // REG_MULTI_SZ
            else {
                value_data = Helper::Format::multi_sz_key(data);
            }

            if (value_data.length() < 1) {
                value_data = L"-";
            }

            full_key_output +=
                Helper::Format::name_and_value(value_name, value_data) + L"\n";
        }

        return full_key_output;
    }

    void Printer::show_lm_printers(std::wostream &stream) {
        /**
          Check if key is openable.
        */
        HKEY hkey;
        LSTATUS result_reg_open_key = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE, localmachine_reg_path, NULL, KEY_READ, &hkey);

        if (result_reg_open_key != ERROR_SUCCESS) {
            stream << Helper::Format::error_message(result_reg_open_key) << "\n";
            return;
        }

        /**
          Print header.
        */
        stream << Helper::Snippets::separator_thick << L"\n\n"
            << L" REGISTRY\n"
            << L" HKEY_LOCAL_MACHINE\\" << localmachine_reg_path << L"\n";

        // Number of entries/printers
        DWORD num_printers;
        RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, &num_printers, nullptr,
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

        /**
          Read value_data from all subkeys.
        */
        for (DWORD idx = 0; idx < num_printers; ++idx) {
            TCHAR name[m_max_key_length];
            DWORD size = m_max_key_length;

            LSTATUS result_reg_enum_key = RegEnumKeyExW(hkey, idx, name, &size, nullptr,
                nullptr, nullptr, nullptr);

            if (result_reg_enum_key != ERROR_SUCCESS) {
                stream << Helper::Format::error_message(result_reg_enum_key)
                    << Helper::Snippets::separator_thick << "\n";
                continue;
            }

            /**
              Store value_data.
            */
            std::wstring dsdriver = read_key(hkey, name, L"DsDriver", m_dsdriver_values_names);
            std::wstring dsspooler = read_key(hkey, name, L"DsSpooler", m_dsspooler_values_names);
            std::wstring pnpdata = read_key(hkey, name, L"PnPData", m_pnpdata_values_names);
            std::wstring printerdriverdata = read_key(hkey, name, L"PrinterDriverData", m_printerdriverdata_values_names);

            /**
              Print body.
            */
            stream << Helper::Snippets::separator_thin << L"\n\n"
                << L" Drucker: " << name << L"\n"
                << Helper::Snippets::separator_thin << L"\n\n"
                << dsdriver << L"\n"
                << dsspooler << L"\n"
                << pnpdata << L"\n"
                << printerdriverdata << L"\n";
        }

        RegCloseKey(hkey);
    }

    void Printer::show_cu_printers(std::wostream &stream) {
        /**
          Check if key is openable.
        */
        HKEY hkey;
        LSTATUS result_reg_open_key = RegOpenKeyExW(
            HKEY_CURRENT_USER, currentuser_reg_path, NULL, KEY_READ, &hkey);

        if (result_reg_open_key != ERROR_SUCCESS) {
            stream << Helper::Format::error_message(result_reg_open_key) << L"\n";
            return;
        }

        DWORD num_values;
        RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
            &num_values, nullptr, nullptr, nullptr, nullptr);

        /**
          Print header.
        */
        stream << Helper::Snippets::separator_thick << L"\n\n"
            << L" REGISTRY\n"
            << L" HKEY_CURRENT_USER\\" << currentuser_reg_path << L"\n"
            << Helper::Snippets::separator_thin << L"\n\n";

        /**
          Read value_data from subkey.
        */
        for (DWORD idx = 0; idx < num_values; ++idx) {
            TCHAR value_name_buffer[m_max_value_name];
            DWORD value_name_buffer_size = m_max_value_name;
            TCHAR value_data_buffer[m_max_value_name];
            DWORD value_data_buffer_size = m_max_value_name;

            LSTATUS result_reg_enum_value = RegEnumValueW(
                hkey, idx, value_name_buffer, &value_name_buffer_size, nullptr,
                nullptr, reinterpret_cast<LPBYTE>(value_data_buffer),
                &value_data_buffer_size);

            /**
              Print body.
            */
            if (result_reg_enum_value != ERROR_SUCCESS) {
                stream << Helper::Format::error_message(result_reg_enum_value) << L"\n";
                continue;
            }

            stream << Helper::Format::name_and_value(value_name_buffer,
                value_data_buffer)
                << L"\n";
        }

        stream << L"\n";
        RegCloseKey(hkey);
    }
}  // namespace Registry