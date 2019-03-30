#include "winapi_printjob.hpp"

#include "format.hpp"

namespace WinApi {

    /**
     Constructor and Destructor
    */
    Printjob::Printjob(const JOB_INFO_2& job_info) : m_job_info(job_info) {
        m_obj_counter++;
        init();
    }

    Printjob::~Printjob() {
        m_obj_counter--;

        if (m_obj_counter < 1 && m_jobs_info_list) {
            delete[] m_jobs_info_list;
        }
    }

    /**
     Static members
    */
    unsigned Printjob::m_obj_counter = 0;
    DWORD Printjob::m_job_count = -1;
    JOB_INFO_2* (Printjob::m_jobs_info_list) = nullptr;
    std::vector<std::unique_ptr<Printjob>> Printjob::m_job_list(0);

    /**
     Function definitions
    */
    std::vector<std::unique_ptr<Printjob>>* Printjob::load_jobs(const _PRINTER_INFO_2W& printer_info) {
        if (set_jobs_info_list(printer_info) == FALSE) {
            return &m_job_list;
        }

        // Reserve space for printjobs in vector
        m_job_list.reserve(m_job_count);

        // Fill vector with printjobs
        for (unsigned i = 0; i < m_job_count; ++i) {
            m_job_list.push_back(std::make_unique<Printjob>(m_jobs_info_list[i]));
        }

        return &m_job_list;
    }

    BOOL Printjob::set_jobs_info_list(const _PRINTER_INFO_2W& printer_info) {
        // Drucker-Handle erstellen
        HANDLE printer_handle;

        if (OpenPrinterW(printer_info.pPrinterName, &printer_handle, NULL) == FALSE) {
            if (printer_handle) {
                CloseHandle(printer_handle);
            }
            return FALSE;
        }

        // Buffer-Gr��e bestimmen
        DWORD needed_buffer;

        EnumJobsW(printer_handle, NULL, printer_info.cJobs, 2, NULL, NULL,
            &needed_buffer, &m_job_count);

        // Job_info Array anhand von Buffer erstellen
        const unsigned size = static_cast<unsigned>(static_cast<double>(needed_buffer) / sizeof(JOB_INFO_2));
        m_jobs_info_list = new JOB_INFO_2[size];

        BOOL res = EnumJobsW(printer_handle, NULL, printer_info.cJobs, 2,
            reinterpret_cast<LPBYTE>(m_jobs_info_list),
            needed_buffer, &needed_buffer, &m_job_count);

        return res;
    }

    void Printjob::set_document_name() { m_document_name = std::wstring(m_job_info.pDocument); }
    void Printjob::set_user_name() { m_user_name = std::wstring(m_job_info.pUserName); }
    void Printjob::set_machine_name() { m_machine_name = std::wstring(m_job_info.pMachineName); }
    void Printjob::set_datatype() { m_datatype = std::wstring(m_job_info.pDatatype); }
    void Printjob::set_size() { m_size = std::wstring(Helper::Format::data_unit_conversion(m_job_info.Size)); }
    void Printjob::set_page_count() { m_page_count = std::to_wstring(m_job_info.TotalPages); }

    void Printjob::set_submitted() {
        SYSTEMTIME submitted_time = m_job_info.Submitted;
        m_submitted = std::to_wstring(submitted_time.wHour) + L":" + std::to_wstring(submitted_time.wMinute);
    }

    void Printjob::set_status() {
        m_status = L"";

        if (m_job_info.Status & JOB_STATUS_BLOCKED_DEVQ) { m_status += L"The driver cannot print the job."; }
        if (m_job_info.Status & JOB_STATUS_DELETED) { m_status += L"Job has been deleted."; }
        if (m_job_info.Status & JOB_STATUS_DELETING) { m_status += L"Job is being deleted."; }
        if (m_job_info.Status & JOB_STATUS_ERROR) { m_status += L"An error is associated with the job."; }
        if (m_job_info.Status & JOB_STATUS_OFFLINE) { m_status += L"Printer is offline."; }
        if (m_job_info.Status & JOB_STATUS_PAPEROUT) { m_status += L"Printer is out of paper."; }
        if (m_job_info.Status & JOB_STATUS_PAUSED) { m_status += L"Job is paused."; }
        if (m_job_info.Status & JOB_STATUS_PRINTED) { m_status += L"Job has printed."; }
        if (m_job_info.Status & JOB_STATUS_PRINTING) { m_status += L"Job is printing."; }
        if (m_job_info.Status & JOB_STATUS_RESTART) { m_status += L"Job has been restarted."; }
        if (m_job_info.Status & JOB_STATUS_SPOOLING) { m_status += L"Job is spooling."; }
        if (m_job_info.Status & JOB_STATUS_USER_INTERVENTION) { m_status += L"Printer has an error that requires the user to do something."; }
        if (m_job_info.Status & JOB_STATUS_COMPLETE) { m_status += L"The job is sent to the printer, but may not be printed yet."; }
        if (m_job_info.Status & JOB_STATUS_RETAINED) { m_status += L"The job has been retained in the print queue following printing."; }
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

    unsigned Printjob::get_job_count() { return static_cast<unsigned>(m_job_count); }

    const std::wstring& Printjob::get_document_name() const { return m_document_name; }
    const std::wstring& Printjob::get_user_name() const { return m_user_name; }
    const std::wstring& Printjob::get_machine_name() const { return m_machine_name; }
    const std::wstring& Printjob::get_submitted() const { return m_submitted; }
    const std::wstring& Printjob::get_datatype() const { return m_datatype; }
    const std::wstring& Printjob::get_size() const { return m_size; }
    const std::wstring& Printjob::get_status() const { return m_status; }
    const std::wstring& Printjob::get_page_count() const { return m_page_count; }

    std::wostream& operator<<(std::wostream& stream, const WinApi::Printjob& job) {
        using namespace Helper;

        stream << Format::name_and_value(L"Dokument: ", job.m_document_name) << '\n'
            << Format::name_and_value(L"User: ", job.m_user_name) << '\n'
            << Format::name_and_value(L"Maschine: ", job.m_machine_name) << '\n'
            << Format::name_and_value(L"Zeit: ", job.m_submitted) << '\n'
            << Format::name_and_value(L"Datentyp: ", job.m_datatype) << '\n'
            << Format::name_and_value(L"Gr��e: ", job.m_size) << '\n'
            << Format::name_and_value(L"Seitenanzahl: ", job.m_page_count) << '\n'
            << Format::name_and_value(L"Status: ", job.m_status) << '\n';

        return stream;
    }
}  // namespace WinApi

