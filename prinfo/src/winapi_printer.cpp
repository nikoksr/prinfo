#include "winapi_printer.hpp"
#include "format.hpp"
#include "snippets.hpp"

#include <stdexcept>
#include <math.h>
#include <Wingdi.h>
#include <string>

namespace winapi {

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
    const std::vector<std::unique_ptr<Printer>>& Printer::LoadPrinters() {
        // Clear old list
        m_printer_list.clear();
        m_number_printers = 0;

        // Load infos lists
        if (!setPrinterInfoList()) {
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

    BOOL Printer::setPrinterInfoList() {
        DWORD needed_buffer;

        EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
            nullptr, 2, nullptr, NULL, &needed_buffer,
            reinterpret_cast<LPDWORD>(&m_number_printers));

        const unsigned size = static_cast<const unsigned>(ceil(static_cast<double>(needed_buffer) / sizeof(_PRINTER_INFO_2W)));

        if (m_printer_info_list) {
            m_printer_info_list.reset();
        }

        m_printer_info_list = std::make_unique<_PRINTER_INFO_2W[]>(size);

        BOOL result = EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
            nullptr, 2, reinterpret_cast<LPBYTE>(m_printer_info_list.get()), needed_buffer,
            &needed_buffer, reinterpret_cast<LPDWORD>(&m_number_printers));

        return result;
    }

    void Printer::init() {
        setName();
        setType();
        setPort();
        setIsShared();
        setShareName();
        setServerName();
        setTerminalServer();
        setDriver();
        setPrintProcessor();
        setDataType();
        setDuplex();
        setKeepsPrintjobs();
        setStatus();
        setPrintjobsCount();
        setPrintjobs();
        setBins();
    }

    /**
      Setter functions
    */
    void Printer::setName() {
        if (m_printer_info.pPrinterName) {
            m_name = m_printer_info.pPrinterName;
        }
        else {
            m_name = L"";
        }
    }
    void Printer::setDriver() {
        if (m_printer_info.pDriverName) {
            m_driver = m_printer_info.pDriverName;
        }
        else {
            m_driver = L"";
        }
    }
    void Printer::setPrintProcessor() {
        if (m_printer_info.pPrintProcessor) {
            m_printprocessor = m_printer_info.pPrintProcessor;
        }
        else {
            m_printprocessor = L"";
        }
    }
    void Printer::setDataType() {
        if (m_printer_info.pDatatype) {
            m_datatype = m_printer_info.pDatatype;
        }
        else {
            m_datatype = L"";
        }
    }

