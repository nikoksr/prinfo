#include "winapi_printjob.hpp"
#include "format.hpp"

#include <math.h>

namespace WinApi {

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
        set_document_name();
        set_user_name();
        set_machine_name();
        set_submitted();
        set_datatype();
        set_size();
        set_status();
        set_page_count();
    }

    int Printjob::set_printjobs(Printer* const printer) {
        // Check printer
        if (!printer) {
            return 1;
        }

        // Clear old printjob list
        printer->m_printjobs.clear();

        // Load jobs infos
        std::unique_ptr<_JOB_INFO_2W[]> jobs_info_list;
        set_jobs_info_list(printer, jobs_info_list);
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

    void Printjob::set_jobs_info_list(Printer* const printer, std::unique_ptr<_JOB_INFO_2W[]>& out_jobs_info_list) {
        // Drucker-Handle erstellen
        HANDLE printer_handle;

        if (!OpenPrinterW(printer->get_printer_info().pPrinterName, &printer_handle, NULL)) {
            if (printer_handle) {
                CloseHandle(printer_handle);
            }
            return;
        }

        // Buffer-Größe bestimmen
        DWORD needed_buffer;
        DWORD needed_structs;

        EnumJobsW(printer_handle, 0, printer->get_printer_info().cJobs, 2, NULL, NULL,
            &needed_buffer, &needed_structs);

        // Job_info Daten abfragen
        const std::size_t size = static_cast<const std::size_t>(ceil(static_cast<double>(needed_buffer) / sizeof(_JOB_INFO_2W)));
        out_jobs_info_list = std::make_unique<_JOB_INFO_2W[]>(size);

        EnumJobsW(printer_handle, NULL, printer->get_printer_info().cJobs, 2,
            reinterpret_cast<LPBYTE>(out_jobs_info_list.get()),
            needed_buffer, &needed_buffer, &needed_structs);
    }

    void Printjob::set_document_name() { m_document_name = std::wstring(m_job_info.pDocument); }
    void Printjob::set_user_name() { m_user_name = std::wstring(m_job_info.pUserName); }
    void Printjob::set_machine_name() { m_machine_name = std::wstring(m_job_info.pMachineName); }
    void Printjob::set_datatype() { m_datatype = std::wstring(m_job_info.pDatatype); }
    void Printjob::set_size() { m_size = std::wstring(Helper::Format::data_unit_conversion(m_job_info.Size)); }
    void Printjob::set_page_count() { m_page_count = std::to_wstring(m_job_info.TotalPages); }

    void Printjob::set_submitted() {
        SYSTEMTIME submitted_time = m_job_info.Submitted;
        m_submitted = std::to_wstring(submitted_time.wDay) + L"." +
            std::to_wstring(submitted_time.wMonth) + L"." +
            std::to_wstring(submitted_time.wYear) + L' ' +
            std::to_wstring(submitted_time.wHour) + L":" +
            std::to_wstring(submitted_time.wMinute) + L":" +
            std::to_wstring(submitted_time.wSecond);
    }

    void Printjob::set_status() {
        m_status = L"";

        if (m_job_info.Status& JOB_STATUS_BLOCKED_DEVQ) { m_status += L"Der Treiber kann den Auftrag nicht drucken.\n"; }
        if (m_job_info.Status& JOB_STATUS_DELETED) { m_status += L"Auftrag wurde gelöscht.\n"; }
        if (m_job_info.Status& JOB_STATUS_DELETING) { m_status += L"Auftrag wird gelöscht.\n"; }
        if (m_job_info.Status& JOB_STATUS_ERROR) { m_status += L"In dem Auftrag ist ein Fehler aufgetreten.\n"; }
        if (m_job_info.Status& JOB_STATUS_OFFLINE) { m_status += L"Drucker ist offline.\n"; }
        if (m_job_info.Status& JOB_STATUS_PAPEROUT) { m_status += L"Drucker hat kein Papier mehr.\n"; }
        if (m_job_info.Status& JOB_STATUS_PAUSED) { m_status += L"Auftrag wurde pausiert.\n"; }
        if (m_job_info.Status& JOB_STATUS_PRINTED) { m_status += L"Auftrag wurde gedruckt.\n"; }
        if (m_job_info.Status& JOB_STATUS_PRINTING) { m_status += L"Auftrag wird gedruckt.\n"; }
        if (m_job_info.Status& JOB_STATUS_RESTART) { m_status += L"Auftrag wurde neugestartet.\n"; }
        if (m_job_info.Status& JOB_STATUS_SPOOLING) { m_status += L"Auftrag wird gespoolt.\n"; }
        if (m_job_info.Status& JOB_STATUS_USER_INTERVENTION) { m_status += L"Drucker hat einen Fehler der eine Benutzer Intervention benötigt.\n"; }
        if (m_job_info.Status& JOB_STATUS_COMPLETE) { m_status += L"Auftrag wurde an Drucker gesendet; wohlmöglich noch nicht gedruckt.\n"; }
        if (m_job_info.Status& JOB_STATUS_RETAINED) { m_status += L"Auftrag wurde nach dem Drucken in der Warteschlange behalten.\n"; }
    }

    const std::wstring& Printjob::get_document_name() const { return m_document_name; }
    const std::wstring& Printjob::get_user_name() const { return m_user_name; }
    const std::wstring& Printjob::get_machine_name() const { return m_machine_name; }
    const std::wstring& Printjob::get_submitted() const { return m_submitted; }
    const std::wstring& Printjob::get_datatype() const { return m_datatype; }
    const std::wstring& Printjob::get_size() const { return m_size; }
    const std::wstring& Printjob::get_status() const { return m_status; }
    const std::wstring& Printjob::get_page_count() const { return m_page_count; }

    std::wostream& operator<<(std::wostream& stream, const Printjob& job) {
        using namespace Helper;

        stream << Format::name_and_value(L"Dokument", job.m_document_name) << '\n'
            << Format::name_and_value(L"User", job.m_user_name) << '\n'
            << Format::name_and_value(L"Maschine", job.m_machine_name) << '\n'
            << Format::name_and_value(L"Zeit", job.m_submitted) << '\n'
            << Format::name_and_value(L"Datentyp", job.m_datatype) << '\n'
            << Format::name_and_value(L"Größe", job.m_size) << '\n'
            << Format::name_and_value(L"Seitenanzahl", job.m_page_count) << '\n'
            << Format::name_and_value(L"Status", job.m_status) << '\n';

        return stream;
    }
}  // namespace WinApi

