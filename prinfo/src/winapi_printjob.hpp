#pragma once

#include "winapi_printer.hpp"

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <winspool.h>

namespace winapi {

    class Printer;

    class Printjob {
    public:
        friend std::wostream& operator<<(std::wostream& stream, const Printjob& job);

        Printjob(const _JOB_INFO_2W& job_info);
        ~Printjob();

        static int SetPrintjobs(Printer* const printer);

        const std::wstring& ID() const;
        const std::wstring& DocumentName() const;
        const std::wstring& UserName() const;
        const std::wstring& MachineName() const;
        const std::wstring& Submitted() const;
        const std::wstring& DataType() const;
        const std::wstring& Size() const;
        const std::wstring& Status() const;
        const std::wstring& PageCount() const;

    private:
        static void setJobsInfoList(Printer* const printer, std::unique_ptr<_JOB_INFO_2W[]>& out_jobs_info_list);

        _JOB_INFO_2W m_job_info;
        std::wstring m_id;
        std::wstring m_document_name;
        std::wstring m_user_name;
        std::wstring m_machine_name;
        std::wstring m_submitted;
        std::wstring m_datatype;
        std::wstring m_size;
        std::wstring m_status;
        std::wstring m_page_count;

        void init();
        void setID();
        void setDocumentName();
        void setUserName();
        void setMachineName();
        void setSubmitted();
        void setDataType();
        void setSize();
        void setStatus();
        void setPageCount();
    };
}