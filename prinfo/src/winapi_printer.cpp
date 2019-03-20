#include "winapi_printer.hpp"
#include "format.hpp"
#include "menu.hpp"
#include <stdexcept>

namespace WinApi {

    /**
      Init static attributes
     */
    unsigned Printer::obj_counter = 0;
    int Printer::number_printers = 0;
    std::vector<Printer *> Printer::printer_list(0);
    _PRINTER_INFO_2W *(Printer::printer_info_list) = nullptr;

    /**
      Constructor and Destructor
    */
    Printer::Printer(const _PRINTER_INFO_2W& printer_info) : m_printer_info(printer_info) {
        obj_counter++;
        init();
    }

    Printer::~Printer() {
        if (obj_counter < 2 && printer_info_list) {
            delete[] printer_info_list;
        }

        obj_counter--;
    }

    /**
      Functions
    */
    std::vector<Printer*> Printer::load_printers() {
        if (set_printer_info_list() == FALSE) {
            return {};
        }

        for (int i = 0; i < number_printers; ++i) {
            Printer* new_printer = new Printer(printer_info_list[i]);
            printer_list.push_back(new_printer);
        }

        return printer_list;
    }

    BOOL Printer::set_printer_info_list() {
        DWORD needed_buffer;

        EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
            nullptr, 2, nullptr, NULL, &needed_buffer,
            reinterpret_cast<LPDWORD>(&number_printers));

        const unsigned size = static_cast<unsigned>(static_cast<double>(needed_buffer) / sizeof(_PRINTER_INFO_2W));
        printer_info_list = new _PRINTER_INFO_2W[size];

        BOOL result = EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
            nullptr, 2, reinterpret_cast<LPBYTE>(printer_info_list), needed_buffer,
            &needed_buffer, reinterpret_cast<LPDWORD>(&number_printers));

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

    void Printer::display(std::wostream &stream) {
        using namespace Helper;

        stream << Menu::get_separator_thin() << L"\n\n"
            << L" " << m_name << L"\n"
            << Menu::get_separator_thin() << L"\n\n"
            << Format::name_and_value(L"Status", m_status) << L"\n\n"
            << Format::name_and_value(L"Typ", m_type) << L"\n"
            << Format::name_and_value(L"Port", m_port) << L"\n"
            << Format::name_and_value(L"Freigabe", m_is_shared) << L"\n"
            << Format::name_and_value(L"Sharename", m_share_name) << L"\n"
            << Format::name_and_value(L"Servername", m_server_name) << L"\n"
            << Format::name_and_value(L"Über Terminal Server verbunden", m_terminalserver) << L"\n\n"
            << Format::name_and_value(L"Treiber", m_driver) << L"\n"
            << Format::name_and_value(L"Printprocessor", m_printprocessor) << L"\n"
            << Format::name_and_value(L"Datentyp", m_datatype) << L"\n"
            << Format::name_and_value(L"Duplex", m_duplex) << L"\n\n"
            << Format::name_and_value(L"Druckaufträge", m_keep_printjobs) << L"\n"
            << Format::name_and_value(L"Anzahl Druckaufträge", m_number_printjobs) << L"\n"
            << Format::name_and_value(L"Druckaufträge", L"") << L"\n";

        for (unsigned i = 0; i < m_printjobs.size(); ++i) {
            m_printjobs.at(i)->display(stream);
        }
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

    void Printer::set_printprocessor() {
        m_printprocessor = m_printer_info.pPrintProcessor;
    }

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

        if (m_printer_info.Status & PRINTER_STATUS_OFFLINE) {
            m_status += L"Offline\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_WAITING) {
            m_status += L"Drucker wartet\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PRINTING) {
            m_status += L"Druckvorgang\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_IO_ACTIVE) {
            m_status += L"Input/Output aktiv\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_ERROR) {
            m_status += L"Error\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_TONER_LOW) {
            m_status += L"Druckerpatronen schwach\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_NO_TONER) {
            m_status += L"Druckerpatronen leer\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PAPER_PROBLEM) {
            m_status += L"Papierproblem\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PAPER_OUT) {
            m_status += L"Papier leer\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_OUT_OF_MEMORY) {
            m_status += L"Speicher-Problem\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_SERVER_UNKNOWN) {
            m_status += L"Unbekannt\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_BUSY) {
            m_status += L"Beschäftigt\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_DOOR_OPEN) {
            m_status += L"Druckertür ist offen\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_INITIALIZING) {
            m_status += L"Initialisiert\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_NOT_AVAILABLE) {
            m_status += L"Nicht verfügbar\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_OUTPUT_BIN_FULL) {
            m_status += L"Ausgabebehälter ist voll\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PAGE_PUNT) {
            m_status += L"Aktuelle Seite kann nicht gedruckt werden\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PAPER_JAM) {
            m_status += L"Papierstau\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PAUSED) {
            m_status += L"Pausiert\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PAGE_PUNT) {
            m_status += L"Aktuelle Seite kann nicht gedruckt werden\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_POWER_SAVE) {
            m_status += L"Energiesparmdous\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_PROCESSING) {
            m_status += L"Druckauftrag wird bearbeitet\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_WAITING) {
            m_status += L"Wartet\n";
        }

        if (m_printer_info.Status & PRINTER_STATUS_WARMING_UP) {
            m_status += L"Aufwärmen\n";
        }
    }

    void Printer::set_printjobs() {
        // Load printjobs
        m_printjobs = Printjob::load_jobs(m_printer_info);

        // Set number of printjobs
        m_number_printjobs = Printjob::get_job_count();
    }

    /**
      Getter functions
    */
    int Printer::get_number_printers() { return number_printers; }

    std::wstring Printer::get_name() {
        return m_name;
    }

    std::wstring Printer::get_type() {
        return m_type;
    }

    std::wstring Printer::get_port() {
        return m_port;
    }

    std::wstring Printer::get_is_shared() {
        return m_is_shared;
    }

    std::wstring Printer::get_sharename() {
        return m_share_name;
    }

    std::wstring Printer::get_server_name() {
        return m_server_name;
    }

    std::wstring Printer::get_terminalserver() {
        return m_terminalserver;
    }

    std::wstring Printer::get_driver() {
        return m_driver;
    }

    std::wstring Printer::get_printprocessor() {
        return m_printprocessor;
    }

    std::wstring Printer::get_datatype() {
        return m_datatype;
    }

    std::wstring Printer::get_duplex() {
        return m_duplex;
    }

    std::wstring Printer::get_keep_printjobs() {
        return m_keep_printjobs;
    }

    std::wstring Printer::get_status() {
        return m_status;
    }

}  // namespace WinApi