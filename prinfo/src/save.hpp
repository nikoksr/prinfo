#pragma once

#include "display.hpp"

#include <sstream>

namespace data {
    class Save {
    private:
        typedef  void(*displayFn)(std::wostream& wos);
    public:
        static void ToFile(displayFn df);
    };
}