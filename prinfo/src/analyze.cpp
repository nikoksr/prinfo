#include "analyze.hpp"
#include "snippets.hpp"
#include "format.hpp"

namespace analyze {
    PrintersFolder::PrintersFolder() : m_num_of_files(countFiles(path)) {}

    const std::size_t PrintersFolder::NumOfFiles() const { return m_num_of_files; }

    std::size_t PrintersFolder::countFiles(std::filesystem::path path) {
        using std::filesystem::directory_iterator;
        using fp = bool(*)(const std::filesystem::path&);
        return std::count_if(directory_iterator(path), directory_iterator{}, (fp)std::filesystem::is_regular_file);
    }

    std::wostream& operator<<(std::wostream& stream, const PrintersFolder& p_folder) {
        stream << L" Analyse " << p_folder.path << "\n"
            << snippets::k_separator_thin << L"\n\n";

        stream << Format::NameValuePair(L"Dateien", std::to_wstring(p_folder.m_num_of_files)) << L"\n\n";
        return stream;
    }
}