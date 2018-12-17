#include "registry_printer.h"
#include "format.h"
#include "menu.h"

namespace Registry {

/**
  Define static members
*/

const std::wstring Printer::dsdriver_values_names[] = {
    L"printLanguage", L"printBinNames", L"printMediaReady",
    L"printOrientationsSupported"};

const std::wstring Printer::dsspooler_values_names[] = {
    L"printerName", L"printShareName",  L"portName", L"driverName",
    L"serverName",  L"shortServerName", L"uNCName",  L"url",
    L"description"};

const std::wstring Printer::pnpdata_values_names[] = {L"Manufacturer",
                                                      L"HardwareID"};

const std::wstring Printer::printerdriverdata_values_names[] = {
    L"Model", L"TrayFormQueueProp", L"TrayFormTable"};

/**
  Functions
*/

std::wstring Printer::read_key(HKEY hkey, std::wstring printer_name,
                               std::wstring subkey,
                               const std::wstring *subkey_list, int list_size) {
  /**
    Check if key is openable and open it if so.
  */
  auto hkey_copy = hkey;
  const auto subkeypfad = printer_name + L"\\" + subkey;
  const auto result_reg_open_key =
      RegOpenKeyExW(hkey_copy, subkeypfad.c_str(), NULL, KEY_READ, &hkey_copy);
  std::wstring full_key_output = L"";

  if (result_reg_open_key != ERROR_SUCCESS) {
    full_key_output += L"\\" + subkey + L" - " +
                       general_error_message(result_reg_open_key) + L"\n";
    RegCloseKey(hkey_copy);
    return full_key_output;
  } else {
    full_key_output += L"\\" + subkey + L"\n";
  }

  /**
    Read all data from subkey.
  */
  for (auto i = 0; i < list_size; ++i) {
    DWORD type;
    DWORD size = max_value_name;
    TCHAR data[max_value_name];

    const auto result_get_value =
        RegGetValueW(hkey, subkeypfad.c_str(), subkey_list[i].c_str(),
                     RRF_RT_ANY, &type, data, &size);

    if (result_get_value != ERROR_SUCCESS) {
      full_key_output += format_name_and_value(
          subkey_list[i], general_error_message(result_get_value));
      continue;
    }

    std::wstring value_data;

    // REG_SZ value_data
    if (type != REG_MULTI_SZ) {
      value_data = std::wstring(data);
    }
    // REG_MULTI_SZ value_data
    else {
      value_data =
          format_multi_sz_key(hkey, printer_name, subkey, subkey_list[i]);
    }

    if (value_data.length() < 1) {
      value_data = L"-";
    }

    full_key_output +=
        format_name_and_value(subkey_list[i], value_data) + L"\n";
  }

  RegCloseKey(hkey_copy);
  return full_key_output;
}

void Printer::show_lm_printers(std::wostream &stream) {
  /**
    Check if key is openable.
  */
  HKEY hkey;
  const std::wstring registry_pfad =
      L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Print\\Printers\\";
  auto result_reg_open_key = RegOpenKeyExW(
      HKEY_LOCAL_MACHINE, registry_pfad.c_str(), NULL, KEY_READ, &hkey);

  if (result_reg_open_key != ERROR_SUCCESS) {
    stream << general_error_message(result_reg_open_key) << "\n";
    return;
  }

  /**
    Print header.
  */
  stream << get_separator_thick() << L"\n\n"
         << L" REGISTRY\n"
         << L" HKEY_LOCAL_MACHINE\\" << registry_pfad << L"\n";

  DWORD num_sub_keys;
  RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, &num_sub_keys, nullptr,
                   nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  /**
    Read value_data from all subkeys.
  */
  for (unsigned index = 0; index < num_sub_keys; ++index) {
    TCHAR name[max_key_length];
    auto size = max_key_length;
    auto result_reg_enum_key = RegEnumKeyExW(hkey, index, name, &size, nullptr,
                                             nullptr, nullptr, nullptr);

    if (result_reg_enum_key != ERROR_SUCCESS) {
      stream << general_error_message(result_reg_enum_key)
             << get_separator_thick() << "\n";
      continue;
    }

    /**
      Store value_data.
    */
    std::wstring dsdriver =
        read_key(hkey, name, L"DsDriver", dsdriver_values_names,
                 (sizeof(dsdriver_values_names) / sizeof(std::wstring)));
    std::wstring dsspooler =
        read_key(hkey, name, L"DsSpooler", dsspooler_values_names,
                 (sizeof(dsspooler_values_names) / sizeof(std::wstring)));
    std::wstring pnpdata =
        read_key(hkey, name, L"PNPData", pnpdata_values_names,
                 (sizeof(pnpdata_values_names) / sizeof(std::wstring)));
    std::wstring printerdriverdata = read_key(
        hkey, name, L"PrinterDriverData", printerdriverdata_values_names,
        (sizeof(printerdriverdata_values_names) / sizeof(std::wstring)));

    /**
      Print body.
    */
    stream << get_separator_thin() << L"\n\n"
           << L" Drucker: " << name << L"\n"
           << get_separator_thin() << L"\n\n"
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
  const std::wstring registry_pfad =
      L"Software\\Microsoft\\Windows NT\\CurrentVersion\\PrinterPorts";
  auto result_reg_open_key = RegOpenKeyExW(
      HKEY_CURRENT_USER, registry_pfad.c_str(), NULL, KEY_READ, &hkey);

  if (result_reg_open_key != ERROR_SUCCESS) {
    stream << general_error_message(result_reg_open_key) << L"\n";
    return;
  }

  DWORD num_values;
  RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   &num_values, nullptr, nullptr, nullptr, nullptr);

  /**
    Print header.
  */
  stream << get_separator_thick() << L"\n\n"
         << L" REGISTRY\n"
         << L" HKEY_CURRENT_USER\\" << registry_pfad << L"\n"
         << get_separator_thin() << L"\n\n";

  /**
    Read value_data from subkey.
  */
  for (unsigned index = 0; index < num_values; ++index) {
    TCHAR value_name_buffer[max_value_name];
    DWORD value_name_buffer_size = max_value_name;
    TCHAR value_data_buffer[max_value_name];
    DWORD value_data_buffer_size = max_value_name;

    auto result_reg_enum_value = RegEnumValueW(
        hkey, index, value_name_buffer, &value_name_buffer_size, nullptr,
        nullptr, reinterpret_cast<LPBYTE>(value_data_buffer),
        &value_data_buffer_size);

    /**
      Print body.
    */
    if (result_reg_enum_value != ERROR_SUCCESS) {
      stream << general_error_message(result_reg_enum_value) << L"\n";
      continue;
    }

    stream << format_name_and_value(value_name_buffer, value_data_buffer)
           << L"\n";
  }

  stream << L"\n";
  RegCloseKey(hkey);
}
}  // namespace Registry