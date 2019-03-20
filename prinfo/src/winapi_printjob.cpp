#include "winapi_printjob.hpp"

#include "format.hpp"

namespace WinApi {

    /**
     Constructor and Destructor
    */
    Printjob::Printjob(const JOB_INFO_2& job_info) : m_job_info(job_info) {
        obj_counter++;
        init();
    }

    Printjob::~Printjob() {
        if (obj_counter < 2 && jobs_info_list) {
            delete[] jobs_info_list;
        }

        obj_counter--;
    }

    /**
     Static members
    */
    unsigned Printjob::obj_counter = 0;
    DWORD Printjob::job_count = 0;
    JOB_INFO_2* (Printjob::jobs_info_list) = nullptr;
    std::vector<Printjob*> Printjob::job_list(0);

    /**
     Function definitions
    */
    std::vector<Printjob*>& Printjob::load_jobs(const _PRINTER_INFO_2W& printer_info) {
        if (set_jobs_info_list(printer_info) == FALSE) {
            return job_list;
        }

        for (DWORD i = 0; i < job_count; ++i) {
            job_list.push_back(new Printjob(jobs_info_list[i]));
        }

        return job_list;
    }

    BOOL Printjob::set_jobs_info_list(const _PRINTER_INFO_2W& printer_info) {
        // Drucker-Handle erstellen
        HANDLE printer_handle;

        if (OpenPrinterW(printer_info.pPrinterName, &printer_handle, NULL) == FALSE) {
            return FALSE;
        }

        // Buffer-Größe bestimmen
        DWORD needed_buffer;

        EnumJobsW(printer_handle, NULL, printer_info.cJobs, 2, NULL, NULL,
            &needed_buffer, &job_count);

        // Job_info Array anhand von Buffer erstellen
        const unsigned size = static_cast<unsigned>(static_cast<double>(needed_buffer) / sizeof(JOB_INFO_2));
        jobs_info_list = new JOB_INFO_2[size];

        return EnumJobsW(printer_handle, NULL, printer_info.cJobs, 2,
            reinterpret_cast<LPBYTE>(jobs_info_list),
            needed_buffer, &needed_buffer, &job_count);
    }

    void Printjob::display(std::wostream &stream) {
        using namespace Helper;

        stream << Format::name_and_value(L"Dokument: ", m_document_name)
            << Format::name_and_value(L"User: ", m_user_name)
            << Format::name_and_value(L"Maschine: ", m_machine_name)
            << Format::name_and_value(L"Zeit: ", m_submitted)
            << Format::name_and_value(L"Datentyp: ", m_datatype)
            << Format::name_and_value(L"Größe: ", m_size)
            << Format::name_and_value(L"Seitenanzahl: ", m_page_count)
            << Format::name_and_value(L"Status: ", m_status);
    }

    void Printjob::set_document_name() {
        m_document_name = std::wstring(m_job_info.pDocument);
    }

    void Printjob::set_user_name() {
        m_user_name = std::wstring(m_job_info.pUserName);
    }

    void Printjob::set_machine_name() {
        m_machine_name = std::wstring(m_job_info.pMachineName);
    }

    void Printjob::set_datatype() {
        m_datatype = std::wstring(m_job_info.pDatatype);
    }

    void Printjob::set_size() {
        m_size = std::wstring(Helper::Format::byte_conversion(m_job_info.Size));
    }

    void Printjob::set_page_count() {
        m_page_count = std::to_wstring(m_job_info.TotalPages);
    }

    void Printjob::set_submitted() {
        SYSTEMTIME submitted_time = m_job_info.Submitted;
        m_submitted = std::to_wstring(submitted_time.wHour) + L":" + std::to_wstring(submitted_time.wMinute);
    }

    void Printjob::set_status() {
        m_status = L"";

        if (m_job_info.Status & JOB_STATUS_BLOCKED_DEVQ) {
            m_status = L"The driver cannot print the job.";
        }
        if (m_job_info.Status & JOB_STATUS_DELETED) {
            m_status = L"Job has been deleted.";
        }
        if (m_job_info.Status & JOB_STATUS_DELETING) {
            m_status = L"Job is being deleted.";
        }
        if (m_job_info.Status & JOB_STATUS_ERROR) {
            m_status = L"An error is associated with the job.";
        }
        if (m_job_info.Status & JOB_STATUS_OFFLINE) {
            m_status = L"Printer is offline.";
        }
        if (m_job_info.Status & JOB_STATUS_PAPEROUT) {
            m_status = L"Printer is out of paper.";
        }
        if (m_job_info.Status & JOB_STATUS_PAUSED) {
            m_status += L"Job is paused.";
        }
        if (m_job_info.Status & JOB_STATUS_PRINTED) {
            m_status += L"Job has printed.";
        }
        if (m_job_info.Status & JOB_STATUS_PRINTING) {
            m_status += L"Job is printing.";
        }
        if (m_job_info.Status & JOB_STATUS_RESTART) {
            m_status += L"Job has been restarted.";
        }
        if (m_job_info.Status & JOB_STATUS_SPOOLING) {
            m_status += L"Job is spooling.";
        }
        if (m_job_info.Status & JOB_STATUS_USER_INTERVENTION) {
            m_status += L"Printer has an error that requires the user to do something.";
        }
        if (m_job_info.Status & JOB_STATUS_COMPLETE) {
            m_status += L"The job is sent to the printer, but may not be printed yet.";
        }
        if (m_job_info.Status & JOB_STATUS_RETAINED) {
            m_status +=
                L"The job has been retained in the print queue following printing.";
        }
    }

    void Printjob::init() {
        set_document_name();
        set_user_name();
        set_machine_name();
        set_submitted();
        set_datatype();
        set_size();
        set_status();
        set_page_count();
    }

    DWORD Printjob::get_job_count() {
        return job_count;
    }
}  // namespace WinApi