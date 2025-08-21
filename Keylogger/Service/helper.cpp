#include <windows.h>
#include <shellapi.h>
#include <iostream>

BOOL IsRunningAsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;                                       // a pointer to a SID (Security Identifier) representing the Administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY; // NtAuthority → specifies the authority under which the SID is created (in this case, the built-in Windows NT authority).

    // The AllocateAndInitializeSid function allocates and initializes a security identifier (SID) with up to eight subauthorities.
    // Creates a SID structure that uniquely identifies a security principal (like a user or group) in Windows.
    if (AllocateAndInitializeSid(
            &NtAuthority,                // The NT authority the SID belongs to.
            2,                           // For Administrators group, 2 sub-authorities are needed.
            SECURITY_BUILTIN_DOMAIN_RID, // First sub-authority.  Refers to the built-in domain (local machine domain).
            DOMAIN_ALIAS_RID_ADMINS,     // Second sub-authority.  Refers to the Administrators group. Together with the first sub-authority, it uniquely identifies the local Admin group.

            // For SIDs with fewer than 8 sub-authorities, the rest are ignored. Must be set to 0 to satisfy the function signature.

            0,
            0,
            0,
            0,
            0,
            0,
            &adminGroup // A pointer to a variable that receives the pointer to the allocated and initialized SID structure.
            ))
    {
        CheckTokenMembership(NULL, adminGroup, &isAdmin); // Windows API function that determines whether a specified security identifier (SID) is enabled in the access token of a process.
        FreeSid(adminGroup);
    }

    // domain SIDs (like on servers or Active Directory) usually have more sub-authorities because they encode domain info.
    // Normal PCs (standalone machines, not joined to a domain) mostly use shorter SIDs with fewer sub-authorities this is we have set the other sub-authoritie to 0
    return isAdmin;
}

int main()
{
    if (!IsRunningAsAdmin())
    {
        SHELLEXECUTEINFOW sei = {sizeof(sei)};
        sei.lpVerb = L"runas";
        sei.lpFile = L"helper.exe"; // this program itself
        sei.lpParameters = L"";
        sei.nShow = SW_SHOWNORMAL;

        if (!ShellExecuteExW(&sei))
        {
            std::wcerr << L"User canceled UAC\n";
            return 1;
        }
        return 0; // exit non-admin copy
    }

    system("MyService.exe install");
    Sleep(1000);
    system("MyService.exe start");

    Sleep(1000);

    const char *serviceName = "MyService"; // Change this to your service name

    // Open a handle to the Service Control Manager
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM)
    {
        std::cerr << "Failed to open Service Control Manager (Error: "
                  << GetLastError() << ")\n";
        return 1;
    }

    // Open the specific service
    SC_HANDLE hService = OpenService(hSCM, serviceName, SERVICE_QUERY_STATUS);
    if (!hService)
    {
        std::cerr << "Service not found \n";
        CloseServiceHandle(hSCM);
        return 1;
    }

    // Query the status
    SERVICE_STATUS_PROCESS status;
    DWORD bytesNeeded;
    if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO,
                             (LPBYTE)&status, sizeof(status), &bytesNeeded))
    {
        if (status.dwCurrentState == SERVICE_RUNNING)
        {
            std::wcout << L"Service '" << serviceName << L"' is RUNNING.\n";
        }
        else
        {
            std::wcout << L"Service '" << serviceName << L"' is INSTALLED but NOT running.\n";
        }
    }
    else
    {
        std::cerr << "Failed to query service status (Error: "
                  << GetLastError() << ")\n";
    }

    // Cleanup
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    return 0;
}
