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
        File(
            std::filesystem::path path,
            std::filesystem::path name,
            std::filesystem::path stem,
            std::filesystem::path extension,
            uintmax_t size,
            std::filesystem::file_time_type last_write_time
        );

        friend std::wostream& operator<<(std::wostream& stream, const File& file);

        bool hasComplementingExtension(std::filesystem::path extension);

        const std::wstring spoolID() const;
        const std::filesystem::path path() const;
        const std::filesystem::path name() const;
        const std::filesystem::path stem() const;
        const std::filesystem::path extension() const;
        const uintmax_t size() const;
        const std::filesystem::file_time_type lastWriteTime() const;

    private:

        std::wstring m_spool_id;
        std::filesystem::path m_path;
        std::filesystem::path m_name;
        std::filesystem::path m_stem;
        std::filesystem::path m_extension;
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
        std::size_t m_num_junk_files;
        std::size_t m_num_folders;
        std::vector<std::shared_ptr<File>> m_spool_files;
        std::vector<std::shared_ptr<File>> m_damaged_spool_files;
        std::vector<std::shared_ptr<File>> m_single_spool_files;

        static constexpr wchar_t path[] = L"C:\\Windows\\System32\\spool\\PRINTERS";

        // Count the number of files in the folder
        std::size_t countFiles(std::filesystem::path path);
        void setFiles(std::filesystem::path path);
        void setSingleSpoolerFiles();

        static std::wstring getSpoolerFileID(const std::wstring& file_name);
    };
}