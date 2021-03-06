﻿#include "system.hpp"
#include "format.hpp"
#include "snippets.hpp"

#include <LM.h>
#include <WbemCli.h>
#include <Windows.h>
#include <comdef.h>
#include <cscapi.h>
#include <iostream>
#include <sstream>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "CscApi.lib")

namespace winapi {
    bool System::isWMIPrepared = false;

    System::System() : m_system_name(L"unnamed") { init(); }
    System::System(const std::wstring& system_name) : m_system_name(system_name) { init(); }

    HRESULT System::prepareWMI() {
        // Check if wmi is already prepared
        if (isWMIPrepared) {
            return ERROR_SUCCESS;
        }

        /**
          Initialize the COM library.
        */
        HRESULT result_com_init = CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);

        if (!SUCCEEDED(result_com_init)) {
            return result_com_init;
        }

        /**
          Register security and set the default security values.
        */
        HRESULT result_init_security = CoInitializeSecurity(
            nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_CONNECT,
            RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);

        isWMIPrepared = true;
        return result_init_security;
    }

    void System::setUsername() {
        DWORD username_buffer_size = 255;
        wchar_t temp_username[255];

        BOOL result_username = GetUserNameW(temp_username, &username_buffer_size);

        if (result_username == 0) {
            m_user_name = Format::ErrorMessage(GetLastError());
        }
        else {
            m_user_name = std::wstring(temp_username);
        }
    }

    void System::setWorkstation() {
        PWKSTA_INFO_102 wksta_info;
        DWORD result_wksta =
            NetWkstaGetInfo(nullptr, 102, reinterpret_cast<LPBYTE *>(&wksta_info));

        if (result_wksta != NERR_Success) {
            std::wstring err = Format::ErrorMessage(result_wksta);
            m_machine_name = err;
            m_domain = err;
        }
        else {
            m_machine_name = std::wstring(wksta_info->wki102_computername);
            m_domain = std::wstring(wksta_info->wki102_langroup);
        }

        NetApiBufferFree(wksta_info);
    }

    void System::setDefaultPrinter() {
        DWORD default_printer_buffer_size = 255;
        wchar_t temp_default_printer[255];

        BOOL result_default_printer =
            GetDefaultPrinterW(temp_default_printer, &default_printer_buffer_size);

        if (result_default_printer == 0) {
            m_default_printer = Format::ErrorMessage(GetLastError());
        }
        else {
            m_default_printer = std::wstring(temp_default_printer);
        }
    }

    void System::setOfflineFiles() {
        BOOL offline_files_active;
        DWORD result_offline_files =
            OfflineFilesQueryStatus(&offline_files_active, nullptr);

        if (result_offline_files != ERROR_SUCCESS) {
            m_offline_files = Format::ErrorMessage(result_offline_files);
        }
        else {
            switch (offline_files_active) {
            case TRUE:
                m_offline_files = L"Aktiviert";
                break;
            case FALSE:
                m_offline_files = L"Offlinedateien oder CSC-Treiber deaktiviert";
                break;
            default:
                m_offline_files = L"Status unbekannt";
                break;
            }
        }
    }

    void System::setOperatingSystem() {
        /**
          Create a single uninitialized object of the class on the local system.
        */
        IWbemLocator *wbem_locator;
        HRESULT result_create_instance = CoCreateInstance(
            CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&wbem_locator));

        if (!SUCCEEDED(result_create_instance)) {
            _com_error err(result_create_instance);
            const TCHAR* err_message = err.ErrorMessage();
            m_os_name = err_message;
            m_os_version = err_message;
            m_os_architecture = err_message;
            wbem_locator->Release();
        }

        /**
          Create a connection through DCOM to a WMI namespace on the computer
          specified in the strNetworkResource parameter.
        */
        IWbemServices *wbem_service;
        HRESULT result_connect_server = wbem_locator->ConnectServer(
            const_cast<BSTR>(L"root\\CIMV2"), NULL, NULL, NULL,
            WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &wbem_service);
        wbem_locator->Release();

        if (!SUCCEEDED(result_connect_server)) {
            _com_error err(result_connect_server);
            std::wstring err_message = err.ErrorMessage();
            m_os_name = err_message;
            m_os_version = err_message;
            m_os_architecture = err_message;
            wbem_service->Release();
        }

        /**
          Execute a query to retrieve objects from WMI
        */
        IEnumWbemClassObject *wbem_object_enum;
        HRESULT result_exec_query = wbem_service->ExecQuery(
            const_cast<BSTR>(L"WQL"),
            const_cast<BSTR>(
                L"SELECT Name,Version,OSArchitecture FROM Win32_OperatingSystem"),
            WBEM_FLAG_FORWARD_ONLY, NULL, &wbem_object_enum);
        wbem_service->Release();

        if (!SUCCEEDED(result_exec_query)) {
            _com_error err(result_exec_query);
            const TCHAR* err_message = err.ErrorMessage();
            m_os_name = err_message;
            m_os_version = err_message;
            m_os_architecture = err_message;
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
            HRESULT result_get_name = wbem_object->Get(L"Name", 0, &value, NULL, NULL);

            if (SUCCEEDED(result_get_name) && value.vt == VT_BSTR) {
                m_os_name = value.bstrVal;
            }
            else {
                m_os_name = L"Nicht identifizierbar";
            }

            HRESULT result_get_version =
                wbem_object->Get(L"Version", 0, &value, NULL, NULL);

            if (SUCCEEDED(result_get_version) && value.vt == VT_BSTR) {
                m_os_version = value.bstrVal;
            }
            else {
                m_os_version = L"Nicht identifizierbar";
            }

            HRESULT result_get_architecture =
                wbem_object->Get(L"OSArchitecture", 0, &value, NULL, NULL);

            if (SUCCEEDED(result_get_architecture) && value.vt == VT_BSTR) {
                m_os_architecture = value.bstrVal;
            }
            else {
                m_os_architecture = L"Nicht identifizierbar";
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

    void System::setProcessor() {
        /**
          Create a single uninitialized object of the class on the local system
        */
        IWbemLocator *wbem_locator;
        HRESULT result_create_instance = CoCreateInstance(
            CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&wbem_locator));

        if (!SUCCEEDED(result_create_instance)) {
            _com_error err(result_create_instance);
            const TCHAR* err_message = err.ErrorMessage();
            m_processor = err_message;
            wbem_locator->Release();
        }

        /**
          Create a connection through DCOM to a WMI namespace on the computer
          specified in the strNetworkResource parameter.
        */
        IWbemServices *wbem_service;
        HRESULT result_connect_server = wbem_locator->ConnectServer(
            const_cast<BSTR>(L"root\\CIMV2"), NULL, NULL, NULL,
            WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &wbem_service);
        wbem_locator->Release();

        if (!SUCCEEDED(result_connect_server)) {
            _com_error err(result_connect_server);
            const TCHAR* err_message = err.ErrorMessage();
            m_processor = err_message;
            wbem_service->Release();
        }

        /**
          Execute a query to retrieve objects from WMI
        */
        IEnumWbemClassObject *wbem_object_enum;
        HRESULT result_exec_query = wbem_service->ExecQuery(
            const_cast<BSTR>(L"WQL"),
            const_cast<BSTR>(L"SELECT Name FROM Win32_Processor"),
            WBEM_FLAG_FORWARD_ONLY, NULL, &wbem_object_enum);
        wbem_service->Release();

        if (!SUCCEEDED(result_exec_query)) {
            _com_error err(result_exec_query);
            const TCHAR* err_message = err.ErrorMessage();
            m_processor = err_message;
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
            HRESULT result_wbem_object_get =
                wbem_object->Get(L"Name", 0, &value, NULL, NULL);

            if (SUCCEEDED(result_wbem_object_get) && value.vt == VT_BSTR) {
                m_processor = value.bstrVal;
            }
            else {
                m_processor = L"Nicht identifizierbar";
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

    void System::setMemory() {
        constexpr unsigned mebibyte = 1024 * 1024;
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        BOOL result_get_ram = GlobalMemoryStatusEx(&statex);

        if (result_get_ram != 0) {
            unsigned long long mpt = statex.ullTotalPhys / mebibyte;
            unsigned long long mpiu = mpt - (statex.ullAvailPhys / mebibyte);
            m_memory_total = std::to_wstring(mpt);
            m_memory_in_use = std::to_wstring(mpiu);
        }
        else {
            std::wstring err = Format::ErrorMessage(GetLastError());
            m_memory_total = err;
            m_memory_in_use = err;
        }
    }

    void System::init() {
        if (prepareWMI() != ERROR_SUCCESS) {
            return;
        }

        setUsername();
        setWorkstation();
        setDefaultPrinter();
        setOfflineFiles();
        setOperatingSystem();
        setProcessor();
        setMemory();
    }

    void System::Refresh() {
        if (prepareWMI() != ERROR_SUCCESS) {
            return;
        }

        m_system_name = L"";
        m_user_name = L"";
        m_machine_name = L"";
        m_domain = L"";
        m_offline_files = L"";
        m_default_printer = L"";
        m_os_name = L"";
        m_os_version = L"";
        m_os_architecture = L"";
        m_processor = L"";
        m_memory_in_use = L"";
        m_memory_total = L"";

        init();
    }

    std::wostream& operator<<(std::wostream& stream, const System& system) {
        stream << snippets::k_separator_thick << L"\n\n"
            << L" Systeminformationen\n"
            << snippets::k_separator_thin << L"\n\n";
        HRESULT result_prepare_wmi = System::prepareWMI();

        if (!SUCCEEDED(result_prepare_wmi)) {
            _com_error err(result_prepare_wmi);
            const TCHAR* err_message = err.ErrorMessage();

            stream << Format::NameValuePair(L"Fehler", err_message) << L"\n\n";
            return stream;
        }

        stream << Format::NameValuePair(L"Username", system.m_user_name) << L"\n"
            << Format::NameValuePair(L"Computername", system.m_machine_name)
            << L"\n"
            << Format::NameValuePair(L"Domain", system.m_domain) << L"\n\n"
            << Format::NameValuePair(L"Standarddrucker", system.m_default_printer)
            << L"\n"
            << Format::NameValuePair(L"Offlinedateien", system.m_offline_files)
            << L"\n\n"
            << Format::NameValuePair(L"Betriebssystem", system.m_os_name) << L"\n"
            << Format::NameValuePair(L"Version", system.m_os_version) << L"\n"
            << Format::NameValuePair(L"Architektur", system.m_os_architecture)
            << L"\n\n"
            << Format::NameValuePair(L"Prozessor", system.m_processor) << L"\n"
            << Format::NameValuePair(L"RAM Physical", (system.m_memory_in_use + L"/" + system.m_memory_total))
            << L" MB\n\n";

        return stream;
    }

    /**
     Getter functions
    */
    const std::wstring& System::SystemName() const { return m_system_name; }
    const std::wstring& System::UserName() const { return m_user_name; }
    const std::wstring& System::WorkstationName() const { return m_machine_name; }
    const std::wstring& System::OfflineFiles() const { return m_offline_files; }
    const std::wstring& System::DefaultPrinter() const { return m_default_printer; }
    const std::wstring& System::OperatingSystem() const { return m_os_name; }
    const std::wstring& System::Processor() const { return m_processor; }
    const std::wstring& System::Memory() const { return m_memory_total; }
}  // namespace WinApi

