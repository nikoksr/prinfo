#include "winapi_printer.hpp"
#include "format.hpp"
#include "snippets.hpp"
#include <stdexcept>
#include <math.h>

namespace WinApi {

    /**
    * Static member initialization
    */
    int Printer::m_number_printers = 0;
    std::unique_ptr<_PRINTER_INFO_2W[]> Printer::m_printer_info_list;
    std::vector<std::unique_ptr<Printer>> Printer::m_printer_list(0);

    /**
      Constructor and Destructor
    */
    Printer::Printer(const _PRINTER_INFO_2W& printer_info) : m_printer_info(printer_info) {
        init();
    }

    Printer::~Printer() {
    }

    /**
      Functions
    */
    const std::vector<std::unique_ptr<Printer>>& Printer::load_printers() {
        if (set_printer_info_list() == FALSE) {
            return m_printer_list;
        }

        // Reserve space for printers in vector
        m_printer_list.reserve(m_number_printers);

        // Fill vector with printers
        for (int i = 0; i < m_number_printers; ++i) {
            m_printer_list.push_back(std::make_unique<Printer>(m_printer_info_list[i]));
        }

        return m_printer_list;
    }

    BOOL Printer::set_printer_info_list() {
        DWORD needed_buffer;

        EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
            nullptr, 2, nullptr, NULL, &needed_buffer,
            reinterpret_cast<LPDWORD>(&m_number_printers));

        const unsigned size = static_cast<const unsigned>(ceil(static_cast<double>(needed_buffer) / sizeof(_PRINTER_INFO_2W)));

        if (m_printer_info_list.get()) {
            m_printer_info_list.reset();
        }

        m_printer_info_list = std::make_unique<_PRINTER_INFO_2W[]>(size);

