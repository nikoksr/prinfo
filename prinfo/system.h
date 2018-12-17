#pragma once

#include <iostream>
#include <string>
//#include <winerror.h>
#include <Windows.h>

// System Infos
void show_system_info(std::wostream &stream = std::wcout);
HRESULT prepare_wmi();
BOOL get_username(std::wstring *username);
DWORD get_workstation(std::wstring *computer_name, std::wstring *domain);
DWORD get_offline_files(std::wstring *offline_files);
BOOL get_default_printer(std::wstring *default_printer);
HRESULT get_operating_system(std::wstring *operating_system,
                             std::wstring *version, std::wstring *architecture);
HRESULT get_processor(std::wstring *processor);
BOOL get_memory(std::wstring *mem_physical_total,
                std::wstring *mem_physical_in_use);