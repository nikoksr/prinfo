#include "winapi_printjob.hpp"
#include "format.hpp"

#include <math.h>

namespace winapi {

    /**
     Constructor and Destructor
    */
    Printjob::Printjob(const _JOB_INFO_2W& job_info) : m_job_info(job_info) {
        init();
    }

    Printjob::~Printjob() {
    }

    /**
     Function definitions
    */
    void Printjob::init() {
        setID();
        setDocumentName();
        setUserName();
        setMachineName();
        setSubmitted();
        setDataType();
        setSize();
        setStatus();
        setPageCount();
    }

    int Printjob::SetPrintjobs(Printer* const printer) {
        // Check printer
        if (!printer) {
            return 1;
        }

        // Clear old printjob list
        printer->m_printjobs.clear();

        // Load jobs infos
        std::unique_ptr<_JOB_INFO_2W[]> jobs_info_list;
        setJobsInfoList(printer, jobs_info_list);
        if (!jobs_info_list) {
            return 2;
        }

        // Reserve space for printjobs in vector
        printer->m_printjobs.reserve(printer->m_printjobs_count);

        // Fill vector with printjobs
        for (unsigned i = 0; i < printer->m_printjobs_count; ++i) {
            printer->m_printjobs.emplace_back(jobs_info_list[i]);
        }

        return 0;
    }

    void Printjob::setJobsInfoList(Printer* const printer, std::unique_ptr<_JOB_INFO_2W[]>& out_jobs_info_list) {
        // Drucker-Handle erstellen
        HANDLE printer_handle;

        if (!OpenPrinterW(printer->PrinterInfo().pPrinterName, &printer_handle, NULL)) {
            if (printer_handle) {
                CloseHandle(printer_handle);
            }
            return;
        }

        // Buffer-Größe bestimmen
        DWORD needed_buffer;
        DWORD needed_structs;

        EnumJobsW(printer_handle, 0, printer->PrinterInfo().cJobs, 2, NULL, NULL,
            &needed_buffer, &needed_structs);

        // Job_info Daten abfragen
        const std::size_t size = static_cast<const std::size_t>(ceil(static_cast<double>(needed_buffer) / sizeof(_JOB_INFO_2W)));
        out_jobs_info_list = std::make_unique<_JOB_INFO_2W[]>(size);

        EnumJobsW(printer_handle, NULL, printer->PrinterInfo().cJobs, 2,
            reinterpret_cast<LPBYTE>(out_jobs_info_list.get()),
            needed_buffer, &needed_buffer, &needed_structs);
    }

    void Printjob::setID() {
        m_id = std::to_wstring(m_job_info.JobId);
    }

    void Printjob::setDocumentName() {
        if (m_job_info.pDocument) {
            m_document_name = std::wstring(m_job_info.pDocument);
        }
        else {
            m_document_name = L"";
        }
    }

    void Printjob::setUserName() {
        if (m_job_info.pUserName) {
            m_user_name = std::wstring(m_job_info.pUserName);
        }
        else {
            m_user_name = L"";
        }
    }

    void Printjob::setMachineName() {
        if (m_job_info.pMachineName) {
            m_machine_name = std::wstring(m_job_info.pMachineName);
        }
        else {
            m_machine_name = L"";
        }
    }

    void Printjob::setDataType() {
        if (m_job_info.pDatatype) {
            m_datatype = std::wstring(m_job_info.pDatatype);
        }
        else {
            m_datatype = L"";
        }
    }

    void Printjob::setSize() {
        m_size = std::wstring(Format::ConvertDataUnit(m_job_info.Size));
    }

    void Printjob::setPageCount() {
        m_page_count = std::to_wstring(m_job_info.TotalPages);
    }

