#include "winapi_printer.h"
#include <stdexcept>

namespace WinApi {

/**
  Define static attributes
 */
int Printer::number_printers = 0;
std::vector<Printer *> Printer::printer_list(0);
_PRINTER_INFO_2W *(Printer::printer_info_list) = nullptr;

/**
  Constructor and Destructor
*/
Printer::Printer(_PRINTER_INFO_2W *printer_info_) {
  printer_info = printer_info_;
  init();
}

Printer::~Printer() {
  if (printer_info_list) {
    delete Printer::printer_info_list;
  }
}

/**
  Functions
*/
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
}

BOOL Printer::set_printer_info_list() {
  DWORD needed_buffer;

  EnumPrintersW(
      PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
      nullptr, 2, nullptr, NULL, &needed_buffer,
      reinterpret_cast<LPDWORD>(&number_printers));

  printer_info_list = new _PRINTER_INFO_2W[needed_buffer];

  auto result = EnumPrintersW(
      PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_CATEGORY_ALL,
      nullptr, 2, reinterpret_cast<LPBYTE>(printer_info_list), needed_buffer,
      &needed_buffer, reinterpret_cast<LPDWORD>(&number_printers));

  return result;
}

std::vector<Printer *> Printer::get_printer_list() {
  load_printers();
  return printer_list;
}

void Printer::load_printers() {
  auto res = set_printer_info_list();

  for (auto i = 0; i < number_printers; ++i) {
    auto printer_info = printer_info_list[i];
    auto new_printer = new Printer(&printer_info);
    printer_list.push_back(new_printer);
  }
}

/**
  Setter functions
*/
void Printer::set_name() { name = printer_info->pPrinterName; }

void Printer::set_type() {
  if (printer_info->Attributes & PRINTER_ATTRIBUTE_LOCAL) {
    type = L"Lokaler Drucker";
  } else {
    type = L"Netzwerk Drucker";
  }
}

void Printer::set_port() { port = printer_info->pPortName; }

void Printer::set_is_shared() {
  if (printer_info->Attributes & PRINTER_ATTRIBUTE_SHARED) {
    is_shared = L"Drucker ist freigegeben";
  } else {
    is_shared = L"Drucker ist nicht freigegeben";
  }
}

void Printer::set_sharename() { sharename = printer_info->pShareName; }

void Printer::set_server_name() {
  if (printer_info->pServerName != NULL) {
    server_name = printer_info->pServerName;
  } else {
    server_name = L"";
  }
}

void Printer::set_terminalserver() {
  if (printer_info->Attributes & PRINTER_ATTRIBUTE_TS) {
    terminalserver = L"Ja";
  } else {
    terminalserver = L"Nein";
  }
}

void Printer::set_driver() { driver = printer_info->pDriverName; }

void Printer::set_printprocessor() {
  printprocessor = printer_info->pPrintProcessor;
}

void Printer::set_datatype() { datatype = printer_info->pDatatype; }

void Printer::set_duplex() {
  switch (printer_info->pDevMode->dmDuplex) {
    case DMDUP_SIMPLEX:
      duplex = L"Nicht aktiv";
      break;
    case DMDUP_HORIZONTAL:
      duplex = L"Aktiv - Horizontaler Duplex-Druck";
      break;
    case DMDUP_VERTICAL:
      duplex = L"Aktiv - Vertikaler Duplex-Druck";
      break;
    default:
      duplex = L"Status unbekannt";
      break;
  }
}

void Printer::set_keep_printjobs() {
  if (printer_info->Attributes & PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS) {
    keep_printjobs = L"nach dem Drucken nicht löschen";
  } else {
    keep_printjobs = L"nach dem Drucken löschen";
  }
}

void Printer::set_status() {
  if (printer_info->Status & PRINTER_STATUS_OFFLINE) {
    status += L"Offline\n";
  }

  if (printer_info->Status & PRINTER_STATUS_WAITING) {
    status += L"Drucker wartet\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PRINTING) {
    status += L"Druckvorgang\n";
  }

  if (printer_info->Status & PRINTER_STATUS_IO_ACTIVE) {
    status += L"Input/Output aktiv\n";
  }

  if (printer_info->Status & PRINTER_STATUS_ERROR) {
    status += L"Error\n";
  }

  if (printer_info->Status & PRINTER_STATUS_TONER_LOW) {
    status += L"Druckerpatronen schwach\n";
  }

  if (printer_info->Status & PRINTER_STATUS_NO_TONER) {
    status += L"Druckerpatronen leer\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PAPER_PROBLEM) {
    status += L"Papierproblem\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PAPER_OUT) {
    status += L"Papier leer\n";
  }

  if (printer_info->Status & PRINTER_STATUS_OUT_OF_MEMORY) {
    status += L"Speicher-Problem\n";
  }

  if (printer_info->Status & PRINTER_STATUS_SERVER_UNKNOWN) {
    status += L"Unbekannt\n";
  }

  if (printer_info->Status & PRINTER_STATUS_BUSY) {
    status += L"Beschäftigt\n";
  }

  if (printer_info->Status & PRINTER_STATUS_DOOR_OPEN) {
    status += L"Druckertür ist offen\n";
  }

  if (printer_info->Status & PRINTER_STATUS_INITIALIZING) {
    status += L"Initialisiert\n";
  }

  if (printer_info->Status & PRINTER_STATUS_NOT_AVAILABLE) {
    status += L"Nicht verfügbar\n";
  }

  if (printer_info->Status & PRINTER_STATUS_OUTPUT_BIN_FULL) {
    status += L"Ausgabebehälter ist voll\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PAGE_PUNT) {
    status += L"Aktuelle Seite kann nicht gedruckt werden\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PAPER_JAM) {
    status += L"Papierstau\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PAUSED) {
    status += L"Pausiert\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PAGE_PUNT) {
    status += L"Aktuelle Seite kann nicht gedruckt werden\n";
  }

  if (printer_info->Status & PRINTER_STATUS_POWER_SAVE) {
    status += L"Energiesparmdous\n";
  }

  if (printer_info->Status & PRINTER_STATUS_PROCESSING) {
    status += L"Druckauftrag wird bearbeitet\n";
  }

  if (printer_info->Status & PRINTER_STATUS_WAITING) {
    status += L"Wartet\n";
  }

  if (printer_info->Status & PRINTER_STATUS_WARMING_UP) {
    status += L"Aufwärmen\n";
  }
}

/**
  Getter functions
*/
int Printer::get_number_printers() { return number_printers; }

std::wstring Printer::get_name() {
  return name;
}

std::wstring Printer::get_type() {
  return type;
}

std::wstring Printer::get_port() {
  return port;
}

std::wstring Printer::get_is_shared() {
  return is_shared;
}

std::wstring Printer::get_sharename() {
  return sharename;
}

std::wstring Printer::get_server_name() {
  return server_name;
}

std::wstring Printer::get_terminalserver() {
  return terminalserver;
}

std::wstring Printer::get_driver() {
  return driver;
}

std::wstring Printer::get_printprocessor() {
  return printprocessor;
}

std::wstring Printer::get_datatype() {
  return datatype;
}

std::wstring Printer::get_duplex() {
  return duplex;
}

std::wstring Printer::get_keep_printjobs() {
  return keep_printjobs;
}

std::wstring Printer::get_status() {
  return status;
}

}  // namespace WinApi