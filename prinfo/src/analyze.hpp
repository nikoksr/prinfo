#pragma once

#include <filesystem>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>

namespace analyze {

    class File {
        // File represents a file in the filesystem. Mostly used to hold critical data of files.

    public:
        File(std::filesystem::path path, uintmax_t size, std::filesystem::file_time_type last_write_time);

        friend std::wostream& operator<<(std::wostream& stream, const File& file);

        const std::filesystem::path Path() const;
        const uintmax_t Size() const;
        const std::filesystem::file_time_type LastWriteTime() const;

    private:

        std::filesystem::path m_path;
        uintmax_t m_size;
        std::filesystem::file_time_type m_last_write_time;
    };

    class PrintersFolder {
        // A class to analyze the C:\Windows\System32\spool\PRINTERS folder

    public:

        friend std::wostream& operator<<(std::wostream& stream, const PrintersFolder& p_folder);

        PrintersFolder();

        const std::size_t NumSpoolFiles() const;

    private:
        std::size_t m_total_num_files;
        std::size_t m_num_spool_files;
        std::size_t m_num_junk_files;
        std::vector<std::unique_ptr<File>> m_spool_files;

        static constexpr wchar_t path[] = L"C:\\Windows\\System32\\spool\\PRINTERS";

        // Count the number of files in the folder
        std::size_t countFiles(std::filesystem::path path);
        void getFiles(std::filesystem::path path);
    };
}