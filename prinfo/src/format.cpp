#include "format.hpp"

#include <math.h>
#include <string>
#include <iomanip>

// Static member
Format::data_units Format::m_data_units;

std::wstring Format::NameValuePair(const std::wstring& name, const std::wstring& value, const wchar_t replace_empty_with) {
    constexpr std::size_t max_length = 50;
    std::wstring name_ = name;
    std::wstring value_ = value;

    // Format name if too long
    if (name_.length() > max_length) {
        name_ = Format::longName(name_, max_length);
    }

    // Format value if too long
    if (value_.length() > max_length) {
        value_ = Format::longValue(value_, max_length);
    }

    // Replace value with char if empty
    if (value_ == L"" || value_ == L" ") {
        value_ = replace_empty_with;
    }

    // Calculate space
    unsigned int num_spaces = 0;
    std::size_t name_len = name.length();
    if (name_len > max_length) {
        num_spaces = max_length - (name_len % max_length);
    }
    else {
        num_spaces = max_length - name_len;
    }

    // Set final string together
    std::wstring name_space = std::wstring(num_spaces + 2, L' ');
    return L" " + name_ + name_space + value_;
}

std::wstring Format::longName(const std::wstring& name, const std::size_t max_line_length) {
    std::vector<std::wstring> lines = lineWrap(name, max_line_length);
    std::wstring name_ = L"";
    unsigned int idx = 0;

    for (const std::wstring& line : lines) {
        if (idx == lines.size() - 1) {
            name_ += line;
            break;
        }
        name_ += (line + L"\n ");
        idx++;
    }
    return name_;
}

std::wstring Format::longValue(const std::wstring& value, const std::size_t max_line_length) {
    std::vector<std::wstring> lines = lineWrap(value, max_line_length);
    std::wstring value_ = L"";
    std::wstring white_space(max_line_length + 2, L' ');
    unsigned int idx = 0;

    for (const std::wstring& line : lines) {
        if (idx == lines.size() - 1) {
            value_ += line;
            break;
        }

        value_ += (line + L"\n " + white_space);
        idx++;
    }
    return value_;
}

std::vector<std::wstring> Format::lineWrap(const std::wstring& line, const std::size_t max_line_length) {
    if (line.length() < 1) {
        return {};
    }

    std::vector<std::wstring> lines;
    std::size_t num_of_lines = static_cast<std::size_t>(ceil(line.length() / max_line_length));
    lines.reserve(num_of_lines);
    std::wstring line_ = line;

    while (line_.length() > max_line_length) {
        lines.emplace_back(line_.substr(0, max_line_length));
        line_ = line_.substr(max_line_length, line_.length());
    }

    lines.emplace_back(line_.substr(0, max_line_length));
    return lines;
}

std::wstring Format::MultiSZKey(TCHAR multi_sz_data[]) {
    std::wstring multi_sz_string = L"";
    size_t multi_sz_string_length = wcslen(multi_sz_data);
    unsigned idx = 0;

    while (multi_sz_string_length > 0) {
        multi_sz_string += &multi_sz_data[idx];
        multi_sz_string += L" ";
        idx += multi_sz_string_length + 1;  // '\0' -> + 1
        multi_sz_string_length = wcslen(&multi_sz_data[idx]);
    }

    return multi_sz_string;
}

// Size in bytes
std::wstring Format::ConvertDataUnit(const uintmax_t size) {
    std::wstring data_unit = L"B";
    long double d_size = static_cast<long double>(size);
    long double converted_size = 0.0;

    // Petabyte
    if (size > m_data_units.PB) {
        converted_size = ceil(d_size / m_data_units.PB);
        data_unit = L"PB";
    }
    // Terrabyte
    else if (size > m_data_units.TB) {
        converted_size = ceil(d_size / m_data_units.TB);
        data_unit = L"TB";
    }
    // Gigabyte
    else if (size > m_data_units.GB) {
        converted_size = ceil(d_size / m_data_units.GB);
        data_unit = L"GB";
    }
    // Megabyte
    else if (size > m_data_units.MB) {
        converted_size = ceil(d_size / m_data_units.MB);
        data_unit = L"MB";
    }
    // Kilobyte
    else if (size > m_data_units.KB) {
        converted_size = ceil(d_size / m_data_units.KB);
        data_unit = L"KB";
    }

    return std::to_wstring(static_cast<uintmax_t>(converted_size)) + L" " + data_unit;
}

std::wstring Format::ErrorMessage(DWORD error_message_id) {
    if (error_message_id == 0) {
        return L"";
    }

    /**
      Format error-message-id to wstring.
    */
    LPWSTR error_message_buffer = nullptr;
    const DWORD size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error_message_id, LOCALE_USER_DEFAULT,
        reinterpret_cast<LPWSTR>(&error_message_buffer), 0, NULL);

    std::wstring error_message(error_message_buffer, size);
    if (error_message_buffer) {
        LocalFree(error_message_buffer);
    }
    return error_message;
}

void Format::HandleException(std::exception_ptr exception_pointer) {
    try {
        if (exception_pointer) {
            std::rethrow_exception(exception_pointer);
        }
    }
    catch (const std::exception& exc) {
        std::wcout << L" Ein unerwarteter Fehler ist aufgetreten: \"" << exc.what()
            << L"\"\n\n"
            << L" Beliebige Taste drücken...";
        std::wcin.get();
    }
}

bool Format::EndsWith(const std::wstring& mainStr, const std::wstring& toMatch)
{
    if (mainStr.size() >= toMatch.size() &&
        mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
        return true;
    else
        return false;
}