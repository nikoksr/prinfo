#include "system.h"
#include "menu.h"
#include "format.h"

#include <LM.h>
#include <WbemCli.h>
#include <Windows.h>
#include <cscapi.h>
#include <iostream>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "CscApi.lib")

// System Infos
void show_system_info(std::wostream &stream) {
  // Initialize COM library and configure security
  auto result_prepare_wmi = prepare_wmi();

  // Username
  std::wstring username;
  auto result_get_username = get_username(&username);

  // Computername und Domain
  std::wstring computer_name, domain;
  auto result_get_workstation = get_workstation(&computer_name, &domain);

  // Standarddrucker
  std::wstring default_printer;
  auto result_get_default_printer = get_default_printer(&default_printer);

  // Offline Dateien
  std::wstring offline_files;
  auto result_get_offline_files = get_offline_files(&offline_files);

  // Betriebssystem Name, Version und Architektur
  std::wstring operating_system, version, architecture;
  auto result_get_operating_system =
      get_operating_system(&operating_system, &version, &architecture);

  // Prozessor
  std::wstring processor;
  auto result_get_processor = get_processor(&processor);

  // RAM Größe
  std::wstring mem_physical_total, mem_physical_in_use, mem_virtual_total,
      mem_virtual_in_use;
  auto result_get_ram = get_memory(&mem_physical_total, &mem_physical_in_use);

  stream << Helper::Menu::get_separator_thick() << L"\n\n"
         << L" Systeminformationen\n"
         << Helper::Menu::get_separator_thin() << L"\n\n"
         << format_name_and_value(L"Username", username) << L"\n"
         << format_name_and_value(L"Computername", computer_name) << L"\n"
         << format_name_and_value(L"Domain", domain) << L"\n\n"
         << format_name_and_value(L"Standarddrucker", default_printer) << L"\n"
         << format_name_and_value(L"Offlinedateien", offline_files) << L"\n\n"
         << format_name_and_value(L"Betriebssystem", operating_system) << L"\n"
         << format_name_and_value(L"Version", version) << L"\n"
         << format_name_and_value(L"Architektur", architecture) << L"\n\n"
         << format_name_and_value(L"Prozessor", processor) << L"\n"
         << format_name_and_value(L"RAM Physical", (mem_physical_in_use + L"/" +
                                                    mem_physical_total))
         << L" MB\n\n";
}

HRESULT prepare_wmi() {
  // Initialize the COM library
  auto result_com_init = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  if (!SUCCEEDED(result_com_init)) {
    return result_com_init;
  }

  // Register security and set the default security values
  auto result_init_security = CoInitializeSecurity(
      nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_CONNECT,
      RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);

  if (!SUCCEEDED(result_init_security) ||
      result_init_security == RPC_E_TOO_LATE) {
    return result_init_security;
  }

  return ERROR_SUCCESS;
}

BOOL get_username(std::wstring *username) {
  DWORD username_buffer_size = 255;
  wchar_t temp_username[255];

  auto result_username = GetUserNameW(temp_username, &username_buffer_size);
  *username = std::wstring(temp_username);

  return result_username;
}

DWORD get_workstation(std::wstring *computer_name, std::wstring *domain) {
  PWKSTA_INFO_102 wksta_info;
  auto result_wksta =
      NetWkstaGetInfo(nullptr, 102, reinterpret_cast<LPBYTE *>(&wksta_info));

  *computer_name = std::wstring(wksta_info->wki102_computername);
  *domain = std::wstring(wksta_info->wki102_langroup);
  NetApiBufferFree(wksta_info);

  return result_wksta;
}

BOOL get_default_printer(std::wstring *default_printer) {
  DWORD default_printer_buffer_size = 255;
  wchar_t temp_default_printer[255];

  auto result_default_printer =
      GetDefaultPrinterW(temp_default_printer, &default_printer_buffer_size);
  *default_printer = std::wstring(temp_default_printer);

  return result_default_printer;
}

DWORD get_offline_files(std::wstring *offline_files) {
  BOOL offline_files_active;
  auto result_offline_files =
      OfflineFilesQueryStatus(&offline_files_active, nullptr);

  // Auswertung
  switch (offline_files_active) {
    case TRUE:
      *offline_files = L"Aktiviert";
      break;
    case FALSE:
      *offline_files = L"Offlinedateien oder CSC-Treiber deaktiviert";
      break;
    default:
      *offline_files = L"Status unbekannt";
      break;
  }

  return result_offline_files;
}

