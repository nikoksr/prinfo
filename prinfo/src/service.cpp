#include "service.hpp"

namespace winapi {

    Service::Service(std::wstring name) :
        m_name(name),
        m_scmanager_hdl(nullptr),
        m_service_hdl(nullptr),
        m_status(nullptr),
        m_start_time(GetTickCount64()) {}

    Service::~Service() {
        close();
    }

    bool Service::connect() {
        // Close open connections
        if (isConnected()) {
            close();
        }

        // Open manager
        m_scmanager_hdl = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
        if (!m_scmanager_hdl) {
            return false;
        }

        // Open service
        m_service_hdl = OpenServiceW(m_scmanager_hdl, m_name.c_str(), SERVICE_ALL_ACCESS);
        if (!m_service_hdl) {
            CloseServiceHandle(m_scmanager_hdl);
            return false;
        }

        // Set the status
        return setStatus();
    }

    bool Service::start() {
        if (!isConnected()) {
            return false;
        }
        if (isRunning()) {
            return true;
        }

        while (isStartPending()) {
            Sleep(setWaitTime());

            if (isRunning()) {
                return true;
            }
            if (isTimedOut()) {
                return false;
            }
        }

        if (!StartServiceW(m_service_hdl, 0, nullptr)) {
            return false;
        }
        return true;
    }

    bool Service::stop() {
        if (!isConnected()) {
            return false;
        }
        if (isStopped()) {
            return true;
        }

        if (!isStopPending()) {
            if (!stopDependentServices()) {
                return false;
            }
            if (!ControlService(m_service_hdl, SERVICE_CONTROL_STOP, reinterpret_cast<LPSERVICE_STATUS>(&m_status))) {
                return false;
            }
            if (m_status != NO_ERROR) {
                return false;
            }
        }

        while (isStopPending()) {
            Sleep(setWaitTime());

            if (isStopped()) {
                return true;
            }
            if (isTimedOut()) {
                return false;
            }
        }
        return true;
    }

    bool Service::kill() {
        if (!isConnected()) {
            return false;
        }
        if (isStopped()) {
            return true;
        }

        if (!isStopPending()) {
            if (!ControlService(m_service_hdl, SERVICE_CONTROL_STOP, reinterpret_cast<LPSERVICE_STATUS>(&m_status))) {
                return false;
            }
            if (m_status != NO_ERROR) {
                return false;
            }
        }

        while (isStopPending()) {
            Sleep(setWaitTime());

            if (isStopped()) {
                return true;
            }
            if (isTimedOut()) {
                return false;
            }
        }
        return true;
    }

    bool Service::restart() {
        if (!stop()) {
            return false;
        }
        return start();
    }

    bool Service::isConnected() {
        if (m_scmanager_hdl || m_service_hdl) {
            return true;
        }
        return false;
    }

    bool Service::isRunning() {
        if (!m_service_hdl) {
            return false;
        }

        if (!setStatus()) {
            return false;
        }

        // Check if the service is already running
        if (m_status->dwCurrentState == SERVICE_RUNNING) {
            return true;
        }
        return false;
    }

    bool Service::isStartPending() {
        if (!m_service_hdl) {
            return false;
        }

        if (!setStatus()) {
            return false;
        }

        // Check if the service is already running
        if (m_status->dwCurrentState == SERVICE_START_PENDING) {
            return true;
        }
        return false;
    }

    bool Service::isPaused() {
        if (!m_service_hdl) {
            return false;
        }

        if (!setStatus()) {
            return false;
        }

        // Check if the service is already running
        if (m_status->dwCurrentState == SERVICE_PAUSED) {
            return true;
        }
        return false;
    }

    bool Service::isPausePending() {
        if (!m_service_hdl) {
            return false;
        }

        if (!setStatus()) {
            return false;
        }

        // Check if the service is already running
        if (m_status->dwCurrentState == SERVICE_PAUSE_PENDING) {
            return true;
        }
        return false;
    }

    bool Service::isStopped() {
        if (!m_service_hdl) {
            return false;
        }

        if (!setStatus()) {
            return false;
        }

        // Check if the service is already running
        if (m_status->dwCurrentState == SERVICE_STOPPED) {
            return true;
        }
        return false;
    }

    bool Service::isStopPending() {
        if (!m_service_hdl) {
            return false;
        }

        if (!setStatus()) {
            return false;
        }

        // Check if the service is already running
        if (m_status->dwCurrentState == SERVICE_STOP_PENDING) {
            return true;
        }
        return false;
    }


    bool Service::setStatus() {
        if (!isConnected()) {
            return false;
        }

        DWORD buf_size;
        if (!QueryServiceStatusEx(m_service_hdl, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE>(m_status.get()), sizeof(SERVICE_STATUS_PROCESS), &buf_size)) {
            return false;
        }
        return true;
    }

    bool Service::isTimedOut() {
        if (GetTickCount64() - m_start_time > Service::timeout) {
            return true;
        }
        return false;
    }

    DWORD Service::setWaitTime() {
        DWORD wait_time = m_status->dwWaitHint / 10;

        if (wait_time < 1000) {
            wait_time = 1000;
        }
        else if (wait_time > 10000) {
            wait_time = 10000;
        }
        return wait_time;
    }

    // Close the connection to the winapi.
    void Service::close() {
        if (m_scmanager_hdl) { CloseServiceHandle(m_scmanager_hdl); }
        if (m_service_hdl) { CloseServiceHandle(m_service_hdl); }
    }

    bool Service::stopDependentServices() {
        if (!isConnected()) {
            return false;
        }

        DWORD buf_size;
        DWORD dep_count;
        std::unique_ptr<ENUM_SERVICE_STATUSW[]> dependencies = nullptr;

        // Get buffer size. If enum call succeeds there are no dependencies
        if (EnumDependentServicesW(m_service_hdl, SERVICE_ACTIVE, dependencies.get(), 0, &buf_size, &dep_count)) {
            return true;
        }
        if (GetLastError() != ERROR_MORE_DATA) {
            return false; // Unexpected error
        }

        dependencies = std::make_unique<ENUM_SERVICE_STATUSW[]>(dep_count);

        if (!dependencies) {
            return false;
        }

        // Enumerate the dependencies.
        if (!EnumDependentServicesW(m_service_hdl, SERVICE_ACTIVE, dependencies.get(), buf_size, &buf_size, &dep_count)) {
            return false;
        }

        for (DWORD i = 0; i < dep_count; i++) {
            auto dep_status = std::make_unique<ENUM_SERVICE_STATUSW>(dependencies.get()[i]);
            auto dep_service = std::make_unique<Service>(dep_status->lpServiceName);

            if (!dep_service) {
                return false;
            }
            if (!dep_service->kill()) {
                return false;
            }
        }
        return true;
    }
}