 #include <windows.h>
#include <tchar.h>
#include <iostream>

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

// Forward declarations
void ServiceMain(int argc, char **argv);
void ControlHandler(DWORD request);
void ServiceWork();

bool InstallService()
{
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE); // used in Windows API to represent a handle to a service control manager or a specific service.
    if (!hSCManager)
    {
        std::cerr << "OpenSCManager failed\n";
        return false;
    }

    //     SC_HANDLE OpenSCManagerA(
    //   [in, optional] LPCSTR lpMachineName,    // The name of the target computer. If the pointer is NULL or points to an empty string, the function connects to the service control manager on the local computer.
    //   [in, optional] LPCSTR lpDatabaseName,   // NULL - Database name  NULL means default database (SERVICES_ACTIVE_DATABASE)
    //   [in]           DWORD  dwDesiredAccess   // Desired access rights For a list of access rights, see https://learn.microsoft.com/en-us/windows/desktop/Services/service-security-and-access-rights
    // );

    // here we have opened with right allowing creating new services

    TCHAR szPath[MAX_PATH]; // TCHAR automatically becomes char or wchar_t based on compilation settings.
    if (!GetModuleFileName(NULL, szPath, MAX_PATH))
    {
        std::cerr << "GetModuleFileName failed\n";
        CloseServiceHandle(hSCManager);
        return false;
    }

    // Creates a Windows service entry in the SCM database
    SC_HANDLE hService = CreateService(
        hSCManager,                       //  Handle to Service Control Manager from OpenSCManager()
        _T("MyService"),                  // Service name
        _T("My Self-Installing Service"), // Display name
        SERVICE_ALL_ACCESS,               // Grants full control rights to the service
        SERVICE_WIN32_OWN_PROCESS,        // Service runs as a separate executable (not DLL)
        SERVICE_AUTO_START,               // Service starts automatically at system boot
        SERVICE_ERROR_NORMAL,             // Log errors but continue system startup normally
        szPath,                           // Path to this exe
        NULL,                             // No Load order group (for boot-start drivers)
        NULL,                             // Don't care about tag ID Output parameter for tag ID (used with load order groups)
        NULL,                             // Other services this service depends on  NULL = No dependencies
        NULL,                             // Account name to run service under  NULL = Run as LocalSystem (high privileges)
        NULL                              // Password for the service account NULL = No password (not needed for LocalSystem)

    );

    // _T converts string literals to the appropriate character type based on compilation settings: Wide Strings (wchar_t) or Narrow Strings (char)

    //     SC_HANDLE CreateServiceA(
    //   [in]            SC_HANDLE hSCManager,
    //   [in]            LPCSTR    lpServiceName,
    //   [in, optional]  LPCSTR    lpDisplayName,
    //   [in]            DWORD     dwDesiredAccess,
    //   [in]            DWORD     dwServiceType,
    //   [in]            DWORD     dwStartType,
    //   [in]            DWORD     dwErrorControl,
    //   [in, optional]  LPCSTR    lpBinaryPathName,
    //   [in, optional]  LPCSTR    lpLoadOrderGroup,
    //   [out, optional] LPDWORD   lpdwTagId,
    //   [in, optional]  LPCSTR    lpDependencies,
    //   [in, optional]  LPCSTR    lpServiceStartName,
    //   [in, optional]  LPCSTR    lpPassword
    // );

    if (!hService)
    {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_EXISTS)
        {
            std::cout << "Service already installed.\n";
        }
        else
        {
            std::cerr << "CreateService failed, error: " << err << "\n";
            CloseServiceHandle(hSCManager);
            return false;
        }
    }
    else
    {
        std::cout << "Service installed successfully!\n";
        CloseServiceHandle(hService);
    }

    CloseServiceHandle(hSCManager);
    return true;
}

bool StartServiceByName()
{
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT); // SC_MANAGER_CONNECT typically used in Windows API programming to specify access rights for connecting to a service control manager
    if (!hSCManager)
        return false;

    SC_HANDLE hService = OpenService(hSCManager, _T("MyService"), SERVICE_START); // asking for permission to start the service
    if (!hService)
    {
        CloseServiceHandle(hSCManager);
        return false;
    }

    StartService(hService, // handle to the service you want to start.
                 0,        // number of arguments passed to the service (0 means no arguments).
                 NULL      // array of strings (optional) that are passed to the service when it starts.

    );

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}

int main(int argc, char* argv[])
{
    
    if (argc > 1) 
    {
        // Handle installer/uninstaller logic if user runs "MyService.exe install"
        if (strcmp(argv[1], "install") == 0)
        {
            if (InstallService())
                std::cout << "Service installed successfully.\n";
            else
                std::cerr << "Service installation failed.\n";
            return 0;
        }
        else if (strcmp(argv[1], "start") == 0)
        {
            if (StartServiceByName())
                std::cout << "Service started successfully.\n";
            else
                std::cerr << "Failed to start service.\n";
            return 0;
        }
    }

    // If no arguments: assume SCM is starting us
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        {(LPSTR)"MyService", (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}};

    if (!StartServiceCtrlDispatcher(ServiceTable))
    {
        std::cerr << "StartServiceCtrlDispatcher failed. Error: " << GetLastError() << "\n";
    }

    return 0;
}


//-------------------------------------
// Service main function
//-------------------------------------
void ServiceMain(int argc, char **argv)
{
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    // Specifies that this is a standard Win32 service (not a driver).
    // Other types exist, e.g., kernel drivers, but SERVICE_WIN32 is the usual type for background apps.

    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    // Tells SCM that the service is starting up.
    // This is important so Windows knows the service isn’t fully running yet.

    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN; // Defines which control commands your service can handle:
    ServiceStatus.dwWin32ExitCode = 0;                                                // dwWin32ExitCode = 0 → no errors.
    ServiceStatus.dwCheckPoint = 0;                                                   // SCM needs a way to know the service is still alive.Otherwise, SCM thinks the service hung and may mark it as failed.
    // dwCheckPoint is an integer counter you increment periodically to tell SCM:
    // “I’m still working; don’t think I’ve crashed.”

    ServiceStatus.dwWaitHint = 0; // dwWaitHint → estimated time (ms) the service needs to start. Here it’s 0 for simplicity.

    hStatus = RegisterServiceCtrlHandler("MyService", ControlHandler); // Registers a function to handle service control requests.
    if (!hStatus)
        return;

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);   // SetServiceStatus is a Windows API function that updates the service control manager with the current status of a service.

    ServiceWork(); // Start background task
}

//-------------------------------------
// Handle stop/shutdown commands
//-------------------------------------
void ControlHandler(DWORD request)
{
    switch (request)
    {
    case SERVICE_CONTROL_STOP:     // "net stop MyService" or Services.msc Stop
    case SERVICE_CONTROL_SHUTDOWN:     // System is shutting down
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;     // Update status to STOPPED
        SetServiceStatus(hStatus, &ServiceStatus);       // Tell Windows we're stopped
        exit(0);           // Terminate the service process
        break; 
    default:
        break;
    }
}

//-------------------------------------
// Background work
//-------------------------------------
void ServiceWork()
{
    while (1)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        char buffer[100];
        sprintf(buffer, "Service running at %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
        FILE *f = fopen("C:\\service_log.txt", "a");
        if (f)
        {
            fputs(buffer, f);
            fclose(f);
        }

        Sleep(10000); // 10 seconds
    }
}

