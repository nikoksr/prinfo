#include "system.h"
#include "format.h"
#include "menu.h"

#include <LM.h>
#include <WbemCli.h>
#include <Windows.h>
#include <comdef.h>
#include <cscapi.h>
#include <iostream>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "CscApi.lib")

namespace WinApi {
HRESULT System::prepare_wmi() {
  /**
    Initialize the COM library.
  */
  auto result_com_init = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  if (!SUCCEEDED(result_com_init)) {
    return result_com_init;
  }

  /**
    Register security and set the default security values.
  */
  auto result_init_security = CoInitializeSecurity(
      nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_CONNECT,
      RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);

  return result_init_security;
}

void System::set_username() {
  DWORD username_buffer_size = 255;
  wchar_t temp_username[255];

  auto result_username = GetUserNameW(temp_username, &username_buffer_size);

  if (result_username == 0) {
    user_name = Helper::Format::error_message(GetLastError());
  } else {
    user_name = std::wstring(temp_username);
  }
}

void System::set_workstation() {
  PWKSTA_INFO_102 wksta_info;
  auto result_wksta =
      NetWkstaGetInfo(nullptr, 102, reinterpret_cast<LPBYTE *>(&wksta_info));

  if (result_wksta != NERR_Success) {
    auto err = Helper::Format::error_message(result_wksta);
    machine_name = err;
    domain = err;
  } else {
    machine_name = std::wstring(wksta_info->wki102_computername);
    domain = std::wstring(wksta_info->wki102_langroup);
  }

  NetApiBufferFree(wksta_info);
}

void System::set_default_printer() {
  DWORD default_printer_buffer_size = 255;
  wchar_t temp_default_printer[255];

  auto result_default_printer =
      GetDefaultPrinterW(temp_default_printer, &default_printer_buffer_size);

  if (result_default_printer == 0) {
    default_printer = Helper::Format::error_message(GetLastError());
  } else {
    default_printer = std::wstring(temp_default_printer);
  }
}

void System::set_offline_files() {
  BOOL offline_files_active;
  auto result_offline_files =
      OfflineFilesQueryStatus(&offline_files_active, nullptr);

  if (result_offline_files != ERROR_SUCCESS) {
    offline_files = Helper::Format::error_message(result_offline_files);
  } else {
    switch (offline_files_active) {
      case TRUE:
        offline_files = L"Aktiviert";
        break;
      case FALSE:
        offline_files = L"Offlinedateien oder CSC-Treiber deaktiviert";
        break;
      default:
        offline_files = L"Status unbekannt";
        break;
    }
  }
}

void System::set_operating_system() {
  /**
    Create a single uninitialized object of the class on the local system.
  */
  IWbemLocator *wbem_locator;
  auto result_create_instance = CoCreateInstance(
      CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&wbem_locator));

  if (!SUCCEEDED(result_create_instance)) {
    _com_error err(result_create_instance);
    auto err_message = err.ErrorMessage();
    os_name = err_message;
    os_version = err_message;
    os_architecture = err_message;
    wbem_locator->Release();
  }

  /**
    Create a connection through DCOM to a WMI namespace on the computer
    specified in the strNetworkResource parameter.
  */
  IWbemServices *wbem_service;
  auto result_connect_server = wbem_locator->ConnectServer(
      const_cast<BSTR>(L"root\\CIMV2"), NULL, NULL, NULL,
      WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &wbem_service);
  wbem_locator->Release();

  if (!SUCCEEDED(result_connect_server)) {
    _com_error err(result_connect_server);
    auto err_message = err.ErrorMessage();
    os_name = err_message;
    os_version = err_message;
    os_architecture = err_message;
    wbem_service->Release();
  }

  /**
    Execute a query to retrieve objects from WMI
  */
  IEnumWbemClassObject *wbem_object_enum;
  auto result_exec_query = wbem_service->ExecQuery(
      const_cast<BSTR>(L"WQL"),
      const_cast<BSTR>(
          L"SELECT Name,Version,OSArchitecture FROM Win32_OperatingSystem"),
      WBEM_FLAG_FORWARD_ONLY, NULL, &wbem_object_enum);
  wbem_service->Release();

  if (!SUCCEEDED(result_exec_query)) {
    _com_error err(result_exec_query);
    auto err_message = err.ErrorMessage();
    os_name = err_message;
    os_version = err_message;
    os_architecture = err_message;
  }

  /**
    Iterate through retrieved objects.
  */
  IWbemClassObject *wbem_object;
  ULONG number_objects;
  VARIANT value;
  VariantInit(&value);

  while (wbem_object_enum->Next(WBEM_INFINITE, 1, &wbem_object,
                                &number_objects) != WBEM_S_FALSE) {
    auto result_get_name = wbem_object->Get(L"Name", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_get_name) && value.vt == VT_BSTR) {
      os_name = value.bstrVal;
    } else {
      os_name = L"Nicht identifizierbar";
    }