    void Printjob::setSubmitted() {
        SYSTEMTIME submitted_time = m_job_info.Submitted;
        m_submitted = std::to_wstring(submitted_time.wDay) + L"." +
            std::to_wstring(submitted_time.wMonth) + L"." +
            std::to_wstring(submitted_time.wYear) + L' ' +
            std::to_wstring(submitted_time.wHour) + L":" +
            std::to_wstring(submitted_time.wMinute) + L":" +
            std::to_wstring(submitted_time.wSecond);
    }

    void Printjob::setStatus() {
        m_status = L"";

        if (m_job_info.Status& JOB_STATUS_BLOCKED_DEVQ) { m_status += L"Der Treiber kann den Auftrag nicht drucken. "; }
        if (m_job_info.Status& JOB_STATUS_DELETED) { m_status += L"Auftrag wurde gelöscht. "; }
        if (m_job_info.Status& JOB_STATUS_DELETING) { m_status += L"Auftrag wird gelöscht. "; }
        if (m_job_info.Status& JOB_STATUS_ERROR) { m_status += L"In dem Auftrag ist ein Fehler aufgetreten. "; }
        if (m_job_info.Status& JOB_STATUS_OFFLINE) { m_status += L"Drucker ist offline. "; }
        if (m_job_info.Status& JOB_STATUS_PAPEROUT) { m_status += L"Drucker hat kein Papier mehr. "; }
        if (m_job_info.Status& JOB_STATUS_PAUSED) { m_status += L"Auftrag wurde pausiert. "; }
        if (m_job_info.Status& JOB_STATUS_PRINTED) { m_status += L"Auftrag wurde gedruckt. "; }
        if (m_job_info.Status& JOB_STATUS_PRINTING) { m_status += L"Auftrag wird gedruckt. "; }
        if (m_job_info.Status& JOB_STATUS_RESTART) { m_status += L"Auftrag wurde neugestartet. "; }
        if (m_job_info.Status& JOB_STATUS_SPOOLING) { m_status += L"Auftrag wird gespoolt. "; }
        if (m_job_info.Status& JOB_STATUS_USER_INTERVENTION) { m_status += L"Drucker hat einen Fehler der eine Benutzer Intervention benötigt. "; }
        if (m_job_info.Status& JOB_STATUS_COMPLETE) { m_status += L"Auftrag wurde an Drucker gesendet; wohlmöglich noch nicht gedruckt. "; }
        if (m_job_info.Status& JOB_STATUS_RETAINED) { m_status += L"Auftrag wurde nach dem Drucken in der Warteschlange behalten. "; }
    }

    const std::wstring& Printjob::ID() const { return m_id; }
    const std::wstring& Printjob::DocumentName() const { return m_document_name; }
    const std::wstring& Printjob::UserName() const { return m_user_name; }
    const std::wstring& Printjob::MachineName() const { return m_machine_name; }
    const std::wstring& Printjob::Submitted() const { return m_submitted; }
    const std::wstring& Printjob::DataType() const { return m_datatype; }
    const std::wstring& Printjob::Size() const { return m_size; }
    const std::wstring& Printjob::Status() const { return m_status; }
    const std::wstring& Printjob::PageCount() const { return m_page_count; }

    std::wostream& operator<<(std::wostream& stream, const Printjob& job) {
        stream << Format::NameValuePair(L"Dokument", job.m_document_name) << '\n'
            << Format::NameValuePair(L"ID", job.m_id) << '\n'
            << Format::NameValuePair(L"User", job.m_user_name) << '\n'
            << Format::NameValuePair(L"Maschine", job.m_machine_name) << '\n'
            << Format::NameValuePair(L"Zeit", job.m_submitted) << '\n'
            << Format::NameValuePair(L"Datentyp", job.m_datatype) << '\n'
            << Format::NameValuePair(L"Größe", job.m_size) << '\n'
            << Format::NameValuePair(L"Seitenanzahl", job.m_page_count) << '\n'
            << Format::NameValuePair(L"Status", job.m_status) << '\n';

        return stream;
    }
}  // namespace WinApi

