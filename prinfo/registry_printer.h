#pragma once

#include <Windows.h>
#include <iostream>

namespace Registry {
class Printer {
 public:
  /**
   Streams data found under LocalMachine to specified output.

   @param &stream Reference of stream which should be used for data output.
 */
  void show_lm_printers(std::wostream &stream = std::wcout);

  /**
    Streams data found under CurrentUser to specified output.

    @param &stream Reference of stream which should be used for data output.
  */
  void show_cu_printers(std::wostream &stream = std::wcout);

 private:
  /**
    Defines max length of registry key.
  */
  const static DWORD max_key_length = 255;

  /**
    Defines max length of registry-entry name.
  */
  const static DWORD max_value_name = 16383;

  /**
    Defines values-names to search for in specific registry-entry.
  */
  const static std::wstring dsdriver_values_names[];
  const static std::wstring dsspooler_values_names[];
  const static std::wstring pnpdata_values_names[];
  const static std::wstring printerdriverdata_values_names[];

  /**
    Reads specified registry key and filters out the results for
    the specified subkey_values_names (e.g pnpdata_values_names).

    @param hkey Handle for registry-key.
    @param printer_name Name of the printer to look for.
    @param subkey Path of the subkey in which to look for printer.
    @param subkey_list List of values to look for in subkey.
    @pararm list_size Size of the subkey_values_names.
  */
  std::wstring read_key(HKEY hkey, std::wstring printer_name,
                        std::wstring subkey, const std::wstring *subkey_list,
                        int list_size);
};  // class Printer
}  // namespace Registry