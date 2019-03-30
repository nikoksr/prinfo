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

namespace WinApi {

    /**
      Class to represent a printer.
      The printer holds many attributes defining it.
      The attributes are defined by the informations gathered
      through the win-api.
    */
    class Printer {
    public:
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
        static int get_number_printers();

        /**
          Fills the printer_list vector with printer objects.
        */
        static const std::vector<std::unique_ptr<Printer>>& load_printers();

        /**
          Getter functions
        */
        const _PRINTER_INFO_2W& get_printer_info() const;
        const std::wstring& get_name() const;
        const std::wstring& get_type() const;
        const std::wstring& get_port() const;
        const std::wstring& get_is_shared() const;
        const std::wstring& get_sharename() const;
        const std::wstring& get_server_name() const;
        const std::wstring& get_terminalserver() const;
        const std::wstring& get_driver() const;
        const std::wstring& get_printprocessor() const;
        const std::wstring& get_datatype() const;
        const std::wstring& get_duplex() const;
        const std::wstring& get_keep_printjobs() const;
        const std::wstring& get_status() const;

        friend std::wostream& operator<<(std::wostream& stream, const WinApi::Printer& printer);

    private:
        /**
         Defines all attributes of printer object with the values found in
         the printer_info structure.
        */
        void init();

        /**
          Points an array of printer_info structures to
          static member printer_info_list.

          @return Returns bool result if retrieving printer_info_list was successful
        */
        static BOOL set_printer_info_list();

        /**
         Object counter
        */
        static unsigned m_obj_counter;

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
        static _PRINTER_INFO_2W *m_printer_info_list;

        /**
          printer_info2w structure containing all informations
          about the printer.
        */
        _PRINTER_INFO_2W m_printer_info;

        /**
          Setter functions
        */
        void set_name();
        void set_type();
        void set_port();
        void set_is_shared();
        void set_sharename();
        void set_server_name();
        void set_terminalserver();
        void set_driver();
        void set_printprocessor();
        void set_datatype();
        void set_duplex();
        void set_keep_printjobs();
        void set_status();
        void set_printjobs();

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

        std::wstring m_number_printjobs;

        std::vector<std::unique_ptr<Printjob>>* m_printjobs = nullptr;

    };  // class Printer
}  // namespace WinApi