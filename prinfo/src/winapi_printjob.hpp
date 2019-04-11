#pragma once

#include "winapi_printer.hpp"

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <winspool.h>

namespace WinApi {

    class Printer;

    class Printjob {
    public:
        friend std::wostream& operator<<(std::wostream& stream, const Printjob& job);

        Printjob(const _JOB_INFO_2W& job_info);
        ~Printjob();

        static int set_printjobs(Printer* const printer);

        const std::wstring& get_document_name() const;
        const std::wstring& get_user_name() const;
        const std::wstring& get_machine_name() const;
        const std::wstring& get_submitted() const;
        const std::wstring& get_datatype() const;
        const std::wstring& get_size() const;
        const std::wstring& get_status() const;
        const std::wstring& get_page_count() const;

    private:
        static void set_jobs_info_list(Printer* const printer, std::unique_ptr<_JOB_INFO_2W[]>& out_jobs_info_list);

        _JOB_INFO_2W m_job_info;
        std::wstring m_document_name;
        std::wstring m_user_name;
        std::wstring m_machine_name;
        std::wstring m_submitted;
        std::wstring m_datatype;
        std::wstring m_size;
        std::wstring m_status;
        std::wstring m_page_count;

        void init();
        void set_document_name();
        void set_user_name();
        void set_machine_name();
        void set_submitted();
        void set_datatype();
        void set_size();
        void set_status();
        void set_page_count();
    };
}