#pragma once

/**
  Exclude rarely-used stuff from Windows headers
  #define WIN32_LEAN_AND_MEAN
*/

#include "winapi_printjob.hpp"

#include <Windows.h>
#include <Winspool.h>
#include <memory>
#include <string>
#include <vector>

namespace winapi {

    /**
      Class to represent a printer.
      The printer holds many attributes defining it.
      The attributes are defined by the informations gathered
      through the win-api.
    */
    class Printer {
    public:
        /**
        * Friends
        */
        friend class Printjob;
        friend std::wostream& operator<<(std::wostream& stream, Printer& printer);

        /**
         Constructor for Printer class.
         Calls init() function.

         @param printer_info_ Const reference to printer_info structure.
       */
        Printer(const _PRINTER_INFO_2W& printer_info);

        /**
          Destructor for Printer class.
          Destroys printer_info_list object.
        */
        ~Printer();

        /**
          Returns total number of printers.

          @return The total number of printers.
        */
        static int NumberOfPrinters();

        /**
          Fills the printer_list vector with printer objects.
        */
        static const std::vector<std::unique_ptr<Printer>>& LoadPrinters();

        /**
          Getter functions
        */
        const _PRINTER_INFO_2W& PrinterInfo() const;
        const std::wstring& Name() const;
        const std::wstring& Type() const;
        const std::wstring& Port() const;
        const std::wstring& IsShared() const;
        const std::wstring& ShareName() const;
        const std::wstring& ServerName() const;
        const std::wstring& TerminalServer() const;
        const std::wstring& Driver() const;
        const std::wstring& PrintProcessor() const;
        const std::wstring& DataType() const;
        const std::wstring& Duplex() const;
        const std::wstring& KeepsPrintjobs() const;
        const std::wstring& Status() const;

    private:
        /**
          Points an array of printer_info structures to
          static member printer_info_list.

          @return Returns bool result if retrieving printer_info_list was successful
        */
        static BOOL setPrinterInfoList();

        /**
         Object counter
        */
        // static unsigned m_obj_counter;

        /**
          Total number of printers
        */
        static int m_number_printers;

        /**
          Vector of printer objects
        */
        static std::vector<std::unique_ptr<Printer>> m_printer_list;

        /**
          Pointer to array of printer_info structures.
        */
        // static _PRINTER_INFO_2W *m_printer_info_list;
        static std::unique_ptr<_PRINTER_INFO_2W[]> m_printer_info_list;

        /**
          printer_info2w structure containing all informations
          about the printer.
        */
        _PRINTER_INFO_2W m_printer_info;

        // Printjobs
        unsigned m_printjobs_count;

        std::vector<Printjob> m_printjobs;

        /**
         Defines all attributes of printer object with the values found in
         the printer_info structure.
        */
        void init();

        /**
          Setter functions
        */
        void setName();
        void setType();
        void setPort();
        void setIsShared();
        void setShareName();
        void setServerName();
        void setTerminalServer();
        void setDriver();
        void setPrintProcessor();
        void setDataType();
        void setDuplex();
        void setStatus();
        void setKeepsPrintjobs();
        void setPrintjobsCount();
        void setPrintjobs();

        /**
          The printer's name.
        */
        std::wstring m_name;

        /**
          Defines whether it's a network or local printer.
        */
        std::wstring m_type;

        /**
          The port through which the printer is connected.
        */
        std::wstring m_port;

        /**
          Defines whether the printer is shared in the network or not.
        */
        std::wstring m_is_shared;

        /**
          The printer's sharename.
        */
        std::wstring m_share_name;

        /**
          The server's name through which the printer is reachable.
        */
        std::wstring m_server_name;

        /**
          Defines whether the printer is connected to a terminalserver.
        */
        std::wstring m_terminalserver;

        /**
          The name of the printer's driver.
        */
        std::wstring m_driver;

        /**
          The name of the printer's printprocessor.
        */
        std::wstring m_printprocessor;

        /**
          The printer's datatype.
        */
        std::wstring m_datatype;

        /**
          Defines whether duplex printing is activated or not.
        */
        std::wstring m_duplex;

        /**
          Defines whether the printer is keeping printjobs after they concluded.
        */
        std::wstring m_keep_printjobs;

        /**
          Defines the printer's current status. Works only on specific printers.
        */
        std::wstring m_status;
    };  // class Printer
}  // namespace WinApi