#include "format.hpp"

#include <math.h>
#include <string>

namespace Helper {
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

    std::wstring Format::byte_conversion(const double job_size) {
        if (job_size < 1024) {
            return std::to_wstring(job_size) + L" B";
        }
        if (job_size < pow(1024, 2)) {
            return std::to_wstring(job_size / 1024) + L" KB";
        }
        if (job_size < pow(1024, 3)) {
            return std::to_wstring(job_size / pow(1024, 2)) + L" MB";
        }
        if (job_size < pow(1024, 4)) {
            return std::to_wstring(job_size / pow(1024, 3)) + L" GB";
        }
        if (job_size < pow(1024, 5)) {
            return std::to_wstring(job_size / pow(1024, 4)) + L" TB";
        }

        return L"Zu groß";
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