    auto result_get_version =
        wbem_object->Get(L"Version", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_get_version) && value.vt == VT_BSTR) {
      os_version = value.bstrVal;
    } else {
      os_version = L"Nicht identifizierbar";
    }

    auto result_get_architecture =
        wbem_object->Get(L"OSArchitecture", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_get_architecture) && value.vt == VT_BSTR) {
      os_architecture = value.bstrVal;
    } else {
      os_architecture = L"Nicht identifizierbar";
    }
  }

  if (wbem_object_enum) {
    wbem_object_enum->Release();
  }

  if (wbem_object) {
    wbem_object->Release();
  }

  VariantClear(&value);
}

void System::set_processor() {
  /**
    Create a single uninitialized object of the class on the local system
  */
  IWbemLocator *wbem_locator;
  auto result_create_instance = CoCreateInstance(
      CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&wbem_locator));

  if (!SUCCEEDED(result_create_instance)) {
    _com_error err(result_create_instance);
    auto err_message = err.ErrorMessage();
    processor = err_message;
    wbem_locator->Release();
  }

  /**
    Create a connection through DCOM to a WMI namespace on the computer
    specified in the strNetworkResource parameter.
  */
  IWbemServices *wbem_service;
  auto result_connect_server = wbem_locator->ConnectServer(
      const_cast<BSTR>(L"root\\CIMV2"), NULL, NULL, NULL,
      WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &wbem_service);
  wbem_locator->Release();

  if (!SUCCEEDED(result_connect_server)) {
    _com_error err(result_connect_server);
    auto err_message = err.ErrorMessage();
    processor = err_message;
    wbem_service->Release();
  }

  /**
    Execute a query to retrieve objects from WMI
  */
  IEnumWbemClassObject *wbem_object_enum;
  auto result_exec_query = wbem_service->ExecQuery(
      const_cast<BSTR>(L"WQL"),
      const_cast<BSTR>(L"SELECT Name FROM Win32_Processor"),
      WBEM_FLAG_FORWARD_ONLY, NULL, &wbem_object_enum);
  wbem_service->Release();

  if (!SUCCEEDED(result_exec_query)) {
    _com_error err(result_exec_query);
    auto err_message = err.ErrorMessage();
    processor = err_message;
    wbem_object_enum->Release();
  }

  /**
    Iterate through retrieved objects.
  */
  IWbemClassObject *wbem_object;
  ULONG number_objects;
  VARIANT value;
  VariantInit(&value);

  while (wbem_object_enum->Next(WBEM_INFINITE, 1, &wbem_object,
                                &number_objects) != WBEM_S_FALSE) {
    auto result_wbem_object_get =
        wbem_object->Get(L"Name", 0, &value, NULL, NULL);

    if (SUCCEEDED(result_wbem_object_get) && value.vt == VT_BSTR) {
      processor = value.bstrVal;
    } else {
      processor = L"Nicht identifizierbar";
    }
  }

  if (wbem_object_enum) {
    wbem_object_enum->Release();
  }

  if (wbem_object) {
    wbem_object->Release();
  }

  VariantClear(&value);
}

void System::set_memory() {
  constexpr auto mebibyte = 1024 * 1024;
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  auto result_get_ram = GlobalMemoryStatusEx(&statex);

  if (result_get_ram != 0) {
    auto mpt = statex.ullTotalPhys / mebibyte;
    auto mpiu = mpt - (statex.ullAvailPhys / mebibyte);
    memory_total = std::to_wstring(mpt);
    memory_in_use = std::to_wstring(mpiu);
  } else {
    auto err = Helper::Format::error_message(GetLastError());
    memory_total = err;
    memory_in_use = err;
  }
}

void System::print_system_info(std::wostream &stream) {
  auto result_prepare_wmi = prepare_wmi();

  if (!SUCCEEDED(result_prepare_wmi)) {
    _com_error err(result_prepare_wmi);
    auto err_message = err.ErrorMessage();

    stream << Helper::Menu::get_separator_thick() << L"\n\n"
           << L" Systeminformationen\n"
           << Helper::Menu::get_separator_thin() << L"\n\n"
           << Helper::Format::name_and_value(L"Fehler", err_message) << L"\n\n";
    return;
  }

  set_username();
  set_workstation();
  set_default_printer();
  set_offline_files();
  set_operating_system();
  set_processor();
  set_memory();

  stream << Helper::Menu::get_separator_thick() << L"\n\n"
         << L" Systeminformationen\n"
         << Helper::Menu::get_separator_thin() << L"\n\n"
         << Helper::Format::name_and_value(L"Username", user_name) << L"\n"
         << Helper::Format::name_and_value(L"Computername", machine_name)
         << L"\n"
         << Helper::Format::name_and_value(L"Domain", domain) << L"\n\n"
         << Helper::Format::name_and_value(L"Standarddrucker", default_printer)
         << L"\n"
         << Helper::Format::name_and_value(L"Offlinedateien", offline_files)
         << L"\n\n"
         << Helper::Format::name_and_value(L"Betriebssystem", os_name) << L"\n"
         << Helper::Format::name_and_value(L"Version", os_version) << L"\n"
         << Helper::Format::name_and_value(L"Architektur", os_architecture)
         << L"\n\n"
         << Helper::Format::name_and_value(L"Prozessor", processor) << L"\n"
         << Helper::Format::name_and_value(
                L"RAM Physical", (memory_in_use + L"/" + memory_total))
         << L" MB\n\n";
}
}  // namespace WinApi