#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>



class Format {
    typedef const struct _dataUnits {
        const uintmax_t KB = 1024;
        const uintmax_t MB = 1048576;
        const uintmax_t GB = 1073741824;
        const uintmax_t TB = 1099511627776;
        const uintmax_t PB = 1125899906842624;
    } data_units;

public:
    /**
      Format name value pair so that the overall output stream
      looks very similar.

      @param name The name to be put in formatted string.
      @param value The value to be put in formatted string.
    */
    static std::wstring NameValuePair(const std::wstring& name, const std::wstring& value, const wchar_t replace_empty_with = L'-');

    /**
      Multi_sz keys are a sequence of null-terminated strings terminated by an
      empty string. (e.g. String1\0String2\0String3\0\0)
      If we would simlpy print this string to console we would only see 'String1'.
      We have to replace the null-terminations ('\0') with blanks (" ").
    */
    static std::wstring MultiSZKey(TCHAR multi_sz_data[]);

    /**
      Converts bytes in more readable size units like KB, MB...

      @param job_size The number to be converted.
    */
    static std::wstring ConvertDataUnit(const uintmax_t size);

    /**
      Formats a system error code in a readable error string.

      @param error_message_id The id of the system error.
    */
    static std::wstring ErrorMessage(DWORD error_message_id);

    /**
      Handle an exception, print out the exception.

      @param exception_pointer Pointer to the previously occured exception.
    */
    static void HandleException(std::exception_ptr exception_pointer);

    static bool EndsWith(const std::wstring &mainStr, const std::wstring &toMatch);

private:
    /**
      Splits a line in sub-lines all with a defined max length.
      Serves as formatting function for long strings. A long string will be
      displayed as block not one long line.

      @param line The line to split.
      @param max_line_length Defines the maximum allowed number of chars per line.
    */
    static std::vector<std::wstring> lineWrap(const std::wstring& line, const std::size_t max_line_length);

    static data_units m_data_units;
};  // class Format
