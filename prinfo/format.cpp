#include "format.h"

#include <string>

std::wstring format_name_and_value(std::wstring name, std::wstring value) {
  const int max_length = 50;

  if (value.length() < max_length) {
    const std::wstring space(max_length - name.length() - 2, L' ');
    return (L" " + name + L":" + space + value);
  }

  auto lines = line_wrap(value, max_length);
  std::wstring formatted_value = L"";
  std::wstring space(max_length, L' ');
  auto index = 0;

  for (const auto &line : lines) {
    if (index == lines.size() - 1) {
      formatted_value += line;
      break;
    }

    formatted_value += (line + L"\n" + space);
    ++index;
  }

  space = std::wstring(max_length - name.length() - 2, L' ');
  return (L" " + name + L":" + space + formatted_value);
}

std::vector<std::wstring> line_wrap(std::wstring value,
                                    const unsigned max_line_length) {
  std::vector<std::wstring> lines;

  if (value.length() < 1) {
    return lines;
  }

  while (value.length() > max_line_length) {
    lines.push_back(value.substr(0, max_line_length));
    value = value.substr(max_line_length, value.length());
  }

  lines.push_back(value.substr(0, max_line_length));

  return lines;
}

// Multi_SZ Keys
std::wstring format_multi_sz_key(HKEY hkey, std::wstring name,
                                 std::wstring subkey, std::wstring value) {
  // Größe der Daten
  std::wstring pfad = name + L"\\" + subkey;
  DWORD size;
  auto result_get_value = RegGetValueW(hkey, pfad.c_str(), value.c_str(),
                                       RRF_RT_ANY, nullptr, nullptr, &size);

  if (result_get_value != ERROR_SUCCESS) {
    return general_error_message(GetLastError());
  }

  auto multisz_data = new TCHAR[size / sizeof(TCHAR)];

  // Daten abfragen
  result_get_value = RegGetValueW(hkey, pfad.c_str(), value.c_str(), RRF_RT_ANY,
                                  nullptr, multisz_data, &size);

  if (result_get_value != ERROR_SUCCESS) {
    delete[] multisz_data;
    return general_error_message(GetLastError());
  }

  // Multi_SZ String zusammensetzen
  std::wstring multisz_string = L"";
  auto multisz_string_length = wcslen(multisz_data);
  auto index = 0;

  while (multisz_string_length > 0) {
    multisz_string += &multisz_data[index];  // den String bis zu \0 auslesen
                                             // und wMultiszValue �bergeben
    multisz_string += L" ";  // danach ein Leerzeichen einf�gen; in der Registry
                             // w�re es ein Absatz
    index +=
        multisz_string_length + 1;  // + 1 wegen null-terminated String ( \0 )
    multisz_string_length = wcslen(&multisz_data[index]);
  }

  delete[] multisz_data;
  return multisz_string;
}

// Größe des jeweiligen Jobs umrechnen
std::wstring format_size_of_job(const DWORD job_size) {
  // Bytes
  if (job_size < 1024) {
    return std::to_wstring(job_size) + L" B";
  }
  // KiloBytes
  if (job_size < 1048576) {
    return std::to_wstring(job_size / 1024) + L" KB";
  }
  // MegaBytes
  if (job_size < 1073741824) {
    return std::to_wstring(job_size / 1048576) + L" MB";
  }
  // GigaBytes
  if (job_size < 1099511627776) {
    return std::to_wstring(job_size / 1073741824) + L" GB";
  }
  // TeraBytes
  if (job_size < 1125899906842624) {
    return std::to_wstring(job_size / 1099511627776) + L" TB";
  }

  return L"Zu groß";
}

std::wstring general_error_message(DWORD error_message_id) {
  // No error
  if (error_message_id == 0) {
    return std::wstring();
  }

  // Format error-message-id to wstring
  LPWSTR error_message_buffer = nullptr;
  const auto size = FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, error_message_id, LANG_USER_DEFAULT,
      reinterpret_cast<LPWSTR>(&error_message_buffer), 0, NULL);

  std::wstring error_message(error_message_buffer, size);

  // Free the buffer
  LocalFree(error_message_buffer);

  return error_message;
}

void handle_exception(std::exception_ptr exception_pointer) {
  try {
    if (exception_pointer) {
      std::rethrow_exception(exception_pointer);
    }
  } catch (const std::exception &exc) {
    std::wcout << L" Ein unerwarteter Fehler ist aufgetreten: \"" << exc.what()
               << L"\"\n\n"
               << L" Beliebige Taste drücken...";
    _getwch();
  }
}