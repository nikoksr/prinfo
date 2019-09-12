#pragma once

#include <filesystem>
#include <sstream>
#include <vector>
#include <memory>

namespace analyze {

    namespace fs = std::filesystem;

    class File {
        // File represents a file in the filesystem. Mostly used to hold critical data of files.

    public:

        File(fs::path path, uintmax_t size, fs::file_time_type last_write_time);

        friend std::wostream& operator<<(std::wostream& stream, const File& file);

        const fs::path Path() const;
        const uintmax_t Size() const;
        const fs::file_time_type LastWriteTime() const;

    private:

        fs::path m_path;
        uintmax_t m_size;
        fs::file_time_type m_last_write_time;
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
        std::size_t countFiles(fs::path path);
        void getFiles(fs::path path);
    };
}