    void Printer::setType() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_LOCAL) {
            m_type = L"Lokaler Drucker";
        }
        else {
            m_type = L"Netzwerk Drucker";
        }
    }

    void Printer::setPort() {
        if (m_printer_info.pPortName) {
            m_port = m_printer_info.pPortName;
        }
        else {
            m_port = L"";
        }
    }

    void Printer::setIsShared() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_SHARED) {
            m_is_shared = L"Drucker ist freigegeben";
        }
        else {
            m_is_shared = L"Drucker ist nicht freigegeben";
        }
    }

    void Printer::setShareName() {
        if (m_printer_info.pShareName) {
            m_share_name = m_printer_info.pShareName;
        }
        else {
            m_share_name = L"";
        }
    }

    void Printer::setServerName() {
        if (m_printer_info.pServerName) {
            m_server_name = m_printer_info.pServerName;
        }
        else {
            m_server_name = L"";
        }
    }

    void Printer::setTerminalServer() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_TS) {
            m_terminalserver = L"Ja";
        }
        else {
            m_terminalserver = L"Nein";
        }
    }

    void Printer::setDuplex() {
        if (!m_printer_info.pDevMode) {
            m_duplex = L"Status unbekannt";
            return;
        }

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

    void Printer::setKeepsPrintjobs() {
        if (m_printer_info.Attributes & PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS) {
            m_keep_printjobs = L"nach dem Drucken nicht löschen";
        }
        else {
            m_keep_printjobs = L"nach dem Drucken löschen";
        }
    }

    void Printer::setStatus() {
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

    void Printer::setPrintjobsCount() {
        m_printjobs_count = static_cast<unsigned>(m_printer_info.cJobs);
    }

    void Printer::setPrintjobs() {
        // Check if there are queued printjobs
        if (m_printjobs_count < 1) {
            return;
        }

        // Load printjobs  
        if (Printjob::SetPrintjobs(this) != 0) {}
    }

    void Printer::setBins() {
        // Determine number of bins
        size_t num_bins = DeviceCapabilitiesW(
            m_name.c_str(),
            m_port.c_str(),
            DC_BINNAMES,
            nullptr,
            nullptr
        );

        // Get bin names
        const size_t MAX_NAME_LEN = 24;
        std::vector<wchar_t> bin_names;
        bin_names.resize(num_bins * MAX_NAME_LEN);

        num_bins = DeviceCapabilitiesW(
            m_name.c_str(),
            m_port.c_str(),
            DC_BINNAMES,
            bin_names.data(),
            nullptr
        );

        m_bins.clear();
        m_bins.reserve(num_bins);

        // Return if no bins were found
        if (num_bins < 1) {
            m_default_source = L"";
            return;
        }

        for (size_t i = 0; i < num_bins; i++) {
            m_bins.emplace_back(
                Format::RemoveTrailingZeros(
                    std::wstring(bin_names.begin() + i * MAX_NAME_LEN, bin_names.begin() + (i + 1) * MAX_NAME_LEN)
                )
            );
        }

        // Default source
        m_default_source = std::to_wstring(m_printer_info.pDevMode->dmDefaultSource);

        num_bins = DeviceCapabilitiesW(
            m_name.c_str(),
            m_port.c_str(),
            DC_BINS,
            nullptr,
            nullptr
        );

        std::vector<short> bin_buf;
        bin_buf.resize(num_bins);

        num_bins = DeviceCapabilitiesW(
            m_name.c_str(),
            m_port.c_str(),
            DC_BINS,
            reinterpret_cast<LPWSTR>(bin_buf.data()),
            nullptr
        );

        // Determine actual name of default source
        for (unsigned i = 0; i < num_bins; i++) {
            if (std::to_wstring(bin_buf[i]) == m_default_source) {
                m_default_source = m_bins.at(i);
            }
        }
    }

    /**
      Getter functions
    */
    int Printer::NumberOfPrinters() { return m_number_printers; }
    const _PRINTER_INFO_2W& Printer::PrinterInfo() const { return m_printer_info; }
    const std::wstring& Printer::Name() const { return m_name; }
    const std::wstring& Printer::Type() const { return m_type; }
    const std::wstring& Printer::Port() const { return m_port; }
    const std::wstring& Printer::IsShared() const { return m_is_shared; }
    const std::wstring& Printer::ShareName() const { return m_share_name; }
    const std::wstring& Printer::ServerName() const { return m_server_name; }
    const std::wstring& Printer::TerminalServer() const { return m_terminalserver; }
    const std::wstring& Printer::Driver() const { return m_driver; }
    const std::wstring& Printer::PrintProcessor() const { return m_printprocessor; }
    const std::wstring& Printer::DataType() const { return m_datatype; }
    const std::wstring& Printer::Duplex() const { return m_duplex; }
    const std::wstring& Printer::KeepsPrintjobs() const { return m_keep_printjobs; }
    const std::wstring& Printer::Status() const { return m_status; }
    const std::wstring& Printer::DefaultTray() const { return m_default_source; }
    const std::vector<std::wstring> Printer::Trays() const { return m_bins; }

    std::wostream& operator<<(std::wostream& stream, const Printer& printer) {
        stream << snippets::k_separator_thin << L"\n\n"
            << L" " << printer.m_name << L"\n"
            << snippets::k_separator_thin << L"\n\n"
            << Format::NameValuePair(L"Typ", printer.m_type) << L"\n"
            << Format::NameValuePair(L"Port", printer.m_port) << L"\n"
            << Format::NameValuePair(L"Standard Quelle", printer.m_default_source) << L"\n"
            << Format::NameValuePair(L"Verfügbare Schächte", Format::VecToStr(printer.m_bins, L", ")) << L"\n"
            << Format::NameValuePair(L"Freigabe", printer.m_is_shared) << L"\n"
            << Format::NameValuePair(L"Sharename", printer.m_share_name) << L"\n"
            << Format::NameValuePair(L"Servername", printer.m_server_name) << L"\n"
            << Format::NameValuePair(L"Über Terminal Server verbunden", printer.m_terminalserver) << L"\n"
            << Format::NameValuePair(L"Treiber", printer.m_driver) << L"\n"
            << Format::NameValuePair(L"Printprocessor", printer.m_printprocessor) << L"\n"
            << Format::NameValuePair(L"Datentyp", printer.m_datatype) << L"\n"
            << Format::NameValuePair(L"Duplex", printer.m_duplex) << L"\n"
            << Format::NameValuePair(L"Status", printer.m_status) << L"\n"
            << Format::NameValuePair(L"Druckaufträge", printer.m_keep_printjobs) << L"\n"
            << Format::NameValuePair(L"Anzahl Druckaufträge", std::to_wstring(printer.m_printjobs_count)) << L"\n"
            << Format::NameValuePair(L"Druckaufträge", L"", L' ') << L"\n";

        for (auto& job : printer.m_printjobs) {
            stream << L"\n" << job;
        }

        stream << L"\n";
        return stream;
    }

}

