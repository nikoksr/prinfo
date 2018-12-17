#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>

// Text formatieren
std::wstring format_name_and_value(std::wstring name, std::wstring value);
std::vector<std::wstring> line_wrap(std::wstring value,
                                    const unsigned max_line_length);
std::wstring format_multi_sz_key(HKEY hkey, std::wstring name,
                                 std::wstring subkey, std::wstring value);
std::wstring format_size_of_job(DWORD job_size);
std::wstring general_error_message(DWORD error_message_id);
void handle_exception(std::exception_ptr exception_pointer);