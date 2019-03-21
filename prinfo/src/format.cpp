#include "format.hpp"

#include <math.h>
#include <string>

namespace Helper {
    // Static member
    data_units Format::m_data_units;

    std::wstring Format::name_and_value(std::wstring name, std::wstring value) {
        const int max_length = 50;

        if (value.length() < max_length) {
            const std::wstring space(max_length - name.length() - 2, L' ');
            return (L" " + name + L":" + space + value);
        }

        std::vector<std::wstring> lines = line_wrap(value, max_length);
        std::wstring formatted_value = L"";
        std::wstring space(max_length, L' ');
        unsigned index = 0;

        for (const std::wstring& line : lines) {
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

    std::vector<std::wstring> Format::line_wrap(std::wstring line,
        const unsigned max_line_length) {
        std::vector<std::wstring> lines;

        if (line.length() < 1) {
            return lines;
        }

        while (line.length() > max_line_length) {
            lines.push_back(line.substr(0, max_line_length));
            line = line.substr(max_line_length, line.length());
        }

        lines.push_back(line.substr(0, max_line_length));

        return lines;
    }

    std::wstring Format::multi_sz_key(TCHAR multi_sz_data[]) {
        std::wstring multi_sz_string = L"";
        size_t multi_sz_string_length = wcslen(multi_sz_data);
        unsigned index = 0;

        while (multi_sz_string_length > 0) {
            multi_sz_string += &multi_sz_data[index];
            multi_sz_string += L" ";
            index += multi_sz_string_length + 1;  // '\0' -> + 1
            multi_sz_string_length = wcslen(&multi_sz_data[index]);
        }

        return multi_sz_string;
    }

    // Size in bytes
    std::wstring Format::data_unit_conversion(const long double size) {
        // String that specifies data unit 
        std::wstring data_unit = L"B";
        unsigned converted_size = 0.0;

        // Bigger than 1 TB
        if (size > m_data_units.TB) {
            converted_size = static_cast<unsigned>(size / m_data_units.TB);
            data_unit = L"TB";
        }
        // Bigger than 1 GB
        else if (size > m_data_units.GB) {
            converted_size = static_cast<unsigned>(size / m_data_units.GB);
            data_unit = L"GB";
        }
        // Bigger than 1 MB
        else if (size > m_data_units.MB) {
            converted_size = static_cast<unsigned>(size / m_data_units.MB);
            data_unit = L"MB";
        }
        // Bigger than 1 KB
        else if (size > m_data_units.KB) {
            converted_size = static_cast<unsigned>(size / m_data_units.KB);
            data_unit = L"KB";
        }

        return std::to_wstring(converted_size) + L" " + data_unit;
    }

    std::wstring Format::error_message(DWORD error_message_id) {
        /**
          Check if is error.
        */
        if (error_message_id == 0) {
            return std::wstring();
        }

        /**
          Format error-message-id to wstring.
        */
        LPWSTR error_message_buffer = nullptr;
        const DWORD size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error_message_id, LANG_USER_DEFAULT,
            reinterpret_cast<LPWSTR>(&error_message_buffer), 0, NULL);

        std::wstring error_message(error_message_buffer, size);
        LocalFree(error_message_buffer);
        return error_message;
    }

    void Format::handle_exception(std::exception_ptr exception_pointer) {
        try {
            if (exception_pointer) {
                std::rethrow_exception(exception_pointer);
            }
        }
        catch (const std::exception &exc) {
            std::wcout << L" Ein unerwarteter Fehler ist aufgetreten: \"" << exc.what()
                << L"\"\n\n"
                << L" Beliebige Taste drücken...";
            _getwch();
        }
    }
}  // namespace Helper