HRESULT get_operating_system(std::wstring *operating_system,
                             std::wstring *version,
                             std::wstring *architecture) {
  // Create a single uninitialized object of the class on the local system
  IWbemLocator *wbem_locator;
  auto result_create_instance = CoCreateInstance(
      CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&wbem_locator));

  if (!SUCCEEDED(result_create_instance)) {
    return result_create_instance;
  }

  // Create a connection through DCOM to a WMI namespace on the computer
  // specified in the strNetworkResource parameter
  IWbemServices *wbem_service;
  auto result_connect_server = wbem_locator->ConnectServer(
      const_cast<BSTR>(L"root\\CIMV2"), NULL, NULL, NULL,
      WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &wbem_service);
  wbem_locator->Release();

  if (!SUCCEEDED(result_connect_server)) {
    return result_connect_server;
  }

  // Execute a query to retrieve objects from WMI
  IEnumWbemClassObject *wbem_object_enum;
  auto result_exec_query = wbem_service->ExecQuery(
      const_cast<BSTR>(L"WQL"),
      const_cast<BSTR>(
          L"SELECT Name,Version,OSArchitecture FROM Win32_OperatingSystem"),
      WBEM_FLAG_FORWARD_ONLY, NULL, &wbem_object_enum);
  wbem_service->Release();

  if (!SUCCEEDED(result_exec_query)) {
    return result_exec_query;
  }

  // Iterate through retrieved objects
  IWbemClassObject *wbem_object;
  ULONG number_objects;
  VARIANT value;
  VariantInit(&value);

  while (wbem_object_enum->Next(WBEM_INFINITE, 1, &wbem_object,
                                &number_objects) != WBEM_S_FALSE) {
    auto result_get_name = wbem_object->Get(L"Name", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_get_name) && value.vt == VT_BSTR) {
      *operating_system = value.bstrVal;
    } else {
      *operating_system = L"Nicht identifizierbar";
    }

    auto result_get_version =
        wbem_object->Get(L"Version", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_get_version) && value.vt == VT_BSTR) {
      *version = value.bstrVal;
    } else {
      *version = L"Nicht identifizierbar";
    }

    auto result_get_architecture =
        wbem_object->Get(L"OSArchitecture", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_get_architecture) && value.vt == VT_BSTR) {
      *architecture = value.bstrVal;
    } else {
      *architecture = L"Nicht identifizierbar";
    }
  }

  if (wbem_object_enum) {
    wbem_object_enum->Release();
  }

  if (wbem_object) {
    wbem_object->Release();
  }

  // VariantClear(&value);

  return ERROR_SUCCESS;
}

HRESULT get_processor(std::wstring *processor) {
  // Create a single uninitialized object of the class on the local system
  IWbemLocator *wbem_locator;
  auto result_create_instance = CoCreateInstance(
      CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&wbem_locator));

  if (!SUCCEEDED(result_create_instance)) {
    wbem_locator->Release();
    return result_create_instance;
  }

  // Create a connection through DCOM to a WMI namespace on the computer
  // specified in the strNetworkResource parameter
  IWbemServices *wbem_service;
  auto result_connect_server = wbem_locator->ConnectServer(
      const_cast<BSTR>(L"root\\CIMV2"), NULL, NULL, NULL,
      WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &wbem_service);
  wbem_locator->Release();

  if (!SUCCEEDED(result_connect_server)) {
    wbem_service->Release();
    return result_connect_server;
  }

  // Execute a query to retrieve objects from WMI
  IEnumWbemClassObject *wbem_object_enum;
  auto result_exec_query = wbem_service->ExecQuery(
      const_cast<BSTR>(L"WQL"),
      const_cast<BSTR>(L"SELECT Name FROM Win32_Processor"),
      WBEM_FLAG_FORWARD_ONLY, NULL, &wbem_object_enum);
  wbem_service->Release();

  if (!SUCCEEDED(result_exec_query)) {
    wbem_object_enum->Release();
    return result_exec_query;
  }

  // Iterate through retrieved objects
  IWbemClassObject *wbem_object;
  ULONG number_objects;
  VARIANT value;
  VariantInit(&value);

  while (wbem_object_enum->Next(WBEM_INFINITE, 1, &wbem_object,
                                &number_objects) != WBEM_S_FALSE) {
    auto result_wbem_object_get =
        wbem_object->Get(L"Name", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_wbem_object_get) && value.vt == VT_BSTR) {
      *processor = value.bstrVal;
    } else {
      *processor = L"Nicht identifizierbar";
    }
  }

  wbem_object_enum->Release();
  wbem_object->Release();
  VariantClear(&value);

  return ERROR_SUCCESS;
}

BOOL get_memory(std::wstring *mem_physical_total,
                std::wstring *mem_physical_in_use) {
  constexpr auto mebibyte = 1024 * 1024;
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  auto result_get_ram = GlobalMemoryStatusEx(&statex);

  if (result_get_ram) {
    auto mpt = statex.ullTotalPhys / mebibyte;
    auto mpiu = mpt - (statex.ullAvailPhys / mebibyte);

    *mem_physical_total = std::to_wstring(mpt);
    *mem_physical_in_use = std::to_wstring(mpiu);
  } else {
    *mem_physical_total = L"Konnte nicht ausgelesen werden";
    *mem_physical_in_use = L"Konnte nicht ausgelesen werden";
  }

  return result_get_ram;
}