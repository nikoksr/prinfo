#pragma once

#include <string>
#include <sstream>
#include <memory>

namespace menu {
    class Functions;

    class Main {
    public:
        Main();
        virtual wchar_t Show();
        const bool isAdmin() const;
    private:
        bool m_is_admin;
        std::unique_ptr<Functions> m_func_menu;
    };

    class Functions : Main {
    public:
        wchar_t Show();
    };

    class Navigation {
    public:
        /**
          Submenus for navigating back, quitting and saving data.
        */
        static wchar_t Quit();
        static wchar_t BackQuit();
        static wchar_t SaveBackQuit();
        static wchar_t SaveExportBackQuit();
        static wchar_t YesNo();
    };
}
