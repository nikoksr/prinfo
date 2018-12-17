#include "registry_printer.h"
#include "format.h"
#include "menu.h"

// Key-Auslesen
std::wstring read_key(HKEY hkey, std::wstring printer_name, std::wstring subkey,
                      const std::wstring *subkey_list, int list_size) {
  // Variablen
  auto hkey_copy = hkey;
  const auto subkeypfad = printer_name + L"\\" + subkey;

  // Kontrollieren, ob Subkey existiert
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

  // Alle Werte der jeweiligen Liste auslesen auslesen
  for (auto i = 0; i < list_size; ++i) {
    // Size, Typ und Daten auslesen
    DWORD type;
    DWORD size = max_value_name;
    TCHAR data[max_value_name];

    const auto result_get_value =
        RegGetValueW(hkey, subkeypfad.c_str(), subkey_list[i].c_str(),
                     RRF_RT_ANY, &type, data, &size);

    if (result_get_value != ERROR_SUCCESS) {
      // full_key_output += format_name_and_value(subkey_list[i],
      // general_error_message(result_get_value)) + L"\n";
      full_key_output += format_name_and_value(
          subkey_list[i], general_error_message(result_get_value));
      continue;
    }

    // Extract value
    std::wstring value;

    // REG_SZ value
    if (type != REG_MULTI_SZ) {
      value = std::wstring(data);
    }
    // REG_MULTI_SZ value
    else {
      value = format_multi_sz_key(hkey, printer_name, subkey, subkey_list[i]);
    }

    if (value.length() < 1) {
      value = L"-";
    }

    // full_key_output += format_name_and_value(subkey_list[i], value) +
    // L"\n\n";
    full_key_output += format_name_and_value(subkey_list[i], value) + L"\n";
  }

  // Key schließen
  RegCloseKey(hkey_copy);

  return full_key_output;
}

// LOCAL_MACHINE_Printers ausgeben
void show_lm_printers(std::wostream &stream) {
  // Key öffnen
  HKEY hkey;
  const std::wstring registry_pfad =
      L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Print\\Printers\\";
  auto result_reg_open_key = RegOpenKeyExW(
      HKEY_LOCAL_MACHINE, registry_pfad.c_str(), NULL, KEY_READ, &hkey);

  if (result_reg_open_key != ERROR_SUCCESS) {
    stream << general_error_message(result_reg_open_key) << "\n";
    return;
  }

  // Print header
  stream << get_separator_thick() << L"\n\n"
         << L" REGISTRY\n"
         << L" HKEY_LOCAL_MACHINE\\" << registry_pfad << L"\n";

  // Anzahl der Subkeys abfragen
  DWORD num_sub_keys;

  RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, &num_sub_keys, nullptr,
                   nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

  // Alle Subkeys auslesen
  for (unsigned index = 0; index < num_sub_keys; ++index) {
    // Key auslesen
    TCHAR name[max_key_length];
    auto size = max_key_length;
    auto result_reg_enum_key = RegEnumKeyExW(hkey, index, name, &size, nullptr,
                                             nullptr, nullptr, nullptr);

    if (result_reg_enum_key != ERROR_SUCCESS) {
      stream << general_error_message(result_reg_enum_key)
             << get_separator_thick() << "\n";
      continue;
    }

    // Erstelle strings mit Daten der Subkeys
    std::wstring dsdriver =
        read_key(hkey, name, L"DsDriver", list_dsdriver,
                 (sizeof(list_dsdriver) / sizeof(std::wstring)));
    std::wstring dsspooler =
        read_key(hkey, name, L"DsSpooler", list_dsspooler,
                 (sizeof(list_dsspooler) / sizeof(std::wstring)));
    std::wstring pnpdata =
        read_key(hkey, name, L"PNPData", list_pnpdata,
                 (sizeof(list_pnpdata) / sizeof(std::wstring)));
    std::wstring printerdriverdata =
        read_key(hkey, name, L"PrinterDriverData", list_printerdriverdata,
                 (sizeof(list_printerdriverdata) / sizeof(std::wstring)));

    // Print body
    stream << get_separator_thin() << L"\n\n"
           << L" Drucker: " << name << L"\n"
           << get_separator_thin() << L"\n\n"
           << dsdriver << L"\n"
           << dsspooler << L"\n"
           << pnpdata << L"\n"
           << printerdriverdata << L"\n";
  }

  // Key schließen
  RegCloseKey(hkey);
}

// CURRENT_USER_Printers ausgeben
void show_cu_printers(std::wostream &stream) {
  // KEY ÖFFNEN
  HKEY hkey;
  const std::wstring registry_pfad =
      L"Software\\Microsoft\\Windows NT\\CurrentVersion\\PrinterPorts";
  auto result_reg_open_key = RegOpenKeyExW(
      HKEY_CURRENT_USER, registry_pfad.c_str(), NULL, KEY_READ, &hkey);

  if (result_reg_open_key != ERROR_SUCCESS) {
    stream << general_error_message(result_reg_open_key) << L"\n";
    return;
  }

  // Anzahl der Werte Abfragen
  DWORD num_values;

  RegQueryInfoKeyW(hkey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   &num_values, nullptr, nullptr, nullptr, nullptr);

  // Print header
  stream << get_separator_thick() << L"\n\n"
         << L" REGISTRY\n"
         << L" HKEY_CURRENT_USER\\" << registry_pfad << L"\n"
         << get_separator_thin() << L"\n\n";

  // Alle Wertenamen und Werte auslesen
  for (unsigned index = 0; index < num_values; ++index) {
    // Variablen definieren/zurücksetzen je Durchlauf
    TCHAR value_name_buffer[max_value_name];
    DWORD value_name_buffer_size = max_value_name;
    TCHAR value_data_buffer[max_value_name];
    DWORD value_data_buffer_size = max_value_name;

    // Werte abfragen
    auto result_reg_enum_value = RegEnumValueW(
        hkey, index, value_name_buffer, &value_name_buffer_size, nullptr,
        nullptr, reinterpret_cast<LPBYTE>(value_data_buffer),
        &value_data_buffer_size);

    // Print body
    if (result_reg_enum_value != ERROR_SUCCESS) {
      stream << general_error_message(result_reg_enum_value) << L"\n";
      continue;
    }

    stream << format_name_and_value(value_name_buffer, value_data_buffer)
           << L"\n";
  }

  // Finaler Absatz
  stream << L"\n";

  // KEY SCHLIESSEN
  RegCloseKey(hkey);
}