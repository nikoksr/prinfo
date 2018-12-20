#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>

namespace Helper {
class Format {
 public:
  /**
    Format name value pair so that the overall output stream
    looks very similar.

    @param name The name to be put in formatted string.
    @param value The value to be put in formatted string.
  */
  static std::wstring name_and_value(std::wstring name, std::wstring value);

  /**
    Multi_sz keys are a sequence of null-terminated strings terminated by an
    empty string. (e.g. String1\0String2\0String3\0\0)
    If we would simlpy print this string to console we would only see 'String1'.
    We have to replace the null-terminations ('\0') with blanks (" ").
  */
  static std::wstring multi_sz_key(HKEY hkey, std::wstring name,
                                   std::wstring subkey, std::wstring value);

  /**
    Converts bytes in more readable size units like KB, MB...

    @param job_size The number to be converted.
  */
  static std::wstring byte_conversion(double job_size);

  /**
    Formats a system error code to a equvilant readable error string.

    @param error_message_id The id of the system error.
  */
  static std::wstring error_message(DWORD error_message_id);

  /**      Handle an exception, print out the exception.

    @param exception_pointer Pointer to the previously occured exception.
  */
  static void handle_exception(std::exception_ptr exception_pointer);

 private:
  /**
    Splits a line in sub-lines all with a defined max length.
    Serves as formatting function for long strings. A long string will be
    displayed as block not one long line.

    @param line The line to split.
    @param max_line_length Defines the maximum allowed number of chars per line.
  */
  static std::vector<std::wstring> line_wrap(std::wstring line,
                                             const unsigned max_line_length);
};  // class Format
}  // namespace Helper