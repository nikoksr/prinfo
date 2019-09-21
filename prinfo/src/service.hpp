#pragma once

#include <Windows.h>
#include <string>
#include <memory>

namespace winapi {

    class Service {

    public:
        Service(std::wstring name);
        ~Service();

        // Connect to the winapi.
        bool connect();

        // Start the service.
        bool start();

        // Stop the service cleanly. Stop all its dependent services first.
        bool stop();

        // Kill the service. Like stop but without stopping the dependent services first.
        bool kill();

        // Restart the service.
        bool restart();

    private:
        std::wstring m_name;
        SC_HANDLE m_scmanager_hdl;
        SC_HANDLE m_service_hdl;
        std::unique_ptr<SERVICE_STATUS_PROCESS> m_status;
        ULONGLONG m_start_time;

        static constexpr DWORD timeout = 30000;

        bool isConnected();
        bool isRunning();
        bool isStartPending();
        bool isPaused();
        bool isPausePending();
        bool isStopped();
        bool isStopPending();
        bool isTimedOut();
        DWORD setWaitTime();
        bool setStatus();
        void close();
        bool stopDependentServices();
    };
}