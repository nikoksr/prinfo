#pragma once

#include <string>

namespace Helper {
    class Snippets {
    public:
        /**
          Returns the default console screen header.
          Includes Copyright, Edit date, Creator, Contact email.
        */
        static std::wstring program_head();

        /**
          Returns a help text.
        */
        static std::wstring help();
    };  // class Information
}  // namespace Helper