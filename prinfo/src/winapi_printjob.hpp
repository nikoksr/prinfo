#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>

namespace WinApi {
    class Printjob
    {
    public:
        Printjob(const JOB_INFO_2& job_info);
        ~Printjob();

        static std::vector<std::unique_ptr<Printjob>>* load_jobs(const _PRINTER_INFO_2W& printer_info);
        static unsigned get_job_count();

        const std::wstring& get_document_name() const;
        const std::wstring& get_user_name() const;
        const std::wstring& get_machine_name() const;
        const std::wstring& get_submitted() const;
        const std::wstring& get_datatype() const;
        const std::wstring& get_size() const;
        const std::wstring& get_status() const;
        const std::wstring& get_page_count() const;

        friend std::wostream& operator<<(std::wostream& stream, const WinApi::Printjob& job);

    private:
        static unsigned m_obj_counter;
        static DWORD m_job_count;
        static JOB_INFO_2* m_jobs_info_list;
        static std::vector<std::unique_ptr<Printjob>> m_job_list;

        JOB_INFO_2 m_job_info;
        std::wstring m_document_name;
        std::wstring m_user_name;
        std::wstring m_machine_name;
        std::wstring m_submitted;
        std::wstring m_datatype;
        std::wstring m_size;
        std::wstring m_status;
        std::wstring m_page_count;

        static BOOL set_jobs_info_list(const _PRINTER_INFO_2W& printer_info);

        void set_document_name();
        void set_user_name();
        void set_machine_name();
        void set_submitted();
        void set_datatype();
        void set_size();
        void set_status();
        void set_page_count();
        void init();
    };
}