        BOOL result = EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
            nullptr, 2, reinterpret_cast<LPBYTE>(m_printer_info_list.get()), needed_buffer,
            &needed_buffer, reinterpret_cast<LPDWORD>(&m_number_printers));

        return result;
    }

    void Printer::init() {
        set_name();
        set_type();
        set_port();
        set_is_shared();
        set_sharename();
        set_server_name();
        set_terminalserver();
        set_driver();
        set_printprocessor();
        set_datatype();
        set_duplex();
        set_keep_printjobs();
        set_status();
        set_printjobs();
    }

    /**
      Setter functions
    */
    void Printer::set_name() { m_name = m_printer_info.pPrinterName; }

    void Printer::set_type() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_LOCAL) {
            m_type = L"Lokaler Drucker";
        }
        else {
            m_type = L"Netzwerk Drucker";
        }
    }

    void Printer::set_port() { m_port = m_printer_info.pPortName; }

    void Printer::set_is_shared() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_SHARED) {
            m_is_shared = L"Drucker ist freigegeben";
        }
        else {
            m_is_shared = L"Drucker ist nicht freigegeben";
        }
    }

    void Printer::set_sharename() { m_share_name = m_printer_info.pShareName; }

    void Printer::set_server_name() {
        if (m_printer_info.pServerName != NULL) {
            m_server_name = m_printer_info.pServerName;
        }
        else {
            m_server_name = L"";
        }
    }

    void Printer::set_terminalserver() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_TS) {
            m_terminalserver = L"Ja";
        }
        else {
            m_terminalserver = L"Nein";
        }
    }

    void Printer::set_driver() { m_driver = m_printer_info.pDriverName; }
    void Printer::set_printprocessor() { m_printprocessor = m_printer_info.pPrintProcessor; }
    void Printer::set_datatype() { m_datatype = m_printer_info.pDatatype; }

    void Printer::set_duplex() {
        switch (m_printer_info.pDevMode->dmDuplex) {
        case DMDUP_SIMPLEX:
            m_duplex = L"Nicht aktiv";
            break;
        case DMDUP_HORIZONTAL:
            m_duplex = L"Aktiv - Horizontaler Duplex-Druck";
            break;
        case DMDUP_VERTICAL:
            m_duplex = L"Aktiv - Vertikaler Duplex-Druck";
            break;
        default:
            m_duplex = L"Status unbekannt";
            break;
        }
    }

    void Printer::set_keep_printjobs() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS) {
            m_keep_printjobs = L"nach dem Drucken nicht löschen";
        }
        else {
            m_keep_printjobs = L"nach dem Drucken löschen";
        }
    }

    void Printer::set_status() {
        m_status = L"";

        if (m_printer_info.Status & PRINTER_STATUS_OFFLINE) { m_status += L"Offline\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_WAITING) { m_status += L"Drucker wartet\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PRINTING) { m_status += L"Druckvorgang\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_IO_ACTIVE) { m_status += L"Input/Output aktiv\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_ERROR) { m_status += L"Error\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_TONER_LOW) { m_status += L"Druckerpatronen schwach\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_NO_TONER) { m_status += L"Druckerpatronen leer\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PAPER_PROBLEM) { m_status += L"Papierproblem\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PAPER_OUT) { m_status += L"Papier leer\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_OUT_OF_MEMORY) { m_status += L"Speicher-Problem\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_SERVER_UNKNOWN) { m_status += L"Unbekannt\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_BUSY) { m_status += L"Beschäftigt\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_DOOR_OPEN) { m_status += L"Druckertür ist offen\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_INITIALIZING) { m_status += L"Initialisiert\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_NOT_AVAILABLE) { m_status += L"Nicht verfügbar\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_OUTPUT_BIN_FULL) { m_status += L"Ausgabebehälter ist voll\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PAGE_PUNT) { m_status += L"Aktuelle Seite kann nicht gedruckt werden\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PAPER_JAM) { m_status += L"Papierstau\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PAUSED) { m_status += L"Pausiert\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PAGE_PUNT) { m_status += L"Aktuelle Seite kann nicht gedruckt werden\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_POWER_SAVE) { m_status += L"Energiesparmdous\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_PROCESSING) { m_status += L"Druckauftrag wird bearbeitet\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_WAITING) { m_status += L"Wartet\n"; }
        if (m_printer_info.Status & PRINTER_STATUS_WARMING_UP) { m_status += L"Aufwärmen\n"; }
    }

    void Printer::set_printjobs() {
        // Load printjobs        
        m_printjobs = Printjob::load_jobs(m_printer_info);

        // Set number of printjobs
        m_number_printjobs = std::to_wstring(Printjob::get_job_count());
    }

    /**
      Getter functions
    */
    int Printer::get_number_printers() { return m_number_printers; }

    const _PRINTER_INFO_2W& Printer::get_printer_info() const { return m_printer_info; }
    const std::wstring& Printer::get_name() const { return m_name; }
    const std::wstring& Printer::get_type() const { return m_type; }
    const std::wstring& Printer::get_port() const { return m_port; }
    const std::wstring& Printer::get_is_shared() const { return m_is_shared; }
    const std::wstring& Printer::get_sharename() const { return m_share_name; }
    const std::wstring& Printer::get_server_name() const { return m_server_name; }
    const std::wstring& Printer::get_terminalserver() const { return m_terminalserver; }
    const std::wstring& Printer::get_driver() const { return m_driver; }
    const std::wstring& Printer::get_printprocessor() const { return m_printprocessor; }
    const std::wstring& Printer::get_datatype() const { return m_datatype; }
    const std::wstring& Printer::get_duplex() const { return m_duplex; }
    const std::wstring& Printer::get_keep_printjobs() const { return m_keep_printjobs; }
    const std::wstring& Printer::get_status() const { return m_status; }


    std::wostream& operator<<(std::wostream& stream, WinApi::Printer& printer) {
        using namespace Helper;

        stream << Snippets::separator_thin << L"\n\n"
            << L" " << printer.m_name << L"\n"
            << Snippets::separator_thin << L"\n\n"
            << Format::name_and_value(L"Typ", printer.m_type) << L"\n"
            << Format::name_and_value(L"Port", printer.m_port) << L"\n"
            << Format::name_and_value(L"Freigabe", printer.m_is_shared) << L"\n"
            << Format::name_and_value(L"Sharename", printer.m_share_name) << L"\n"
            << Format::name_and_value(L"Servername", printer.m_server_name) << L"\n"
            << Format::name_and_value(L"Über Terminal Server verbunden", printer.m_terminalserver) << L"\n"
            << Format::name_and_value(L"Treiber", printer.m_driver) << L"\n"
            << Format::name_and_value(L"Printprocessor", printer.m_printprocessor) << L"\n"
            << Format::name_and_value(L"Datentyp", printer.m_datatype) << L"\n"
            << Format::name_and_value(L"Duplex", printer.m_duplex) << L"\n"
            << Format::name_and_value(L"Status", printer.m_status) << L"\n"
            << Format::name_and_value(L"Druckaufträge", printer.m_keep_printjobs) << L"\n"
            << Format::name_and_value(L"Anzahl Druckaufträge", printer.m_number_printjobs) << L"\n"
            << Format::name_and_value(L"Druckaufträge", L"") << L"\n";

        for (auto& job : *(printer.m_printjobs)) {
            stream << '\n' << *job;
        }

        return stream;
    }

}  // namespace WinApi

