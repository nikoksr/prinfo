#include "analyze.hpp"
#include "snippets.hpp"
#include "format.hpp"

#include <locale>
#include <codecvt>

#include <fstream>
#include <iostream>
#include <filesystem>

namespace analyze {

    // File
    File::File(std::filesystem::path path, uintmax_t size, std::filesystem::file_time_type last_write_time) :
        m_path(path),
        m_size(size),
        m_last_write_time(last_write_time) {}

    std::wostream& operator<<(std::wostream& stream, const File& file) {
        stream << file.Path();
        return stream;
    }

    const std::filesystem::path File::Path() const { return m_path; }
    const uintmax_t File::Size() const { return m_size; }
    const std::filesystem::file_time_type File::LastWriteTime() const { return m_last_write_time; }

    // PrintersFolder
    PrintersFolder::PrintersFolder() {
        getFiles(path);
    }

    const std::size_t PrintersFolder::NumSpoolFiles() const { return m_num_spool_files; }

    std::wostream& operator<<(std::wostream& stream, const PrintersFolder& p_folder) {
        stream << snippets::k_separator_thick << L"\n\n"
            << L" Analyse " << p_folder.path << "\n"
            << snippets::k_separator_thin << L"\n\n";

        stream << Format::NameValuePair(L"Dateien Gesamt", std::to_wstring(p_folder.m_total_num_files)) << L"\n"
            << Format::NameValuePair(L"Spool Dateien", std::to_wstring(p_folder.m_num_spool_files)) << L"\n"
            << Format::NameValuePair(L"Junk Dateien", std::to_wstring(p_folder.m_num_junk_files)) << L"\n\n";
        return stream;
    }

    std::size_t PrintersFolder::countFiles(std::filesystem::path path) {
        using std::filesystem::directory_iterator;
        using fp = bool(*)(const std::filesystem::path&);
        return static_cast<std::size_t>(std::count_if(directory_iterator(path), directory_iterator{}, static_cast<fp>(std::filesystem::is_regular_file)));
    }

    void PrintersFolder::getFiles(std::filesystem::path path) {
        m_spool_files.clear();
        m_total_num_files = countFiles(path);

        if (m_total_num_files < 1) {
            return;
        }

        m_spool_files.reserve(m_total_num_files);

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            // Spooler file
            if (Format::EndsWith(entry.path(), L".SHD")
                || Format::EndsWith(entry.path(), L".SPL")) {
                m_spool_files.push_back(std::make_unique<File>(entry.path(), entry.file_size(), entry.last_write_time()));
                m_num_spool_files += 1;
                continue;
            }

            // Junk file
            m_num_junk_files += 1;
        }
    }
}