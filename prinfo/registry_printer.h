#pragma once

#include <Windows.h>
#include <iostream>

constexpr DWORD max_key_length = 255;
constexpr DWORD max_value_name = 16383;

// DsDriver Werte
const static std::wstring list_dsdriver[] = {L"printLanguage", L"printBinNames",
                                             L"printMediaReady",
                                             L"printOrientationsSupported"};

// DsSpooler Werte
const static std::wstring list_dsspooler[] = {
    L"printerName", L"printShareName",  L"portName", L"driverName",
    L"serverName",  L"shortServerName", L"uNCName",  L"url",
    L"description"};

// PNPData Werte
const static std::wstring list_pnpdata[] = {L"Manufacturer", L"HardwareID"};

// PrinterDriverData Werte
const static std::wstring list_printerdriverdata[] = {
    L"Model", L"TrayFormQueueProp", L"TrayFormTable"};

// Key auslesen
std::wstring read_key(HKEY hkey, std::wstring printer_name, std::wstring subkey,
                      const std::wstring *subkey_list, int list_size);

// HKEY LOCAL_MACHINE
void show_lm_printers(std::wostream &stream = std::wcout);

// HKEY CURRENT_USER
void show_cu_printers(std::wostream &stream = std::wcout);