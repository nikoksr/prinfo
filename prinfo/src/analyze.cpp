#include "analyze.hpp"
#include "snippets.hpp"
#include "format.hpp"

#include <locale>
#include <codecvt>

#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <regex>

namespace analyze {

    // File
    File::File(
        std::filesystem::path path,
        std::filesystem::path name,
        std::filesystem::path stem,
        std::filesystem::path extension,
        uintmax_t size,
        std::filesystem::file_time_type last_write_time
    ) :
        m_path(path),
        m_name(name),
        m_stem(stem),
        m_extension(extension),
        m_size(size),
        m_last_write_time(last_write_time) {}

    std::wostream& operator<<(std::wostream& stream, const File& file) {
        stream << file.path();
        return stream;
    }

    const std::wstring File::spoolID() const { return m_spool_id; }
    const std::filesystem::path File::path() const { return m_path; }
    const std::filesystem::path File::name() const { return m_name; }
    const std::filesystem::path File::stem() const { return m_stem; }
    const std::filesystem::path File::extension() const { return m_extension; }
    const uintmax_t File::size() const { return m_size; }
    const std::filesystem::file_time_type File::lastWriteTime() const { return m_last_write_time; }

    bool File::hasComplementingExtension(std::filesystem::path extension) {
        if ((extension == L".SHD" && m_extension == L".SPL")
            || (extension == L".SPL" && m_extension == L".SHD")) {
            return true;
        }
        return false;
    }

    // PrintersFolder
    PrintersFolder::PrintersFolder() {
        setFiles(path);
        setSingleSpoolerFiles();
    }

    const std::size_t PrintersFolder::NumSpoolFiles() const {
        return m_spool_files.size();
    }

    std::wostream& operator<<(std::wostream& stream, const PrintersFolder& p_folder) {
        stream << snippets::k_separator_thin << L"\n\n" << L" Analyse " << p_folder.path
            << "\n" << snippets::k_separator_thin << L"\n\n";

        stream << Format::NameValuePair(L"Dateien Gesamt", std::to_wstring(p_folder.m_total_num_files)) << L"\n"
            << Format::NameValuePair(L"Spool Dateien", std::to_wstring(p_folder.m_spool_files.size())) << L"\n"
            << Format::NameValuePair(L"Junk Dateien", std::to_wstring(p_folder.m_num_junk_files)) << L"\n"
            << Format::NameValuePair(L"Verzeichnisse", std::to_wstring(p_folder.m_num_folders)) << L"\n"
            << snippets::k_separator_thin << L"\n\n"
            << L" Leere Dateien: " << std::to_wstring(p_folder.m_damaged_spool_files.size()) << L"\n\n";

        for (auto& file : p_folder.m_damaged_spool_files) {
            stream << L" -" << Format::NameValuePair(file->name(), L"", L' ') << L"\n";
        }

        // Unique files
        stream << snippets::k_separator_thin << L"\n\n Solo Spool Dateien: " << std::to_wstring(p_folder.m_single_spool_files.size()) << L"\n\n";

        for (auto& file : p_folder.m_single_spool_files) {
            stream << L" -" << Format::NameValuePair(file->name(), L"", L' ') << L"\n";
        }

        stream << L"\n";
        return stream;
    }

    std::size_t PrintersFolder::countFiles(std::filesystem::path path) {
        using std::filesystem::directory_iterator;
        using fp = bool(*)(const std::filesystem::path&);
        return static_cast<std::size_t>(std::count_if(directory_iterator(path), directory_iterator{}, static_cast<fp>(std::filesystem::is_regular_file)));
    }

    void PrintersFolder::setFiles(std::filesystem::path path) {
        m_spool_files.clear();
        m_damaged_spool_files.clear();
        m_total_num_files = countFiles(path);
        m_num_junk_files = 0;
        m_num_folders = 0;

        if (m_total_num_files < 1) {
            return;
        }

        m_spool_files.reserve(m_total_num_files);
        m_damaged_spool_files.reserve(m_total_num_files);

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            // Spooler file
            if (Format::EndsWith(entry.path(), L".SHD")
                || Format::EndsWith(entry.path(), L".SPL")) {

                m_spool_files.push_back(std::make_shared<File>(
                    entry.path(),
                    entry.path().filename(),
                    entry.path().stem(),
                    entry.path().extension(),
                    entry.file_size(),
                    entry.last_write_time()));

                if (entry.file_size() == 0) {
                    m_damaged_spool_files.push_back(m_spool_files.back());
                }
                continue;
            }

            // Junk file
            if (entry.is_regular_file()) {
                m_num_junk_files += 1;
                continue;
            }

            m_num_folders += 1;
        }
    }

    void PrintersFolder::setSingleSpoolerFiles() {
        m_single_spool_files.clear();
        m_single_spool_files.reserve(m_spool_files.size());

        // m_spool_files is expected to be alphabetically ordered
        for (const auto& file : m_spool_files) {
            // No files to compare with
            if (m_single_spool_files.size() == 0) {
                m_single_spool_files.emplace_back(file);
                continue;
            }

            const auto prev = m_single_spool_files.back();
            if (prev->stem() == file->stem() && file->hasComplementingExtension(prev->extension())) {
                // Found partner file
                m_single_spool_files.pop_back();
                continue;
            }
            // No partner file found
            m_single_spool_files.emplace_back(file);
        }
    }

    std::wstring PrintersFolder::getSpoolerFileID(const std::wstring& file_name) {
        std::wregex reg_id(L"(\\d{5,})");
        std::wsmatch matches;

        if (!std::regex_search(file_name, matches, reg_id)) {
            return L"";
        }

        std::wstring id = L"";

        for (auto match : matches) {
            id = match;
        }

        return id;
    }
}