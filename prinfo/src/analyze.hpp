#pragma once

#include <filesystem>
#include <sstream>

namespace analyze {

    class PrintersFolder {
        // A class to analyze the C:\Windows\System32\spool\PRINTERS folder

    public:

        friend std::wostream& operator<<(std::wostream& stream, const PrintersFolder& p_folder);

        PrintersFolder();

        const std::size_t NumOfFiles() const;

    private:

        std::size_t m_num_of_files;

        static constexpr wchar_t path[] = L"C:\\Windows\\System32\\spool\\PRINTERS";

        // Count the number of files in the folder
        std::size_t countFiles(std::filesystem::path path);
    };
}